#pragma once

#include <cstdint>

namespace modbus {

struct functions {
	enum function_t {
		read_coils                    = 0x01,
		read_discrete_inputs          = 0x02,
		read_holding_registers        = 0x03,
		read_input_registers          = 0x04,
		write_single_coil             = 0x05,
		write_single_register         = 0x06,
		read_exception_status         = 0x07,
		diagnostic                    = 0x08,
		get_com_event_counter         = 0x0b,
		get_com_event_log             = 0x0c,
		write_multiple_coils          = 0x0f,
		write_multiple_registers      = 0x10,
		report_server_id              = 0x11,
		read_file_record              = 0x14,
		write_file_record             = 0x15,
		mask_write_register           = 0x16,
		read_write_multiple_registers = 0x17,
		read_fifo_record              = 0x18,
	};
};

}
