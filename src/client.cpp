#include <functional>

#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/system_error.hpp>

#include "client.hpp"
#include "impl/serialize.hpp"
#include "impl/deserialize.hpp"

namespace modbus {

namespace {
	// Make a handler that deserializes a messages and passes it to the user callback.
	template<typename T>
	std::function<std::uint8_t const * (std::uint8_t const * start, std::size_t length, tcp_mbap const & header, boost::system::error_code error)>
	make_handler(client::Callback<T> && callback) {
		return [callback] (std::uint8_t const * start, std::size_t length, tcp_mbap const & header, boost::system::error_code error) {
			T response;
			std::uint8_t const * current = start;
			std::uint8_t const * end     = start + length;

			// Pass errors to callback.
			if (error) {
				callback(header, response, error);
				return current;
			}

			// Make sure the message contains atleast a function code.
			if (length < 1) {
				callback(header, response, modbus_error(errc::message_size_mismatch));
				return current;
			}

			// Function codes 128 and above are exception responses.
			if (*current >= 128) {
				callback(header, response, modbus_error(length >= 2 ? errc_t(start[1]) : errc::message_size_mismatch));
				return current;
			}

			// Try to deserialize the PDU.
			current = impl::deserialize(current, end - current, response, error);
			if (error) {
				callback(header, response, error);
				return current;
			}

			// Check response length consistency.
			// Length from the MBAP header includes the unit ID (1 byte) which is part of the MBAP header, not the response ADU.
			if (current - start != header.length - 1) {
				callback(header, response, modbus_error(errc::message_size_mismatch));
				return current;
			}

			callback(header, response, error);
			return current;
		};
	}
}

/// Send a Modbus request to the server.
template<typename T>
void client::send_message(
	std::uint8_t unit,                               ///< The unit identifier of the target device.
	T const & request,                               ///< The application data unit of the request.
	client::Callback<typename T::response> callback  ///< The callback to invoke when the reply arrives.
) {
	strand.dispatch([this, unit, request, callback] () mutable {
		auto handler = make_handler<typename T::response>(std::move(callback));

		tcp_mbap header;
		header.transaction = allocate_transaction(request.function, handler);
		header.protocol    = 0;                    // 0 means Modbus.
		header.length      = request.length() + 1; // Unit ID is also counted in length field.
		header.unit        = unit;

		auto out = std::ostreambuf_iterator<char>(&write_buffer);
		impl::serialize(out, header);
		impl::serialize(out, request);
		flush_write_buffer();
	});

}

/// Construct a client.
client::client(boost::asio::io_service & ios) : strand(ios), socket(ios), resolver(ios) {
	_connected = false;
}

/// Connect to a server.
void client::connect(std::string const & hostname, std::string const & port, std::function<void(boost::system::error_code const &)> callback) {
	tcp::resolver::query query(hostname, port);

	auto handler = strand.wrap(std::bind(&client::on_resolve, this, std::placeholders::_1, std::placeholders::_2, callback));
	resolver.async_resolve(query, handler);
}

/// Disconnect from the server.
void client::close() {
	// Call all remaining transaction handlers with operation_aborted, then clear transactions.
	for (auto & transaction : transactions) transaction.second.handler(nullptr, 0, {}, boost::asio::error::operation_aborted);
	transactions.clear();

	// Shutdown and close socket.
	boost::system::error_code error;
	resolver.cancel();
	socket.shutdown(tcp::socket::shutdown_both, error);
	socket.close(error);
	_connected = false;
}

/// Reset the client.
void client::reset() {
	// Clear buffers.
	read_buffer.consume(read_buffer.size());
	write_buffer.consume(write_buffer.size());
	writing.clear();

	// Old socket may hold now invalid file descriptor.
	socket = boost::asio::ip::tcp::socket(ios());
	_connected = false;
}

/// Read a number of coils from the connected server.
void client::read_coils(std::uint8_t unit, std::uint16_t address, std::uint16_t count, Callback<response::read_coils> const & callback) {
	send_message(unit, request::read_coils{address, count}, callback);
}

/// Read a number of discrete inputs from the connected server.
void client::read_discrete_inputs(std::uint8_t unit, std::uint16_t address, std::uint16_t count, Callback<response::read_discrete_inputs> const & callback) {
	send_message(unit, request::read_discrete_inputs{address, count}, callback);
}

/// Read a number of holding registers from the connected server.
void client::read_holding_registers(std::uint8_t unit, std::uint16_t address, std::uint16_t count, Callback<response::read_holding_registers> const & callback) {
	send_message(unit, request::read_holding_registers{address, count}, callback);
}

/// Read a number of input registers from the connected server.
void client::read_input_registers(std::uint8_t unit, std::uint16_t address, std::uint16_t count, Callback<response::read_input_registers> const & callback) {
	send_message(unit, request::read_input_registers{address, count}, callback);
}

/// Write to a single coil on the connected server.
void client::write_single_coil(std::uint8_t unit, std::uint16_t address, bool value, Callback<response::write_single_coil> const & callback) {
	send_message(unit, request::write_single_coil{address, value}, callback);
}

/// Write to a single register on the connected server.
void client::write_single_register(std::uint8_t unit, std::uint16_t address, std::uint16_t value, Callback<response::write_single_register> const & callback) {
	send_message(unit, request::write_single_register{address, value}, callback);
}

/// Write to a number of coils on the connected server.
void client::write_multiple_coils(std::uint8_t unit, std::uint16_t address, std::vector<bool> values, Callback<response::write_multiple_coils> const & callback) {
	send_message(unit, request::write_multiple_coils{address, values}, callback);
}

/// Write to a number of registers on the connected server.
void client::write_multiple_registers(std::uint8_t unit, std::uint16_t address, std::vector<uint16_t> values, Callback<response::write_multiple_registers> const & callback) {
	send_message(unit, request::write_multiple_registers{address, values}, callback);
}

