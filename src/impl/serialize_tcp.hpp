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
#include "tcp.hpp"

namespace modbus {
namespace impl {

/// Serialize a TCP MBAP.
template<typename OutputIterator>
std::size_t serialize(OutputIterator & out, tcp_mbap const & header) {
	std::size_t written = 0;
	written += serialize_be16(out, header.transaction);
	written += serialize_be16(out, header.protocol);
	written += serialize_be16(out, header.length);
	written += serialize_be8 (out, header.unit);
	return written;
}

/// Serialize a TCP PDU.
template<typename OutputIterator, typename T>
std::size_t serialize(OutputIterator & out, tcp_pdu<T> const & pdu) {
	std::size_t written = 0;

	// Serialize MBAP followed by ADU.
	written += serialize(out, static_cast<tcp_mbap const &>(pdu));
	written += serialize(out, static_cast<T        const &>(pdu));

	return written;
}

}}
