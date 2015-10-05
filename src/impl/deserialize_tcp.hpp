#pragma once
#include "deserialize_base.hpp"
#include "tcp.hpp"

namespace modbus {
namespace impl {

/// Deserialize a TCP MBAP header.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, tcp_mbap & header, boost::system::error_code & error) {
	if (!check_length(length, 7, error)) return start;
	start = deserialize_be16(start, header.transaction);
	start = deserialize_be16(start, header.protocol   );
	start = deserialize_be16(start, header.length     );
	start = deserialize_be8 (start, header.unit       );
	return start;
}

}}
