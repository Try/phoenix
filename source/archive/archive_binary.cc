// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "archive_binary.hh"

#include <stdexcept>

namespace phoenix {
	void ArchiveReaderBinary::read_header() {
		{
			std::string objects = input.get_line();
			if (objects.find("objects ") != 0) {
				throw ParserError {"ArchiveReaderBinary", "objects header field missing"};
			}

			try {
				_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
			} catch (std::invalid_argument const& e) {
				throw ParserError {"ArchiveReaderBinary", e, "reading int"};
			}
		}

		if (input.get_line_and_ignore("\n") != "END") {
			throw ParserError {"ArchiveReaderBinary", "second END missing"};
		}
	}

	bool ArchiveReaderBinary::read_object_begin(ArchiveObject& obj) {
		if (input.remaining() < 12)
			return false;

		auto pos = input.position();
		_m_object_end.push(pos + input.get_uint());

		obj.version = input.get_ushort();
		obj.index = input.get_uint();
		obj.object_name = input.get_line(false);
		obj.class_name = input.get_line(false);
		return true;
	}

	bool ArchiveReaderBinary::read_object_end() {
		if (input.position() == _m_object_end.top()) {
			_m_object_end.pop();
			return true;
		}

		return input.remaining() == 0;
	}

	std::string ArchiveReaderBinary::read_string() {
		return input.get_line(false);
	}

	std::int32_t ArchiveReaderBinary::read_int() {
		return input.get_int();
	}

	float ArchiveReaderBinary::read_float() {
		return input.get_float();
	}

	std::uint8_t ArchiveReaderBinary::read_byte() {
		return input.get();
	}

	std::uint16_t ArchiveReaderBinary::read_word() {
		return input.get_ushort();
	}

	std::uint32_t ArchiveReaderBinary::read_enum() {
		return input.get();
	}

	bool ArchiveReaderBinary::read_bool() {
		return input.get() != 0;
	}

	glm::u8vec4 ArchiveReaderBinary::read_color() {
		auto b = input.get();
		auto g = input.get();
		auto r = input.get();
		auto a = input.get();

		return {r, g, b, a};
	}

	glm::vec3 ArchiveReaderBinary::read_vec3() {
		return input.get_vec3();
	}

	glm::vec2 ArchiveReaderBinary::read_vec2() {
		return input.get_vec2();
	}

	AxisAlignedBoundingBox ArchiveReaderBinary::read_bbox() {
		return AxisAlignedBoundingBox::parse(input);
	}

	glm::mat3x3 ArchiveReaderBinary::read_mat3x3() {
		return input.get_mat3x3();
	}

	Buffer ArchiveReaderBinary::read_raw_bytes() {
		return input.slice();
	}

	Buffer ArchiveReaderBinary::read_raw_bytes(uint32_t size) {
		return input.extract(size);
	}

	void ArchiveReaderBinary::skip_entry() {
		throw ParserError {"ArchiveReader", "cannot skip entry in binary archive"};
	}

	void ArchiveReaderBinary::skip_object(bool skip_current) {
		if (skip_current) {
			input.position(_m_object_end.top());
			_m_object_end.pop();
		} else {
			input.skip(input.get_uint() - 4);
		}
	}

	std::variant<ArchiveObject, ArchiveObjectEnd, ArchiveEntry> ArchiveReaderBinary::unstable__next() {
		throw ParserError {"ArchiveReader", "next() doesn't work for binary archives"};
	}
} // namespace phoenix
