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

#include <iostream>

#include "client.hpp"

modbus::client * client;

void on_io_error(std::error_code const & error) {
	std::cout << "Read error: " << error.message() << "\n";
}

void on_read_reply(modbus::tcp_mbap const & header, modbus::response::read_holding_registers const & response, std::error_code const & error ) {
	(void) header;

	std::cout << "Multiple registers (error " << error.message() << ")\n";
	for (std::size_t i = 0; i < response.values.size(); ++i) {
		std::cout << "\t" << " " << response.values[i] << "\n";
	}
}

void on_write_reply(modbus::tcp_mbap const & header, modbus::response::write_multiple_registers const & response, std::error_code const & error) {
	(void) header;

	std::cout << "Wrote " << response.count << " registers starting at " << response.address <<  " with error " << error.message() << "\n";
	client->read_holding_registers(0, 128, 20, on_read_reply);
}


void on_connect(std::error_code const & error) {
	std::cout << "Connected (error " << error.message() << ").\n";
	client->write_multiple_registers(0, 128, {1234, 4321, 1, 2, 3, 4, 5, 6, 7, 8}, on_write_reply);
}

int main(int argc, char * * argv) {
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <hostname>\n";
		return -1;
	}

	asio::io_service ios;

	std::string hostname = argv[1];

	modbus::client client{ios};
	client.on_io_error = on_io_error;
	::client = &client;

	client.connect(hostname, "502", on_connect);

	ios.run();

}
