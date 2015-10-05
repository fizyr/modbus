#include <string>
#include "error.hpp"

namespace modbus {

namespace {
	/// Error category for modbus errors.
	class modbus_category_t : public boost::system::error_category {
		/// Get the name of the error category
		char const * name() const noexcept override {
			return "modbus";
		}

		/// Get a descriptive error message for an error code.
		std::string message(int error) const noexcept override {
			switch (errc::errc_t(error)) {
				case errc::illegal_function:                        return "error 01: Illegal function";
				case errc::illegal_data_address:                    return "error 02: Illegal data address";
				case errc::illegal_data_value:                      return "error 03: Illegal data value";
				case errc::server_device_failure:                   return "error 04: Server device failure";
				case errc::acknowledge:                             return "error 05: Acknowledge";
				case errc::server_device_busy:                      return "error 06: Server device busy";
				case errc::memory_parity_error:                     return "error 08: Memory parity error";
				case errc::gateway_path_unavailable:                return "error 10: Gateway path unavailable";
				case errc::gateway_target_device_failed_to_respond: return "error 11: Gateway target device failed to respond";

				case errc::message_size_mismatch:                   return "peer error: message size mismatch";
				case errc::message_too_large:                       return "peer error: message size limit exceeded";
				case errc::unexpected_function_code:                return "peer error: unexpected function code";
				case errc::invalid_value:                           return "peer error: invalid value received";
			}

			return "unknown error: " + std::to_string(error);
		}
	} modbus_category_;
}

/// The error category for modbus errors.
boost::system::error_category const & modbus_category() {
	return modbus_category_;
}


}
