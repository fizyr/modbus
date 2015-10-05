#pragma once
#include <cstdint>
#include <vector>

namespace modbus {
namespace impl {

	/// Convert a boolean to a uint16 Modbus representation.
	/**
	 * \return 0xff00 if value is true, 0x0000 otherwise.
	 */
	inline std::uint16_t bool_to_uint16(bool value) {
		return value ? 0xff00 : 0x0000;
	}

	/// Serialize an uint8_t in big endian.
	/**
	 * \return The number of bytes written.
	 */
	template<typename OutputIterator>
	std::size_t serialize_be8(OutputIterator & out, std::uint8_t value) {
		*out++ = value;
		return 1;
	}

	/// Serialize an uint16_t in big endian.
	/**
	 * \return The number of bytes written.
	 */
	template<typename OutputIterator>
	std::size_t serialize_be16(OutputIterator & out, std::uint16_t value) {
		*out++ = value >> 8 & 0xff;
		*out++ = value >> 0 & 0xff;
		return 2;
	}

	/// Serialize a packed list of booleans for Modbus.
	/**
	 * Writes the bits packed in little endian.
	 *
	 * \return The number of bytes written.
	 */
	template<typename OutputIterator>
	std::size_t serialize_bit_list(OutputIterator & out, std::vector<bool> const & values) {
		std::size_t written = 0;

		for (std::size_t start_bit = 0; start_bit < values.size(); start_bit += 8) {
			std::uint8_t byte = 0;
			for (int sub_bit = 0; sub_bit < 8 && start_bit + sub_bit < values.size(); ++sub_bit) {
				byte |= 1 << sub_bit;
			}
			written += serialize_be8(out, byte);
		}

		return written;
	}


	/// Serialize a vector of booleans for a Modbus request message.
	/**
	 * Writes first the number of booleans as uint16,
	 * then the number of bytes as uint8 and finally
	 * all the bits packed in little endian.
	 *
	 * \return The number of bytes written.
	 */
	template<typename OutputIterator>
	std::size_t serialize_bits_request(OutputIterator & out, std::vector<bool> const & values) {
		std::size_t written = 0;

		// Serialize bit count and byte count.
		written += serialize_be16(out, values.size());
		written += serialize_be8(out, (values.size() + 7) / 8);
		written += serialize_bit_list(out, values);

		return written;
	}

	/// Serialize a vector of booleans for a Modbus response message.
	/**
	 * Writes the number of bytes as uint8 followed by
	 * the bits packed in little endian.
	 *
	 * \return The number of bytes written.
	 */
	template<typename OutputIterator>
	std::size_t serialize_bits_response(OutputIterator & out, std::vector<bool> const & values) {
		std::size_t written = 0;

		// Serialize byte count and packed bits.
		written += serialize_be8(out, (values.size() + 7) / 8);
		written += serialize_bit_list(out, values);

		return written;
	}

	/// Serialize a vector of 16 bit words for a Modbus request message.
	/**
	 * Writes first the number of words as uint16,
	 * then the number of bytes as uint8 and finally
	 * all the words in big endian.
	 *
	 * \return The number of bytes written.
	 */
	template<typename OutputIterator>
	std::size_t serialize_words_request(OutputIterator & out, std::vector<std::uint16_t> const & values) {
		std::size_t written = 0;

		// Serialize word count, byte count and data.
		written += serialize_be16(out, values.size());
		written += serialize_be8(out,  values.size() * 2);
		for (auto value : values) written += serialize_be16(out, value);

		return written;
	}

	/// Serialize a vector of 16 bit words for a Modbus reponse message.
	/**
	 * Writes first the number of words as uint16,
	 * then the number of bytes as uint8 and finally
	 * all the words in big endian.
	 *
	 * \return The number of bytes written.
	 */
	template<typename OutputIterator>
	std::size_t serialize_words_response(OutputIterator & out, std::vector<std::uint16_t> const & values) {
		std::size_t written = 0;

		// Serialize byte count and data.
		written += serialize_be8(out, values.size() * 2);
		for (auto value : values) written += serialize_be16(out, value);

		return written;
	}


}}
