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
