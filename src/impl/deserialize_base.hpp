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
#include <string>
#include <vector>

#include <boost/system/error_code.hpp>

#include "error.hpp"

namespace modbus {
namespace impl {

	/// Check if length is sufficient.
	/**
	 * Also sets the error code to message_size_mismatch if the size is insufficient,
	 * but only if the error code is empty.
	 *
	 * \return False if the size is not enough or if error code is not empty.
	 */
	inline bool check_length(std::size_t actual, std::size_t needed, boost::system::error_code & error) {
		if (error) {
			return false;
		} else if (actual < needed) {
			error = modbus_error(errc::message_size_mismatch);
			return false;
		} else {
			return true;
		}
	}

	/// Convert a uint16 Modbus boolean to a bool.
	/**
	 * Sets error to invalid_value if input was not a valid Modbus boolean and error was empty.
	 *
	 * \return The parsed boolean.
	 */
	inline bool uint16_to_bool(uint16_t value, boost::system::error_code & error) {
		if (value == 0xff00) return true;
		if (value != 0x0000 && !error) error = boost::system::error_code(errc::invalid_value, modbus_category());
		return false;
	}

	/// Deserialize an uint8_t in big endian.
	/**
	 * \return Iterator past the read sequence.
	 */
	template<typename InputIterator>
	InputIterator deserialize_be8(InputIterator start, std::uint8_t & out) {
		out = *start++;
		return start;
	}

	/// Deserialize an uint16_t in big endian.
	/**
	 * \return Iterator past the read sequence.
	 */
	template<typename InputIterator>
	InputIterator deserialize_be16(InputIterator start, std::uint16_t & out) {
		std::uint16_t result;
		result  = *start++ << 8;
		result |= *start++ << 0;
		out = result;
		return start;
	}

	/// Deserialize a Modbus boolean.
	/**
	 * \return Iterator past the read sequence.
	 */
	template<typename InputIterator>
	InputIterator deserialize_bool(InputIterator start, bool & out, boost::system::error_code & error) {
		std::uint16_t word = 0xbeef;
		deserialize_be16(start, word);
		out = uint16_to_bool(word, error);
		return start;
	}

	/// Parse and check the function code.
	/**
	 * Does not save the parsed function code anywhere.
	 *
	 * Sets the error code to unexpected_function_code if the function code is not the expected code,
	 * but only if the error code is empty.
	 *
	 * \return The input iterator after parsing the function code.
	 */
	template<typename InputIterator>
	InputIterator deserialize_function(InputIterator start, std::uint8_t expected_function, boost::system::error_code & error) {
		std::uint8_t function;
		start = deserialize_be8(start, function);
		if (function != expected_function && !error) error = modbus_error(errc::unexpected_function_code);
		return start;
	}


	/// Reads a Modbus list of bits from a byte sequence.
	/**
	 * Reads the given number of bits packed in little endian.
	 *
	 * Reads nothing if error code contains an error.
	 *
	 * \return Iterator past the read sequence.
	 */
	template<typename InputIterator>
	InputIterator deserialize_bit_list(InputIterator start, std::size_t length, std::size_t bit_count, std::vector<bool> & values, boost::system::error_code & error) {
		// Check available data length.
		if (!check_length(length, (bit_count + 7) / 8, error)) return start;

		// Read bits.
		values.reserve(values.size() + bit_count);
		for (unsigned int start_bit = 0; start_bit < bit_count; start_bit += 8) {
			std::uint8_t byte;
			start = deserialize_be8(start, byte);

			for (unsigned int sub_bit = 0; sub_bit < 8 && start_bit + sub_bit < bit_count; ++sub_bit) {
				values.push_back(byte & 1);
				byte >>= 1;
			}
		}

		return start;
	}

