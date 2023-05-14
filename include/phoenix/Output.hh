// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "Phoenix.hh"

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstddef>
#include <memory>
#include <ostream>
#include <vector>

namespace phoenix {
	class PHOENIX_API Output {
	public:
		virtual ~Output() = default;
		virtual void write(std::byte const* bytes, std::size_t length) = 0;

		PHOENIX_API static std::unique_ptr<Output> to_vector(std::vector<std::byte>* vector);
		PHOENIX_API static std::unique_ptr<Output> to_stream(std::ostream* stream);
	};

	class OutputWriter {
	public:
		PHOENIX_API explicit OutputWriter(Output* out);
		PHOENIX_API explicit OutputWriter(std::unique_ptr<Output>& out);
		PHOENIX_API OutputWriter(OutputWriter const&) = delete;
		PHOENIX_API OutputWriter(OutputWriter&&) noexcept = default;

		PHOENIX_API void write(std::byte const* b, std::size_t length);
		PHOENIX_API void write_byte(std::byte b);
		PHOENIX_API void write_byte(std::uint8_t b);
		PHOENIX_API void write_char(char c);
		PHOENIX_API void write_short(std::int16_t i);
		PHOENIX_API void write_ushort(std::uint16_t i);
		PHOENIX_API void write_int(std::int32_t i);
		PHOENIX_API void write_uint(std::uint32_t i);
		PHOENIX_API void write_float(float f);
		PHOENIX_API void write_string(std::string_view sv);
		PHOENIX_API void write_line(std::string_view sv);
		PHOENIX_API void write_vec2(glm::vec2 const& v);
		PHOENIX_API void write_vec3(glm::vec3 const& v);
		PHOENIX_API void write_mat3(glm::mat3 const& v);
		PHOENIX_API void write_mat4(glm::mat4 const& v);

	protected:
		template <typename T,
		          typename = typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value ||
		                                             std::is_same_v<T, std::byte>>>
		PHOENIX_INTERNAL void write_t(T value);

	private:
		Output* _m_output;
	};
} // namespace phoenix
