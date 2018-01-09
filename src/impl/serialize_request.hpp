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

#include "serialize_base.hpp"
#include "request.hpp"

namespace modbus {
namespace impl {

/// Serialize a read_coils request.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, request::read_coils const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.count);
	return written;
}

/// Serialize a read_discrete_inputs request.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, request::read_discrete_inputs const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.count);
	return written;
}

/// Serialize a read_holding_registers request.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, request::read_holding_registers const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.count);
	return written;
}

/// Serialize a read_input_registers request.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, request::read_input_registers const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.count);
	return written;
}

/// Serialize a write_single_coil request.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, request::write_single_coil const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, bool_to_uint16(adu.value));
	return written;
}

/// Serialize a write_single_register request.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, request::write_single_register const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.value);
	return written;
}

/// Serialize a write_multiple_coils request.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, request::write_multiple_coils const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_bits_request(out, adu.values);
	return written;
}

/// Serialize a write_multiple_registers request.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, request::write_multiple_registers const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,   adu.function);
	written += serialize_be16(out,  adu.address);
	written += serialize_words_request(out, adu.values);
	return written;
}

/// Serialize a mask_write_register request.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, request::mask_write_register const & adu) {
	std::size_t written = 0;
	written += serialize_be8(out,  adu.function);
	written += serialize_be16(out, adu.address);
	written += serialize_be16(out, adu.and_mask);
	written += serialize_be16(out, adu.or_mask);
	return written;
}

}}