	/// Read a Modbus vector of 16 bit words from a byte sequence.
	/**
	 * Reads the given number of words as 16 bit integers.
	 *
	 * Reads nothing if error code contains an error.
	 *
	 * \return Iterator past the read sequence.
	 */
	template<typename InputIterator>
	InputIterator deserialize_word_list(InputIterator start, std::size_t length, std::size_t word_count, std::vector<std::uint16_t> & values, boost::system::error_code & error) {
		// Check available data length.
		if (!check_length(length, word_count * 2, error)) return start;

		// Read words.
		values.reserve(values.size() + word_count);
		for (unsigned int i = 0; i < word_count; ++i) {
			values.push_back(0);
			start = deserialize_be16(start, values.back());
		}

		return start;
	}

	/// Read a Modbus vector of bits from a byte sequence representing a request message.
	/**
	 * Reads bit count as 16 bit integer, byte count as 8 bit integer and finally the bits packed in little endian.
	 *
	 * Reads nothing if error code contains an error.
	 *
	 * \return Iterator past the read sequence.
	 */
	template<typename InputIterator>
	InputIterator deserialize_bits_request(InputIterator start, std::size_t length, std::vector<bool> & values, boost::system::error_code & error) {
		if (!check_length(length, 3, error)) return start;

		// Read word and byte count.
		std::uint16_t bit_count;
		std::uint8_t  byte_count;
		start = deserialize_be16(start, bit_count);
		start = deserialize_be8 (start, byte_count);

		// Make sure bit and byte count match.
		if (byte_count != (bit_count + 7) / 8) {
			error = modbus_error(errc::message_size_mismatch);
			return start;
		}

		start = deserialize_bit_list(start, length - 3, bit_count, values, error);
		return start;
	}

	/// Read a Modbus vector of bits from a byte sequence representing a response message.
	/**
	 * Reads byte count as 8 bit integer and finally the bits packed in little endian.
	 *
	 * Reads nothing if error code contains an error.
	 *
	 * \return Iterator past the read sequence.
	 */
	template<typename InputIterator>
	InputIterator deserialize_bits_response(InputIterator start, std::size_t length, std::vector<bool> & values, boost::system::error_code & error) {
		if (!check_length(length, 3, error)) return start;

		// Read word and byte count.
		std::uint8_t  byte_count;
		start = deserialize_be8 (start, byte_count);
		start = deserialize_bit_list(start, length - 1, byte_count * 8, values, error);
		return start;
	}

	/// Read a Modbus vector of 16 bit words from a byte sequence representing a request message.
	/**
	 * Reads word count as 16 bit integer, byte count as 8 bit integer and finally the words as 16 bit integers.
	 *
	 * Reads nothing if error code contains an error.
	 *
	 * \return Iterator past the read sequence.
	 */
	template<typename InputIterator>
	InputIterator deserialize_words_request(InputIterator start, std::size_t length, std::vector<std::uint16_t> & values, boost::system::error_code & error) {
		if (!check_length(length, 3, error)) return start;

		// Read word and byte count.
		std::uint16_t word_count;
		std::uint8_t  byte_count;
		start = deserialize_be16(start, word_count);
		start = deserialize_be8 (start, byte_count);

		// Make sure word and byte count match.
		if (byte_count != 2 * word_count) {
			error = modbus_error(errc::message_size_mismatch);
			return start;
		}

		start = deserialize_word_list(start, length - 3, word_count, values, error);
		return start;
	}

	/// Read a Modbus vector of 16 bit words from a byte sequence representing a response message.
	/**
	 * Reads byte count as 8 bit integer and finally the words as 16 bit integers.
	 *
	 * Reads nothing if error code contains an error.
	 *
	 * \return Iterator past the read sequence.
	 */
	template<typename InputIterator>
	InputIterator deserialize_words_response(InputIterator start, std::size_t length, std::vector<std::uint16_t> & values, boost::system::error_code & error) {
		if (!check_length(length, 3, error)) return start;

		// Read word and byte count.
		std::uint8_t  byte_count;
		start = deserialize_be8 (start, byte_count);
		start = deserialize_word_list(start, length - 1, byte_count / 2, values, error);
		return start;
	}

}}
