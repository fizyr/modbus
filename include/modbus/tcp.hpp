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