	/// Perform a masked write to a register on the connected server.
void client::mask_write_register(std::uint8_t unit, std::uint16_t address, std::uint16_t and_mask, std::uint16_t or_mask, Callback<response::mask_write_register> const & callback) {
	send_message(unit, request::mask_write_register{address, and_mask, or_mask}, callback);
}

/// Called when the resolver finished resolving a hostname.
void client::on_resolve(boost::system::error_code const & error, tcp::resolver::iterator iterator, std::function<void(boost::system::error_code const &)> callback) {
	if (error) return callback(error);

	auto handler = strand.wrap(std::bind(&client::on_connect, this, std::placeholders::_1, std::placeholders::_2, callback));
	boost::asio::async_connect(socket, iterator, handler);
}

/// Called when the socket finished connecting.
void client::on_connect(boost::system::error_code const & error, tcp::resolver::iterator iterator, std::function<void(boost::system::error_code const &)> callback) {
	(void) iterator;

	if (callback) callback(error);

	// Start read loop if no error occured.
	if (!error) {
		_connected = true;
		auto handler = strand.wrap(std::bind(&client::on_read, this, std::placeholders::_1, std::placeholders::_2));
		socket.async_read_some(read_buffer.prepare(1024), handler);
	}
}

/// Called when the socket finished a read operation.
void client::on_read(boost::system::error_code const & error, size_t bytes_transferred) {
	if (error) {
		if (on_io_error) on_io_error(error);
		return;
	}

	read_buffer.commit(bytes_transferred);

	// Parse and process all complete messages in the buffer.
	while (process_message());

	// Read more data.
	auto handler = strand.wrap(std::bind(&client::on_read, this, std::placeholders::_1, std::placeholders::_2));
	socket.async_read_some(read_buffer.prepare(1024), handler);
}

/// Called when the socket finished a write operation.
void client::on_write(boost::system::error_code const & error, size_t bytes_transferred) {
	if (error) {
		if (on_io_error) on_io_error(error);
		writing.clear();
		return;
	}

	write_buffer.consume(bytes_transferred);

	if (write_buffer.size()) {
		flush_write_buffer_();
	} else {
		writing.clear();
	}
}

/// Allocate a transaction in the transaction table.
std::uint16_t client::allocate_transaction(std::uint8_t function, Handler handler) {
	std::uint16_t id = ++next_id;
	transactions.insert({id, {function, handler}});
	return id;
}

/// Parse and process a message from the read buffer.
bool client::process_message() {
	/// Modbus/TCP MBAP header is 7 bytes.
	if (read_buffer.size() < 7) return false;

	uint8_t const * data = boost::asio::buffer_cast<uint8_t const *>(read_buffer.data());

	boost::system::error_code error;
	tcp_mbap header;

	data = impl::deserialize(data, read_buffer.size(), header, error);

	// Handle deserialization errors in TCP MBAP.
	// Cant send an error to a specific transaction and can't continue to read from the connection.
	if (error) {
		if (on_io_error) on_io_error(error);
		close();
		return false;
	}

	// Ensure entire message is in buffer.
	if (read_buffer.size() < std::size_t(6 + header.length)) return false;

	auto transaction = transactions.find(header.transaction);
	if (transaction == transactions.end()) {
		// TODO: Transaction not found. Possibly call on_io_error?
		return true;
	}
	data = transaction->second.handler(data, header.length - 1, header, boost::system::error_code());
	transactions.erase(transaction);

	// Remove read data and handled transaction.
	read_buffer.consume(6 + header.length);

	return true;
}

/// Flush the write buffer.
void client::flush_write_buffer_() {
	auto handler = strand.wrap(std::bind(&client::on_write, this, std::placeholders::_1, std::placeholders::_2));
	socket.async_write_some(write_buffer.data(), handler);
}

/// Flush the write buffer.
void client::flush_write_buffer() {
	if (writing.test_and_set()) return;
	flush_write_buffer_();
}


}
