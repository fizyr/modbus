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
#include <cstdint>
#include <utility>

namespace modbus {

/// Modbus/TCP application protocol (MBAP) header.
/**
 * The TCP MBAP contains fields specific to Modbus/TCP.
 * Together with a regular Modbus application data unit (ADU) it forms
 * a Modbus/TCP protocol data unit (PDU).
 */
struct tcp_mbap {
	/// Transaction identifier.
	std::uint16_t transaction;

	/// Protocol identifier. Set to 0 for modbus.
	std::uint16_t protocol = 0;

	/// Length of payload + unit identifier.
	std::uint16_t length;

	/// Unit identifier.
	std::uint8_t unit;
};

/// Modbus/TCP protocol data unit (PDU).
/**
 * A Modbus/TCP PDU contains a Modbus/TCP application protocol (MBAP) header
 * and a regular Modbus application data unit (ADU).
 * The MBAP header contains additional data needed for Modbus/TCP.
 */
template<typename T>
struct tcp_pdu : tcp_mbap, T {
	/// Construct a Modbus/TCP PDU from an MBAP header and an ADU.
	tcp_pdu(tcp_mbap const & mbap, T const &  adu) : tcp_mbap(mbap), T(adu) {}

	/// Construct a Modbus/TCP PDU from an MBAP header and an ADU.
	tcp_pdu(tcp_mbap const & mbap, T && adu) : tcp_mbap(mbap), T(std::move(adu)) {}
};
}
