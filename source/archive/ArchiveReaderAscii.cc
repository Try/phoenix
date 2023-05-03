// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "ArchiveReaderAscii.hh"

#include <charconv>
#include <cstring>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace phoenix {
	static const std::unordered_map<std::string, ArchiveEntryType> type_name_to_enum {
	    {"string", ArchiveEntryType::STRING},
	    {"int", ArchiveEntryType::INTEGER},
	    {"float", ArchiveEntryType::FLOAT},
	    {"byte", ArchiveEntryType::BYTE},
	    {"word", ArchiveEntryType::WORD},
	    {"bool_", ArchiveEntryType::BOOL},
	    {"vec3", ArchiveEntryType::VEC3},
	    {"color", ArchiveEntryType::COLOR},
	    {"raw", ArchiveEntryType::RAW},
	    {"rawFloat", ArchiveEntryType::RAW_FLOAT},
	    {"enum", ArchiveEntryType::ENUM},
	    {"hash", ArchiveEntryType::HASH},
	};

	void ArchiveReaderAscii::read_header() {
		{
			std::string objects = input.get_line();
			if (objects.find("objects ") != 0) {
				throw ParserError {"ArchiveReaderAscii", "objects field missing"};
			}

			try {
				_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
			} catch (std::invalid_argument const& e) {
				throw ParserError {"ArchiveReaderAscii", e, "reading int"};
			}
		}

		if (input.get_line() != "END") {
			throw ParserError {"ArchiveReaderAscii", "second END missing"};
		}
	}

	bool ArchiveReaderAscii::read_object_begin(ArchiveObject& obj) {
		if (input.remaining() < 3)
			return false;

		input.mark();

		auto line = input.get_line();

		// Fail quickly if we know this can't be an object begin
		if (line.length() <= 2) {
			input.reset();
			return false;
		}

		char class_name[128];
		char object_name[128];

		auto parsed_elements =
		    std::sscanf(line.c_str(), "[%127s %127s %hu %u]", object_name, class_name, &obj.version, &obj.index);

		if (parsed_elements != 4) {
			input.reset();
			return false;
		}

		obj.object_name = object_name;
		obj.class_name = class_name;
		return true;
	}

	bool ArchiveReaderAscii::read_object_end() {
		// When there are less than 3 bytes left in the input, this must be the end of the archive.
		if (input.remaining() < 3)
			return true;

		input.mark();
		auto line = input.get_line();

		// Compatibility fix for binary data in ASCII archives.
		// TODO: Optimize using `find_if`!
		while (std::isspace(static_cast<unsigned char>(line[0]))) {
			line = line.substr(1);
		}

		if (line != "[]") {
			input.reset();
			return false;
		}

		return true;
	}

	std::string ArchiveReaderAscii::read_entry(std::string_view type) {
		auto line = input.get_line();
		line = line.substr(line.find('=') + 1);
		auto colon = line.find(':');

		if (line.substr(0, colon) != type) {
			throw ParserError {"ArchiveReaderAscii",
			                   "type mismatch: expected " + std::string {type} + ", got: " + line.substr(0, colon)};
		}

		auto rv = line.substr(colon + 1);
		return rv;
	}

	std::string ArchiveReaderAscii::read_string() {
		return read_entry("string");
	}

	std::int32_t ArchiveReaderAscii::read_int() {
		try {
			return std::stoi(read_entry("int"));
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ArchiveReaderAscii", e, "reading int"};
		}
	}

	float ArchiveReaderAscii::read_float() {
		try {
			return std::stof(read_entry("float"));
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ArchiveReaderAscii", e, "reading int"};
		}
	}

	std::uint8_t ArchiveReaderAscii::read_byte() {
		try {
			return std::stoul(read_entry("int")) & 0xFF;
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ArchiveReaderAscii", e, "reading int"};
		}
	}

	std::uint16_t ArchiveReaderAscii::read_word() {
		try {
			return std::stoul(read_entry("int")) & 0xFF'FF;
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ArchiveReaderAscii", e, "reading int"};
		}
	}

	std::uint32_t ArchiveReaderAscii::read_enum() {
		try {
			return std::stoul(read_entry("enum")) & 0xFFFF'FFFF;
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ArchiveReaderAscii", e, "reading int"};
		}
	}

	bool ArchiveReaderAscii::read_bool() {
		try {
			return std::stoul(read_entry("bool")) != 0;
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ArchiveReaderAscii", e, "reading int"};
		}
	}

	glm::u8vec4 ArchiveReaderAscii::read_color() {
		std::stringstream in {read_entry("color")};

		std::uint16_t r, g, b, a;
		in >> r >> g >> b >> a;
		return glm::u8vec4 {(std::uint8_t) r, (std::uint8_t) g, (std::uint8_t) b, (std::uint8_t) a};
	}

	glm::vec3 ArchiveReaderAscii::read_vec3() {
		std::stringstream in {read_entry("vec3")};
		glm::vec3 v {};

		in >> v.x >> v.y >> v.z;
		return v;
	}

	glm::vec2 ArchiveReaderAscii::read_vec2() {
		std::stringstream in {read_entry("rawFloat")};
		glm::vec2 v {};

		in >> v.x >> v.y;
		return v;
	}

	void ArchiveReaderAscii::skip_entry() {
		(void) input.get_line();
	}

	AxisAlignedBoundingBox ArchiveReaderAscii::read_bbox() {
		std::stringstream in {read_entry("rawFloat")};
		AxisAlignedBoundingBox box {};

		in >> box.min.x >> box.min.y >> box.min.z >> box.max.x >> box.max.y >> box.max.z;
		return box;
	}

	glm::mat3x3 ArchiveReaderAscii::read_mat3x3() {
		auto in = read_entry("raw");

		if (in.length() < 2 /* 2 chars a byte */ * sizeof(float) * 9) {
			throw ParserError {"ArchiveReaderAscii", "raw entry does not contain enough bytes to be a 3x3 matrix"};
		}

		auto beg_it = in.data();

		glm::mat3x3 v {};
		std::uint8_t tmp[4];

		for (int32_t i = 0; i < 9; ++i) {
			std::from_chars(beg_it + 0, beg_it + 2, tmp[0], 16);
			std::from_chars(beg_it + 2, beg_it + 4, tmp[1], 16);
			std::from_chars(beg_it + 4, beg_it + 6, tmp[2], 16);
			std::from_chars(beg_it + 6, beg_it + 8, tmp[3], 16);
			beg_it += 8;

			memcpy(&v[i / 3][i % 3], tmp, sizeof(float));
		}

		return glm::transpose(v);
	}

	Buffer ArchiveReaderAscii::read_raw_bytes() {
		auto in = read_entry("raw");
		auto length = in.length() / 2;

		std::vector<std::byte> out {};
		out.resize(length);

		auto beg_it = in.data();

		for (std::byte& i : out) {
			std::from_chars(beg_it + 0, beg_it + 2, reinterpret_cast<std::uint8_t&>(i), 16);
			beg_it += 2;
		}

		return Buffer::of(std::move(out));
	}

	Buffer ArchiveReaderAscii::read_raw_bytes(uint32_t size) {
		auto in = read_entry("raw");
		auto length = in.length() / 2;

		if (length < size) {
			throw ParserError {"ArchiveReaderAscii", "not enough raw bytes to read!"};
		} else if (length > size) {
			PX_LOGW("read_raw_bytes: reading ", size, " bytes although ", length, " are actually available");
		}

		std::vector<std::byte> out {};
		out.resize(length);

		auto beg_it = in.data();

		for (std::byte& i : out) {
			std::from_chars(beg_it + 0, beg_it + 2, reinterpret_cast<std::uint8_t&>(i), 16);
			beg_it += 2;
		}

		return Buffer::of(std::move(out));
	}

	std::variant<ArchiveObject, ArchiveObjectEnd, ArchiveEntry> ArchiveReaderAscii::unstable__next() {
		static ArchiveObject tmp {};
		if (read_object_begin(tmp)) {
			return tmp;
		} else if (read_object_end()) {
			return ArchiveObjectEnd {};
		} else {
			input.mark();

			ArchiveEntry entry {};

			auto line = input.get_line();
			entry.name = line.substr(line.find('='));

			line = line.substr(line.find('=') + 1);
			entry.type = type_name_to_enum.at(line.substr(0, line.find(':')));

			input.reset();

			switch (entry.type) {
			case ArchiveEntryType::STRING:
				entry.value = read_string();
				break;
			case ArchiveEntryType::INTEGER:
				entry.value = read_int();
				break;
			case ArchiveEntryType::FLOAT:
				entry.value = read_float();
				break;
			case ArchiveEntryType::BYTE:
				entry.value = read_byte();
				break;
			case ArchiveEntryType::WORD:
				entry.value = read_word();
				break;
			case ArchiveEntryType::BOOL:
				entry.value = read_bool();
				break;
			case ArchiveEntryType::VEC3:
				entry.value = read_vec3();
				break;
			case ArchiveEntryType::COLOR:
				entry.value = read_color();
				break;
			case ArchiveEntryType::RAW:
			case ArchiveEntryType::RAW_FLOAT:
				entry.value = read_raw_bytes();
				break;
			case ArchiveEntryType::ENUM:
				entry.value = read_enum();
				break;
			case ArchiveEntryType::HASH:
				entry.value = 0u;
				break;
			}

			return entry;
		}
	}
} // namespace phoenix
