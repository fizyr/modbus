#pragma once
#include <boost/system/error_code.hpp>

namespace modbus {

/// Modbus error code constants.
namespace errc {
	enum errc_t {
		illegal_function                        = 0x01,
		illegal_data_address                    = 0x02,
		illegal_data_value                      = 0x03,
		server_device_failure                   = 0x04,
		acknowledge                             = 0x05,
		server_device_busy                      = 0x06,
		memory_parity_error                     = 0x08,
		gateway_path_unavailable                = 0x0a,
		gateway_target_device_failed_to_respond = 0x0b,

		message_size_mismatch                   = 0x1001,
		message_too_large                       = 0x1002,
		unexpected_function_code                = 0x1003,
		invalid_value                           = 0x1004,
	};
}

/// Enum type for Modbus error codes.
using errc_t = errc::errc_t;

/// The error category for Modbus errors.
boost::system::error_category const & modbus_category();

/// Get an error code for a Modbus error,
inline boost::system::error_code modbus_error(modbus::errc_t error_code) {
	return boost::system::error_code(error_code, modbus_category());
}

}
