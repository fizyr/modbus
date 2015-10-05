#pragma once

#include "deserialize_base.hpp"
#include "request.hpp"

namespace modbus {
namespace impl {

/// Deserialize a read_coils request.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, request::read_coils & adu, boost::system::error_code & error) {
	if (!check_length(length, 5, error)) return start;
	start = deserialize_be8 (start, adu.function);
	start = deserialize_be16(start, adu.address );
	start = deserialize_be16(start, adu.count   );
	return start;
}

/// Deserialize a read_discrete_inputs request.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, request::read_discrete_inputs & adu, boost::system::error_code & error) {
	if (!check_length(length, 5, error)) return start;
	start = deserialize_be8 (start, adu.function);
	start = deserialize_be16(start, adu.address );
	start = deserialize_be16(start, adu.count   );
	return start;
}

/// Deserialize a read_holding_registers request.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, request::read_holding_registers & adu, boost::system::error_code & error) {
	if (!check_length(length, 5, error)) return start;
	start = deserialize_be8 (start, adu.function);
	start = deserialize_be16(start, adu.address );
	start = deserialize_be16(start, adu.count   );
	return start;
}

/// Deserialize a read_input_registers request.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, request::read_input_registers & adu, boost::system::error_code & error) {
	if (!check_length(length, 5, error)) return start;
	start = deserialize_be8 (start, adu.function);
	start = deserialize_be16(start, adu.address );
	start = deserialize_be16(start, adu.count   );
	return start;
}

/// Deserialize a write_single_coil request.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, request::write_single_coil & adu, boost::system::error_code & error) {
	if (!check_length(length, 5, error)) return start;
	start = deserialize_be8 (start, adu.function);
	start = deserialize_be16(start, adu.address );
	start = deserialize_bool(start, adu.value   );
	return start;
}

/// Deserialize a write_single_register request.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, request::write_single_register & adu, boost::system::error_code & error) {
	if (!check_length(length, 5, error)) return start;
	start = deserialize_be8 (start, adu.function);
	start = deserialize_be16(start, adu.address );
	start = deserialize_be16(start, adu.value   );
	return start;
}

/// Deserialize a write_multiple_coils request.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, request::write_multiple_coils & adu, boost::system::error_code & error) {
	if (!check_length(length, 3, error)) return start;
	start = deserialize_be8 (start, adu.function);
	start = deserialize_be16(start, adu.address );
	start = deserialize_bits_request(start, length - 3, adu.values, error);
	return start;
}

/// Deserialize a write_multiple_registers request.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, request::write_multiple_registers & adu, boost::system::error_code & error) {
	if (!check_length(length, 3, error)) return start;
	start = deserialize_be8 (start, adu.function);
	start = deserialize_be16(start, adu.address );
	start = deserialize_words_request(start, length - 3, adu.values, error);
	return start;
}

/// Deserialize a mask_write_register request.
template<typename InputIterator>
InputIterator deserialize(InputIterator start, std::size_t length, request::mask_write_register & adu, boost::system::error_code & error) {
	if (!check_length(length, 7, error)) return start;
	start = deserialize_be8 (start, adu.function);
	start = deserialize_be16(start, adu.address );
	start = deserialize_be16(start, adu.and_mask);
	start = deserialize_be16(start, adu.or_mask );
	return start;
}

}}
