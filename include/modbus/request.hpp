#pragma once
#include <cstdint>
#include <vector>

#include "functions.hpp"

namespace modbus {

namespace response {
	struct read_coils;
	struct read_discrete_inputs;
	struct read_holding_registers;
	struct read_input_registers;
	struct write_single_coil;
	struct write_single_register;
	struct write_multiple_coils;
	struct write_multiple_registers;
	struct mask_write_register;
}

namespace request {

	/// Message representing a read_coils request.
	struct read_coils {
		/// Response type.
		using response = response::read_coils;

		/// The function code.
		static constexpr std::uint8_t function = functions::read_coils;

		/// The address of the first coil/register to read from.
		std::uint16_t address;

		/// The number of registers/coils to read.
		std::uint16_t count;

		/// The length of the serialized ADU in bytes.
		std::size_t length() const {
			return 5;
		}
	};

	/// Message representing a read_discrete_inputs request.
	struct read_discrete_inputs {
		/// Response type.
		using response = response::read_discrete_inputs;

		/// The function code.
		static constexpr std::uint8_t function = functions::read_discrete_inputs;

		/// The address of the first coil/register to read from.
		std::uint16_t address;

		/// The number of registers/coils to read.
		std::uint16_t count;

		/// The length of the serialized ADU in bytes.
		std::size_t length() const {
			return 5;
		}
	};

	/// Message representing a read_holding_registers request.
	struct read_holding_registers {
		/// Response type.
		using response = response::read_holding_registers;

		/// The function code.
		static constexpr std::uint8_t function = functions::read_holding_registers;

		/// The address of the first coil/register to read from.
		std::uint16_t address;

		/// The number of registers/coils to read.
		std::uint16_t count;

		/// The length of the serialized ADU in bytes.
		std::size_t length() const {
			return 5;
		}
	};

	/// Message representing a read_input_registers request.
	struct read_input_registers {
		/// Response type.
		using response = response::read_input_registers;

		/// The function code.
		static constexpr std::uint8_t function = functions::read_input_registers;

		/// The address of the first coil/register to read from.
		std::uint16_t address;

		/// The number of registers/coils to read.
		std::uint16_t count;

		/// The length of the serialized ADU in bytes.
		std::size_t length() const {
			return 5;
		}
	};

	/// Message representing a write_single_coil request.
	struct write_single_coil {
		/// Response type.
		using response = response::write_single_coil;

		/// The function code.
		static constexpr std::uint8_t function = functions::write_single_coil;

		/// The address of the coil to write to.
		std::uint16_t address;

		/// The value to write.
		bool value;

		/// The length of the serialized ADU in bytes.
		std::size_t length() const {
			return 5;
		}
	};

	/// Message representing a write_single_register request.
	struct write_single_register {
		/// Response type.
		using response = response::write_single_register;

		/// The function code.
		static constexpr std::uint8_t function = functions::write_single_register;

		/// The address of the register to write to.
		std::uint16_t address;

		/// The value to write.
		std::uint16_t value;

		/// The length of the serialized ADU in bytes.
		std::size_t length() const {
			return 5;
		}
	};

	/// Message representing a write_multiple_coils request.
	struct write_multiple_coils {
		/// Response type.
		using response = response::write_multiple_coils;

		/// The function code.
		static constexpr std::uint8_t function = functions::write_multiple_coils;

		/// The address of the first coil to write to.
		std::uint16_t address;

		/// The values to write.
		std::vector<bool> values;

		/// The length of the serialized ADU in bytes.
		std::size_t length() const {
			return 6 + (values.size() + 7) / 8;
		}
	};

	/// Message representing a write_multiple_registers request.
	struct write_multiple_registers {
		/// Response type.
		using response = response::write_multiple_registers;

		/// The function code.
		static constexpr std::uint8_t function = functions::write_multiple_registers;

		/// The address of the first register to write to.
		std::uint16_t address;

		/// The values to write.
		std::vector<std::uint16_t> values;

		/// The length of the serialized ADU in bytes.
		std::size_t length() const {
			return 6 + values.size() * 2;
		}
	};

	/// Message representing a mask_write_register request.
	struct mask_write_register {
		/// Response type.
		using response = response::mask_write_register;

		/// The function code.
		static constexpr std::uint8_t function = functions::mask_write_register;

		/// The address of the register to write to.
		std::uint16_t address;

		/// The mask to AND the register value with.
		std::uint16_t and_mask;

		/// The mask to OR the register value with.
		std::uint16_t or_mask;

		/// The length of the serialized ADU in bytes.
		std::size_t length() const {
			return 7;
		}
	};

}}
