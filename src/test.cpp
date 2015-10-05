#include <iostream>

#include "client.hpp"

modbus::client * client;

void on_io_error(boost::system::error_code const & error) {
	std::cout << "Read error: " << error.message() << "\n";
}

void on_read_reply(modbus::tcp_mbap const & header, modbus::response::read_holding_registers const & response, boost::system::error_code const & error ) {
	(void) header;

	std::cout << "Multiple registers (error " << error.message() << ")\n";
	for (std::size_t i = 0; i < response.values.size(); ++i) {
		std::cout << "\t" << " " << response.values[i] << "\n";
	}
}

void on_write_reply(modbus::tcp_mbap const & header, modbus::response::write_multiple_registers const & response, boost::system::error_code const & error) {
	(void) header;

	std::cout << "Wrote " << response.count << " registers starting at " << response.address <<  " with error " << error.message() << "\n";
	client->read_holding_registers(0, 128, 20, on_read_reply);
}


void on_connect(boost::system::error_code const & error) {
	std::cout << "Connected (error " << error.message() << ").\n";
	client->write_multiple_registers(0, 128, {1234, 4321, 1, 2, 3, 4, 5, 6, 7, 8}, on_write_reply);
}

int main(int argc, char * * argv) {
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <hostname>\n";
		return -1;
	}

	boost::asio::io_service ios;

	std::string hostname = argv[1];

	modbus::client client{ios};
	client.on_io_error = on_io_error;
	::client = &client;

	client.connect(hostname, "502", on_connect);

	ios.run();

}
