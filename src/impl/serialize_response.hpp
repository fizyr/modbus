#pragma once

#include "serialize_base.hpp"
#include "response.hpp"

namespace modbus {
namespace impl {

/// Serialize a read_coils response.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, response::read_coils const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out, adu.function);
	written += serialize_bits_response(out, adu.values);
	return written;
}

/// Serialize a read_discrete_inputs response.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, response::read_discrete_inputs const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out, adu.function);
	written += serialize_bits_response(out, adu.values);
	return written;
}

/// Serialize a read_holding_registers response.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, response::read_holding_registers const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out, adu.function);
	written += serialize_words_response(out, adu.values);
	return written;
}

/// Serialize a read_input_registers response.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, response::read_input_registers const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out, adu.function);
	written += serialize_words_response(out, adu.values);
	return written;
}

/// Serialize a write_single_coil response.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, response::write_single_coil const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, bool_to_uint16(adu.value));
	return written;
}

/// Serialize a write_single_register response.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, response::write_single_register const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.value);
	return written;
}

/// Serialize a write_multiple_coils response.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, response::write_multiple_coils const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.count);
	return written;
}

/// Serialize a write_multiple_registers response.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, response::write_multiple_registers const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.count);
	return written;
}

/// Serialize a mask_write_register response.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, response::mask_write_register const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.and_mask);
	written += serialize_be16(out, adu.or_mask);
	return written;
}

}}
