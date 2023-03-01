// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "archive_binsafe.hh"

#include <cstring>
#include <iostream>

namespace phoenix {
	void ArchiveReaderBinsafe::read_header() {
		_m_bs_version = input.get_uint();
		_m_object_count = input.get_uint();
		auto hash_table_offset = input.get_uint();

		{
			input.mark();
			input.position(hash_table_offset);
			auto hash_table_size = input.get_uint();
			_m_hash_table_entries.resize(hash_table_size);

			for (std::uint32_t i = 0; i < hash_table_size; ++i) {
				auto key_length = input.get_ushort();
				auto insertion_index = input.get_ushort();
				auto hash_value = input.get_uint();
				auto key = input.get_string(key_length);

				_m_hash_table_entries[insertion_index] = HashTableEntry {key, hash_value};
			}

			input.reset();
		}
	}

	bool ArchiveReaderBinsafe::read_object_begin(ArchiveObject& obj) {
		if (input.remaining() < 6)
			return false;
		input.mark();

		if (static_cast<ArchiveEntryType>(input.get()) != ArchiveEntryType::STRING) {
			input.reset();
			return false;
		}

		auto line = input.get_string(input.get_ushort());

		// Fail quickly if we know this can't be an object begin
		if (line.length() <= 2) {
			input.reset();
			return false;
		}

		char* it = line.data();
		if (*it++ != '[') {
			input.reset();
			return false;
		}

		auto* object_name = strtok(it, " ]");
		auto* class_name = strtok(nullptr, " ]");
		auto* version = strtok(nullptr, " ]");
		auto* index = strtok(nullptr, " ]");

		if (object_name == nullptr || class_name == nullptr || version == nullptr || index == nullptr) {
			input.reset();
			return false;
		}

		obj.version = atoi(version);
		obj.index = atoi(index);
		obj.object_name = object_name;
		obj.class_name = class_name;
		return true;
	}

	bool ArchiveReaderBinsafe::read_object_end() {
		if (input.remaining() == 0)
			return true;
		if (input.remaining() < 6)
			return false;
		input.mark();

		if (static_cast<ArchiveEntryType>(input.get()) != ArchiveEntryType::STRING) {
			input.reset();
			return false;
		}

		if (input.get_ushort() != 2) {
			input.reset();
			return false;
		}

		if (input.get_string(2) != "[]") {
			input.reset();
			return false;
		}

		return true;
	}

	const std::string& ArchiveReaderBinsafe::get_entry_key() {
		if (static_cast<ArchiveEntryType>(input.get()) != ArchiveEntryType::HASH) {
			throw ParserError {"ArchiveReaderBinsafe", "invalid format"};
		}

		auto hash = input.get_uint();
		return _m_hash_table_entries[hash].key;
	}

	std::string ArchiveReaderBinsafe::read_string() {
		return input.get_string(ensure_entry_meta<ArchiveEntryType::STRING>());
	}

	std::int32_t ArchiveReaderBinsafe::read_int() {
		ensure_entry_meta<ArchiveEntryType::INTEGER>();
		return input.get_int();
	}

	float ArchiveReaderBinsafe::read_float() {
		ensure_entry_meta<ArchiveEntryType::FLOAT>();
		return input.get_float();
	}

	std::uint8_t ArchiveReaderBinsafe::read_byte() {
		ensure_entry_meta<ArchiveEntryType::BYTE>();
		return input.get();
	}

	std::uint16_t ArchiveReaderBinsafe::read_word() {
		ensure_entry_meta<ArchiveEntryType::WORD>();
		return input.get_ushort();
	}

	std::uint32_t ArchiveReaderBinsafe::read_enum() {
		ensure_entry_meta<ArchiveEntryType::ENUM>();
		return input.get_uint();
	}

	bool ArchiveReaderBinsafe::read_bool() {
		ensure_entry_meta<ArchiveEntryType::BOOL>();
		return input.get_uint() != 0;
	}

	glm::u8vec4 ArchiveReaderBinsafe::read_color() {
		ensure_entry_meta<ArchiveEntryType::COLOR>();

		auto b = input.get();
		auto g = input.get();
		auto r = input.get();
		auto a = input.get();

		return {r, g, b, a};
	}

	glm::vec3 ArchiveReaderBinsafe::read_vec3() {
		ensure_entry_meta<ArchiveEntryType::VEC3>();
		return input.get_vec3();
	}

	glm::vec2 ArchiveReaderBinsafe::read_vec2() {
		auto unused = static_cast<std::int32_t>(ensure_entry_meta<ArchiveEntryType::RAW_FLOAT>() - 2 * sizeof(float));

		if (unused < 0) {
			throw ParserError {"ArchiveReaderBinsafe"
			                   "cannot read vec2 (2 * float): not enough space in rawFloat entry."};
		}

		auto c = input.get_vec2();

		// There might be more bytes in this. We'll ignore them.
		input.skip(unused);
		return c;
	}

