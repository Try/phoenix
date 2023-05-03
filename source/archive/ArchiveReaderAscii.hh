// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "phoenix/Archive.hh"

namespace phoenix {
	class ArchiveReaderAscii final : public ArchiveReader {
	public:
		inline ArchiveReaderAscii(Buffer& in, ArchiveHeader&& parent_header)
		    : ArchiveReader(in, std::move(parent_header)) {}

		bool read_object_begin(ArchiveObject& obj) override;
		bool read_object_end() override;
		std::string read_string() override;
		std::int32_t read_int() override;
		float read_float() override;
		std::uint8_t read_byte() override;
		std::uint16_t read_word() override;
		std::uint32_t read_enum() override;
		bool read_bool() override;
		glm::u8vec4 read_color() override;
		glm::vec3 read_vec3() override;
		glm::vec2 read_vec2() override;
		AxisAlignedBoundingBox read_bbox() override;
		glm::mat3x3 read_mat3x3() override;
		Buffer read_raw_bytes() override;
		Buffer read_raw_bytes(uint32_t size) override;

		std::variant<ArchiveObject, ArchiveObjectEnd, ArchiveEntry> unstable__next() override;

	protected:
		void read_header() override;
		void skip_entry() override;

		std::string read_entry(std::string_view type);

	private:
		int32_t _m_objects {0};
	};

} // namespace phoenix
