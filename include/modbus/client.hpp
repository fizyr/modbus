// Copyright (c) 2017, Fizyr (https://fizyr.com)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the copyright holder(s) nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

#include "functions.hpp"
#include "tcp.hpp"
#include "request.hpp"
#include "response.hpp"

namespace modbus {


/// A connection to a Modbus server.
class client  {
public:
	typedef boost::asio::ip::tcp tcp;

	/// Callback type.
	template<typename T>
	using Callback = std::function<void (tcp_mbap const & header, T const & response, boost::system::error_code const &)>;

	/// Callback to invoke for IO errors that cants be linked to a specific transaction.
	/**
	 * Additionally the connection will be closed and every transaction callback will be called with an EOF error.
	 */
	std::function<void (boost::system::error_code const &)> on_io_error;

protected:
	/// Low level message handler.
	using Handler = std::function<std::uint8_t const * (std::uint8_t const * start, std::size_t size, tcp_mbap const & header, boost::system::error_code error)>;

	/// Struct to hold transaction details.
	struct transaction_t {
		std::uint8_t function;
		Handler handler;
	};

	/// Strand to use to prevent concurrent handler execution.
	boost::asio::io_service::strand strand;

	/// The socket to use.
	tcp::socket socket;

	/// The resolver to use.
	tcp::resolver resolver;

	/// Buffer for read operations.
	boost::asio::streambuf read_buffer;

	/// Buffer for write operations.
	boost::asio::streambuf write_buffer;

	/// Output iterator for write buffer.
	std::ostreambuf_iterator<char> output_iterator{&write_buffer};

	/// Transaction table to keep track of open transactions.
	std::map<int, transaction_t> transactions;

	/// Next transaction ID.
	std::uint16_t next_id = 0;

	/// Indicates if a message is currently being written.
	/**
	 * During this time, new messages will be buffered instead.
	 * Only the latest message gets buffered, all older messages are discarded.
	 */
	std::atomic_flag writing{false};

	/// Track connected state of client.
	bool _connected;

public:
	/// Construct a client.
	client(
		boost::asio::io_service & ios ///< The IO service to use.
	);

	/// Get the IO service used by the client.
	boost::asio::io_service & ios() { return socket.get_io_service(); };

	/// Connect to a server.
	void connect(
		std::string const & hostname,                                   ///< The IP address or host name of the server.
		std::string const & port,                                       ///< The port to connect to.
		std::function<void(boost::system::error_code const &)> callback ///< The callback to invoke when the connection is established, or when an error occurs.
	);

	/// Connect to a server at the default Modbus port 502.
	void connect(
		std::string const & hostname,                                   ///< The IP address or host name of the server.
		std::function<void(boost::system::error_code const &)> callback ///< The callback to invoke when the connection is established, or when an error occurs.
	) {
		connect(hostname, "502", callback);
	}

	/// Disconnect from the server.
	/**
	 * Any remaining transaction callbacks will be invoked with an EOF error.
	 */
	void close();

	/// Reset the client.
	/**
	 * Should be called before re-opening a connection after a previous connection was closed.
	 */
	void reset();

	/// Check if the connection to the server is open.
	/**
	 * \return True if the connection to the server is open.
	 */
	bool is_open() {
		return socket.is_open();
	}

	/// Check if the client is connected.
	bool is_connected() {
		return is_open() && _connected;
	}

	/// Read a number of coils from the connected server.
	void read_coils(
		std::uint8_t unit,                                              ///< The Modbus TCP unit to send the command to.
		std::uint16_t address,                                          ///< The address of the first coil to read.
		std::uint16_t count,                                            ///< The number of coils to read.
		Callback<response::read_coils> const & callback                 ///< The callback to invoke when the reply or error arrives.
	);

	/// Read a number of discrete inputs from the connected server.
	void read_discrete_inputs(
		std::uint8_t unit,                                              ///< The Modbus TCP unit to send the command to.
		std::uint16_t address,                                          ///< The address of the first coil to read.
		std::uint16_t count,                                            ///< The number of inputs to read.
		Callback<response::read_discrete_inputs> const & callback       ///< The callback to invoke when the reply or error arrives.
	);

	/// Read a number of holding registers from the connected server.
	void read_holding_registers(
		std::uint8_t unit,                                              ///< The Modbus TCP unit to send the command to.
		std::uint16_t address,                                          ///< The address of the first coil to read.
		std::uint16_t count,                                            ///< The number of registers to read.
		Callback<response::read_holding_registers> const & callback     ///< The callback to invoke when the reply or error arrives.
	);

