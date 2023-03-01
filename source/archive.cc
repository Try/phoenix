// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/archive.hh"

#include "archive/archive_ascii.hh"
#include "archive/archive_binary.hh"
#include "archive/archive_binsafe.hh"

#include <iostream>

namespace phoenix {
	constexpr std::string_view type_names[] = {
	    "unknown", // ?            = 0x00
	    "string",  // bs_string    = 0x01,
	    "int",     // bs_int       = 0x02,
	    "float",   // bs_float     = 0x03,
	    "byte",    // bs_byte      = 0x04,
	    "word",    // bs_word      = 0x05,
	    "bool",    // bs_bool      = 0x06,
	    "vec3",    // bs_vec3      = 0x07,
	    "color",   // bs_color     = 0x08,
	    "raw",     // bs_raw       = 0x09,
	    "unknown", // ?            = 0x0A
	    "unknown", // ?            = 0x0B
	    "unknown", // ?            = 0x0C
	    "unknown", // ?            = 0x0D
	    "unknown", // ?            = 0x0E
	    "unknown", // ?            = 0x0F
	    "unknown", // bs_raw_float = 0x10,
	    "enum",    // bs_enum      = 0x11,
	    "hash",    // bs_hash      = 0x12,
	};

	ArchiveHeader ArchiveHeader::parse(Buffer& in) {
		try {
			ArchiveHeader header {};

			if (in.get_line() != "ZenGin Archive") {
				throw ParserError {"ArchiveHeader", "magic missing"};
			}

			std::string version = in.get_line();
			if (version.find("ver ") != 0) {
				throw ParserError {"ArchiveHeader", "ver field missing"};
			}
			header.version = std::stoi(version.substr(version.find(' ') + 1));

			header.archiver = in.get_line();

			auto format = in.get_line();
			if (format == "ASCII") {
				header.format = ArchiveFormat::ASCII;
			} else if (format == "BINARY") {
				header.format = ArchiveFormat::BINARY;
			} else if (format == "BIN_SAFE") {
				header.format = ArchiveFormat::BINSAFE;
			}

			std::string save_game = in.get_line();
			if (save_game.find("saveGame ") != 0) {
				throw ParserError {"ArchiveHeader", "saveGame field missing"};
			}
			header.save = std::stoi(save_game.substr(save_game.find(' ') + 1)) != 0;

			std::string optional = in.get_line();
			if (optional.find("date ") == 0) {
				header.date = optional.substr(optional.find(' ') + 1);
				optional = in.get_line();
			}

			if (optional.find("user ") == 0) {
				header.user = optional.substr(optional.find(' ') + 1);
				optional = in.get_line();
			}

			if (optional != "END") {
				throw ParserError {"ArchiveHeader", "first END missing"};
			}

			return header;
		} catch (const BufferError& exc) {
			throw ParserError {"ArchiveHeader", exc, "eof reached"};
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ArchiveHeader", e, "reading int"};
		}
	}

	std::unique_ptr<ArchiveReader> ArchiveReader::open(Buffer& in) {
		auto header = ArchiveHeader::parse(in);
		std::unique_ptr<ArchiveReader> reader;

		if (header.format == ArchiveFormat::ASCII) {
			reader = std::make_unique<ArchiveReaderAscii>(in, std::move(header));
		} else if (header.format == ArchiveFormat::BINARY) {
			reader = std::make_unique<ArchiveReaderBinary>(in, std::move(header));
		} else if (header.format == ArchiveFormat::BINSAFE) {
			reader = std::make_unique<ArchiveReaderBinsafe>(in, std::move(header));
		} else {
			throw ParserError {"ArchiveReader",
			                   "format '" + std::to_string(static_cast<uint32_t>(header.format)) +
			                       "' is not supported"};
		}

		reader->read_header();
		return reader;
	}

	void ArchiveReader::skip_object(bool skip_current) {
		ArchiveObject tmp;
		int32_t level = skip_current ? 1 : 0;

		do {
			if (read_object_begin(tmp)) {
				++level;
			} else if (read_object_end()) {
				--level;
			} else {
				skip_entry();
			}
		} while (level > 0);
	}

	void ArchiveReader::print_structure(bool open_object) {
		this->unstable__visit_objects(
		    open_object,
		    [](const std::optional<ArchiveObject>& obj, const std::optional<ArchiveEntry>& ent) {
			    if (obj) {
				    std::cout << "<object class=\"" << obj->class_name << "\" name=\"" << obj->object_name
				              << "\" version=\"" << obj->version << "\" index=\"" << obj->index << "\">\n";
			    } else if (ent) {
				    std::cout << "<entry name=\"" << ent->name << "\" type=\""
				              << type_names[static_cast<uint8_t>(ent->type)] << "\" ";

				    switch (ent->type) {
				    case ArchiveEntryType::STRING:
					    std::cout << "value=\"" << std::get<std::string>(ent->value) << "\" ";
					    break;
				    case ArchiveEntryType::INTEGER:
					    std::cout << "value=\"" << std::get<int>(ent->value) << "\" ";
					    break;
				    case ArchiveEntryType::FLOAT:
					    std::cout << "value=\"" << std::get<float>(ent->value) << "\" ";
					    break;
				    case ArchiveEntryType::BYTE:
					    std::cout << "value=\"" << static_cast<uint16_t>(std::get<uint8_t>(ent->value)) << "\" ";
					    break;
				    case ArchiveEntryType::WORD:
					    std::cout << "value=\"" << std::get<uint16_t>(ent->value) << "\" ";
					    break;
				    case ArchiveEntryType::BOOL:
					    std::cout << "value=\"" << std::get<bool>(ent->value) << "\" ";
					    break;
				    case ArchiveEntryType::VEC3: {
					    auto v = std::get<glm::vec3>(ent->value);
					    std::cout << "value=\"(" << v.x << ", " << v.y << ", " << v.z << ")\" ";
					    break;
				    }
				    case ArchiveEntryType::COLOR: {
					    auto v = std::get<glm::u8vec4>(ent->value);
					    std::cout << "value=\"(" << v.r << ", " << v.g << ", " << v.b << ", " << v.a << ")\" ";
					    break;
				    }
				    case ArchiveEntryType::RAW:
				    case ArchiveEntryType::RAW_FLOAT:
					    std::cout << "length=\"" << std::get<Buffer>(ent->value).remaining() << "\" ";
					    break;
				    case ArchiveEntryType::ENUM:
					    std::cout << "value=\"" << std::get<uint32_t>(ent->value) << "\" ";
					    break;
				    case ArchiveEntryType::HASH:
					    std::cout << "value=\"" << std::get<uint32_t>(ent->value) << "\" ";
					    break;
				    }

				    std::cout << "/>\n";
			    } else {
				    std::cout << "</object>\n";
			    }
		    });
	}

	void ArchiveReader::unstable__visit_objects(bool open_object, const ArchiveVisitor& cb) {
		std::variant<ArchiveObject, ArchiveObjectEnd, ArchiveEntry> ent;
		int32_t level = open_object ? 1 : 0;

		do {
			ent = unstable__next();

			if (std::holds_alternative<ArchiveObject>(ent)) {
				cb(std::get<ArchiveObject>(ent), std::nullopt);
				++level;
			} else if (std::holds_alternative<ArchiveObjectEnd>(ent)) {
				cb(std::nullopt, std::nullopt);
				--level;
			} else {
				cb(std::nullopt, std::get<ArchiveEntry>(ent));
			}
		} while (level > 0);
	}
} // namespace phoenix
