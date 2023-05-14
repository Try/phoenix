// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/Output.hh"

#include <cstddef>
#include <glm/gtc/type_ptr.hpp>

#include <ostream>

namespace phoenix {

	namespace detail {
		class VectorOutput : public Output {
		public:
			explicit VectorOutput(std::vector<std::byte>* vec) : _m_out(vec) {}

			void write(std::byte const* bytes, std::size_t length) override {
				_m_out->insert(_m_out->end(), bytes, bytes + length);
			}

		private:
			std::vector<std::byte>* _m_out;
		};

		class StreamOutput : public Output {
		public:
			explicit StreamOutput(std::ostream* stream) : _m_out(stream) {}

			void write(std::byte const* bytes, std::size_t length) override {
				_m_out->write(reinterpret_cast<char const*>(bytes), static_cast<long>(length));
			}

		private:
			std::ostream* _m_out;
		};
	} // namespace detail

	std::unique_ptr<Output> Output::to_vector(std::vector<std::byte>* vector) {
		return std::make_unique<detail::VectorOutput>(vector);
	}

	std::unique_ptr<Output> Output::to_stream(std::ostream* stream) {
		return std::make_unique<detail::StreamOutput>(stream);
	}

	OutputWriter::OutputWriter(Output* out) : _m_output(out) {}

	OutputWriter::OutputWriter(std::unique_ptr<Output>& out) : _m_output(out.get()) {}

	void OutputWriter::write(std::byte const* b, std::size_t length) {
		_m_output->write(b, length);
	}

	void OutputWriter::write_byte(std::byte b) {
		this->write_t(b);
	}

	void OutputWriter::write_byte(std::uint8_t b) {
		this->write_byte(std::byte {b});
	}

	void OutputWriter::write_char(char c) {
		this->write_t(c);
	}

	void OutputWriter::write_short(std::int16_t i) {
		this->write_t(i);
	}

	void OutputWriter::write_ushort(std::uint16_t i) {
		this->write_t(i);
	}

	void OutputWriter::write_int(std::int32_t i) {
		this->write_t(i);
	}

	void OutputWriter::write_uint(std::uint32_t i) {
		this->write_t(i);
	}

	void OutputWriter::write_float(float f) {
		this->write_t(f);
	}

	void OutputWriter::write_string(std::string_view sv) {
		this->write(reinterpret_cast<std::byte const*>(sv.data()), sv.length());
	}

	void OutputWriter::write_line(std::string_view sv) {
		this->write(reinterpret_cast<std::byte const*>(sv.data()), sv.length());
		this->write_char('\n');
	}

	void OutputWriter::write_vec2(glm::vec2 const& v) {
		this->write(reinterpret_cast<std::byte const*>(glm::value_ptr(v)), sizeof(float) * 2);
	}

	void OutputWriter::write_vec3(glm::vec3 const& v) {
		this->write(reinterpret_cast<std::byte const*>(glm::value_ptr(v)), sizeof(float) * 3);
	}

	void OutputWriter::write_mat3(glm::mat3 const& v) {
		this->write(reinterpret_cast<std::byte const*>(glm::value_ptr(v)), sizeof(float) * 3 * 3);
	}

	void OutputWriter::write_mat4(glm::mat4 const& v) {
		this->write(reinterpret_cast<std::byte const*>(glm::value_ptr(v)), sizeof(float) * 4 * 4);
	}

	template <typename T, typename>
	void OutputWriter::write_t(T value) {
		_m_output->write(reinterpret_cast<std::byte*>(&value), sizeof value);
	}
} // namespace phoenix