	/// Read a number of input registers from the connected server.
	void read_input_registers(
		std::uint8_t unit,                                              ///< The Modbus TCP unit to send the command to.
		std::uint16_t address,                                          ///< The address of the first coil to read.
		std::uint16_t count,                                            ///< The number of registers to read.
		Callback<response::read_input_registers> const & callback       ///< The callback to invoke when the reply or error arrives.
	);

	/// Write to a single coil on the connected server.
	void write_single_coil(
		std::uint8_t unit,                                              ///< The Modbus TCP unit to send the command to.
		std::uint16_t address,                                          ///< The address of the coil.
		bool value,                                                     ///< The value to write.
		Callback<response::write_single_coil> const & callback          ///< The callback to invoke when the reply or error arrives.
	);

	/// Write to a single register on the connected server.
	void write_single_register(
		std::uint8_t unit,                                              ///< The Modbus TCP unit to send the command to.
		std::uint16_t address,                                          ///< The address of the register.
		std::uint16_t value,                                            ///< The value to write.
		Callback<response::write_single_register> const & callback      ///< The callback to invoke when the reply or error arrives.
	);

	/// Write to a number of coils on the connected server.
	void write_multiple_coils(
		std::uint8_t unit,                                              ///< The Modbus TCP unit to send the command to.
		std::uint16_t address,                                          ///< The address of the first coil to write.
		std::vector<bool> values,                                       ///< The values to write.
		Callback<response::write_multiple_coils> const & callback       ///< The callback to invoke when the reply or error arrives.
	);

	/// Write to a number of registers on the connected server.
	void write_multiple_registers(
		std::uint8_t unit,                                              ///< The Modbus TCP unit to send the command to.
		std::uint16_t address,                                          ///< The address of the first register to write.
		std::vector<std::uint16_t> values,                              ///< The values to write.
		Callback<response::write_multiple_registers> const & callback   ///< The callback to invoke when the reply or error arrives.
	);

	/// Perform a masked write to a register on the connected server.
	/**
	 * Compliant servers will set the value of the register to:
	 * ((old_value AND and_mask) OR (or_mask AND NOT and_MASK))
	 */
	void mask_write_register(
		std::uint8_t unit,                                         ///< The Modbus TCP unit to send the command to.
		std::uint16_t address,                                     ///< The address of the first register to write.
		std::uint16_t and_mask,                                    ///< The AND mask to apply.
		std::uint16_t or_mask,                                     ///< The OR mask to apply.
		Callback<response::mask_write_register> const & callback   ///< The callback to invoke when the reply or error arrives.
	);

protected:
	/// Called when the resolver finished resolving a hostname.
	void on_resolve(
		boost::system::error_code const & error,                        ///<[in] The error that occured, if any.
		tcp::resolver::iterator iterator,                               ///<[in] The iterator to the first endpoint found by the resolver.
		std::function<void(boost::system::error_code const &)> callback ///<[in] User callback to invoke whent the connection succeeded.
	);

	/// Called when the socket finished connecting.
	void on_connect(
		boost::system::error_code const & error,                        ///<[in] The error that occured, if any.
		tcp::resolver::iterator iterator,                               ///<[in] The iterator to the first endpoint found by the resolver.
		std::function<void(boost::system::error_code const &)> callback ///<[in] User callback to invoke whent the connection succeeded.
	);

	/// Called when the socket finished a read operation.
	void on_read(
		boost::system::error_code const & error, ///<[in] The error that occured, if any.
		std::size_t bytes_transferred            ///<[in] The amount of bytes read from the socket.
	);

	/// Called when the socket finished a write operation.
	void on_write(
		boost::system::error_code const & error, ///<[in] The error that occured, if any.
		std::size_t bytes_transferred            ///<[in] The amount of bytes read from the socket.
	);

	/// Allocate a transaction in the transaction table.
	std::uint16_t allocate_transaction(std::uint8_t function, Handler handler);

	/// Parse and process a message from the read buffer.
	/**
	 * \return True if a message was parsed succesfully, false if there was not enough data.
	 */
	bool process_message();

	/// Flush the write buffer.
	void flush_write_buffer_();

	/// Flush the write buffer.
	/**
	 * Does nothing if a write operation is still busy.
	 * The buffer will be automatically flushed when when the write operation finishes.
	 */
	void flush_write_buffer();

	/// Send a Modbus request to the server.
	template<typename T>
	void send_message(
		std::uint8_t unit,                       ///< The unit identifier of the target device.
		T const & request,                       ///< The application data unit of the request.
		Callback<typename T::response> callback  ///< The callback to invoke when the reply arrives.
	);
};

}