	AxisAlignedBoundingBox ArchiveReaderBinsafe::read_bbox() {
		auto unused =
		    static_cast<std::int32_t>(ensure_entry_meta<ArchiveEntryType::RAW_FLOAT>() - 3 * 2 * sizeof(float));

		if (unused < 0) {
			throw ParserError {"archive:reader_binsafe",
			                   "cannot read bbox (6 * float): not enough space in rawFloat entry."};
		}

		auto c = AxisAlignedBoundingBox::parse(input);

		// There might be more bytes in this. We'll ignore them.
		input.skip(unused);
		return c;
	}

	glm::mat3x3 ArchiveReaderBinsafe::read_mat3x3() {
		auto unused = static_cast<std::int32_t>(ensure_entry_meta<ArchiveEntryType::RAW>() - 3 * 3 * sizeof(float));

		if (unused < 0) {
			throw ParserError("ArchiveReaderBinsafe", "cannot read mat3x3 (9 * float): not enough space in raw entry.");
		}

		auto v = input.get_mat3x3();
		input.skip(unused);
		return v;
	}

	Buffer ArchiveReaderBinsafe::read_raw_bytes() {
		auto length = ensure_entry_meta<ArchiveEntryType::RAW>();
		return input.extract(length);
	}

	Buffer ArchiveReaderBinsafe::read_raw_bytes(uint32_t size) {
		auto length = ensure_entry_meta<ArchiveEntryType::RAW>();

		if (length < size) {
			throw ParserError {"ArchiveReaderBinsafe", "not enough raw bytes to read!"};
		} else if (length > size) {
			PX_LOGW("read_raw_bytes: reading ", size, " bytes although ", length, " are actually available");
		}

		return input.extract(length);
	}

	void ArchiveReaderBinsafe::skip_entry() {
		auto type = static_cast<ArchiveEntryType>(input.get());

		switch (type) {
		case ArchiveEntryType::STRING:
		case ArchiveEntryType::RAW:
		case ArchiveEntryType::RAW_FLOAT:
			input.skip(input.get_ushort());
			break;
		case ArchiveEntryType::ENUM:
		case ArchiveEntryType::HASH:
		case ArchiveEntryType::INTEGER:
		case ArchiveEntryType::FLOAT:
		case ArchiveEntryType::BOOL:
		case ArchiveEntryType::COLOR:
			(void) input.get_uint();
			break;
		case ArchiveEntryType::BYTE:
			(void) input.get();
			break;
		case ArchiveEntryType::WORD:
			(void) input.get_ushort();
			break;
		case ArchiveEntryType::VEC3:
			(void) input.get_float();
			(void) input.get_float();
			(void) input.get_float();
			break;
		}
	}

	std::variant<ArchiveObject, ArchiveObjectEnd, ArchiveEntry> ArchiveReaderBinsafe::unstable__next() {
		static ArchiveObject obj {};
		if (read_object_begin(obj)) {
			return obj;
		} else if (read_object_end()) {
			return ArchiveObjectEnd {};
		} else {
			ArchiveEntry entry {};
			entry.name = get_entry_key();
			entry.type = static_cast<ArchiveEntryType>(input.get());

			switch (entry.type) {
			case ArchiveEntryType::STRING:
				entry.value = input.get_string(input.get_ushort());
				break;
			case ArchiveEntryType::RAW:
			case ArchiveEntryType::RAW_FLOAT:
				entry.value = input.extract(input.get_ushort());
				break;
			case ArchiveEntryType::ENUM:
				entry.value = input.get_uint();
				break;
			case ArchiveEntryType::HASH:
				entry.value = input.get_uint();
				break;
			case ArchiveEntryType::INTEGER:
				entry.value = input.get_int();
				break;
			case ArchiveEntryType::FLOAT:
				entry.value = input.get_float();
				break;
			case ArchiveEntryType::BOOL:
				entry.value = input.get_uint() != 0;
				break;
			case ArchiveEntryType::COLOR: {
				auto b = input.get();
				auto g = input.get();
				auto r = input.get();
				auto a = input.get();

				entry.value = glm::u8vec4 {r, g, b, a};
				break;
			}
			case ArchiveEntryType::BYTE:
				entry.value = input.get();
				break;
			case ArchiveEntryType::WORD:
				entry.value = input.get_ushort();
				break;
			case ArchiveEntryType::VEC3: {
				auto x = input.get_float();
				auto y = input.get_float();
				auto z = input.get_float();

				entry.value = glm::vec3 {x, y, z};
				break;
			}
			}

			return entry;
		}
	}
} // namespace phoenix
