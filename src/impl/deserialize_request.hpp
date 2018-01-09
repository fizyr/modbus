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
