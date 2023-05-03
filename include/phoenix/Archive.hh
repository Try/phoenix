// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "Buffer.hh"
#include "Math.hh"

#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace phoenix {
	class Buffer;

	enum class ArchiveFormat {
		BINARY = 0,
		BINSAFE = 1,
		ASCII = 2,
		binary PHOENIX_DEPRECATED("renamed to ArchiveFormat::BINARY") = BINARY,
		binsafe PHOENIX_DEPRECATED("renamed to ArchiveFormat::BINSAFE") = BINSAFE,
		ascii PHOENIX_DEPRECATED("renamed to ArchiveFormat::ASCII") = ASCII,
	};

	/// \brief Represents the header of a ZenGin archive.
	struct ArchiveHeader {
		int32_t version;

		/// \brief The type of archiver used to create the archive. Either `zCArchiverGeneric` or `zCArchiverBinSafe`.
		std::string archiver;

		/// \brief The format of the archive.
		ArchiveFormat format;

		/// \brief Whether the archive contains a save-game or not.
		bool save {false};

		/// \brief The user who created the archive.
		std::string user;

		/// \brief The date this archive was created at.
		std::string date;

		/// \brief Reads in an archive_header from the given reader.
		/// \param in The reader to read from.
		/// \return The header read.
		/// \throws ParserError if parsing fails.
		[[nodiscard]] PHOENIX_INTERNAL static ArchiveHeader parse(Buffer& in);
	};

	/// \brief Represents the header of an object stored in a ZenGin archive.
	struct ArchiveObject {
		/// \brief The name of the sub-object used for storing this object in the ZenGin.
		std::string object_name;

		/// \brief The original class name of the object in the ZenGin. Used to identify the type of object.
		std::string class_name;

		/// \brief A version identifier for the object. A way of determining the Gothic version from
		///        this has yet to be discovered.
		std::uint16_t version;

		/// \brief The index of the object in the archive. Unique for each object in an archive.
		std::uint32_t index;
	};

	enum class ArchiveEntryType : uint8_t {
		STRING = 0x1,
		INTEGER = 0x2,
		FLOAT = 0x3,
		BYTE = 0x4,
		WORD = 0x5,
		BOOL = 0x6,
		VEC3 = 0x7,
		COLOR = 0x8,
		RAW = 0x9,
		RAW_FLOAT = 0x10,
		ENUM = 0x11,
		HASH = 0x12,

		string PHOENIX_DEPRECATED("renamed to ArchiveEntryType::STRING") = STRING,
		int_ PHOENIX_DEPRECATED("renamed to ArchiveEntryType::INTEGER") = INTEGER,
		float_ PHOENIX_DEPRECATED("renamed to ArchiveEntryType::FLOAT") = FLOAT,
		byte PHOENIX_DEPRECATED("renamed to ArchiveEntryType::BYTE") = BYTE,
		word PHOENIX_DEPRECATED("renamed to ArchiveEntryType::WORD") = WORD,
		bool_ PHOENIX_DEPRECATED("renamed to ArchiveEntryType::BOOL") = BOOL,
		vec3 PHOENIX_DEPRECATED("renamed to ArchiveEntryType::VEC3") = VEC3,
		color PHOENIX_DEPRECATED("renamed to ArchiveEntryType::COLOR") = COLOR,
		raw PHOENIX_DEPRECATED("renamed to ArchiveEntryType::RAW") = RAW,
		raw_float PHOENIX_DEPRECATED("renamed to ArchiveEntryType::RAW_FLOAT") = RAW_FLOAT,
		enum_ PHOENIX_DEPRECATED("renamed to ArchiveEntryType::ENUM") = ENUM,
		hash PHOENIX_DEPRECATED("renamed to ArchiveEntryType::HASH") = HASH,
	};

	struct ArchiveEntry {
		ArchiveEntryType type;
		std::string name;
		std::variant<std::string, int, float, uint8_t, uint16_t, bool, glm::vec3, glm::u8vec4, Buffer, uint32_t> value;
	};

	struct ArchiveObjectEnd {};

	using ArchiveVisitor = std::function<void(const std::optional<ArchiveObject>&, const std::optional<ArchiveEntry>&)>;

	/// \brief A reader for ZenGin archives.
	class PHOENIX_API ArchiveReader {
	public:
		/// \brief Constructs a new ArchiveReader from the given reader and header.
		/// \note This constructor should never be called explicitly. Use #open instead!
		/// \param in The reader to read from
		/// \param header The header of the archive.
		inline ArchiveReader(Buffer& in, ArchiveHeader&& parent_header) : header(std::move(parent_header)), input(in) {}
		virtual ~ArchiveReader() = default;

		/// \brief Creates a new ArchiveReader from the given buffer.
		/// \param[in,out] in The buffer to use.
		/// \return The new ArchiveReader.
		/// \throws ParserError
		static std::unique_ptr<ArchiveReader> open(Buffer& in);

		/// \brief Tries to read the begin of a new object from the archive.
		///
		/// If a beginning of an object could not be read, the internal buffer is reverted to the state
		/// before the call of the method and the reader may be used as normal.
		///
		/// \param[out] obj The object to store the data in.
		/// \return `true` if the begin of an object was read successfully, `false` if not.
		virtual bool read_object_begin(ArchiveObject& obj) = 0;

		/// \brief Tries to read the end of an object from the archive.
		///
		/// If a end of an object could not be read, the internal buffer is reverted to the state
		/// before the call of the method and the reader may be used as normal.
		///
		/// \return `true` if the end of an object was read successfully, `false` if not.
		virtual bool read_object_end() = 0;

		/// \brief Reads a string value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a string
		virtual std::string read_string() = 0;

		/// \brief Reads an integer value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not an integer
		virtual std::int32_t read_int() = 0;

		/// \brief Reads a float value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a float
		virtual float read_float() = 0;

		/// \brief Reads a byte value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a byte
		virtual std::uint8_t read_byte() = 0;

		/// \brief Reads a word (`uint16_t`) value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a word
		virtual std::uint16_t read_word() = 0;

		/// \brief Reads a enum (`uint32_t`) value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a enum
		virtual std::uint32_t read_enum() = 0;

		/// \brief Reads a bool value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a bool
		virtual bool read_bool() = 0;

		/// \brief Reads a RGBA color value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a color
		virtual glm::u8vec4 read_color() = 0;

		/// \brief Reads a vec3 value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a vec3
		virtual glm::vec3 read_vec3() = 0;

		/// \brief Reads a vec2 value from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a vec2
		virtual glm::vec2 read_vec2() = 0;

		/// \brief Reads a bounding box consisting of two consecutive vec3's from the reader.
		/// \return The value read.
		/// \throws ParserError if the value actually present is not a bounding box
		virtual AxisAlignedBoundingBox read_bbox() = 0;

		/// \brief Reads a 3-by-3 matrix from the reader.
		/// \return The matrix.
		virtual glm::mat3x3 read_mat3x3() = 0;

		/// \brief Reads a raw entry and returns the raw bytes stored within.
		/// \return A vector containing the raw bytes of the entry.
		/// \throws ParserError if the value actually present is not raw
		PHOENIX_DEPRECATED("unsafe api: use read_raw_bytes(uint32_t) instead") virtual Buffer read_raw_bytes() = 0;

		/// \brief Reads a raw entry and returns the raw bytes stored within.
		/// \param size The number of bytes to read (checked at runtime for ASCII and BIN_SAFE archives)
		/// \return A vector containing the raw bytes of the entry.
		/// \throws ParserError if the value actually present is not raw
		virtual Buffer read_raw_bytes(uint32_t size) = 0;

		/// \brief Skips the next object in the reader and all it's children
		/// \param skip_current If `false` skips the next object in this buffer, otherwise skip the object
		///                     currently being read.
		virtual void skip_object(bool skip_current);

		/// \brief Dumps the current or next object of this reader as XML to standard out.
		/// \param open_object If `false`, dumps out the next object in the reader, otherwise dumps all
		///                    values until the current object closes.
		virtual void print_structure(bool open_object);

		/// \brief Get the next element in the archive.
		///
		/// Parses the next element, either an object begin, object end or entry from the archive and returns
		/// its associated value.
		///
		/// \return The value of the next element in the archive.
		/// \warning This API is unstable and may change at any time!
		virtual std::variant<ArchiveObject, ArchiveObjectEnd, ArchiveEntry> unstable__next() = 0;

		/// \brief Walks the current or next object of the archive recursively, calling `cb` for each element.
		/// \param open_object If `false`, walks out the next object in the reader, otherwise walks all
		///                    values until the current object closes.
		/// \param cb A callback function to handle each element.
		/// \warning This API is unstable and may change at any time!
		void unstable__visit_objects(bool open_object, const ArchiveVisitor& cb);

		/// \return The header of the archive
		[[nodiscard]] const ArchiveHeader& get_header() const noexcept {
			return header;
		}

		/// \return Whether or not this archive represents a save-game.
		[[nodiscard]] inline bool is_save_game() const noexcept {
			return header.save;
		}

	protected:
		/// \brief Read the header of the specific archive format.
		virtual void read_header() = 0;

		/// \brief Skips the next entry in the reader.
		virtual void skip_entry() = 0;

	protected:
		ArchiveHeader header;
		Buffer& input;
	};

	using archive_format PHOENIX_DEPRECATED("renamed to ArchiveFormat") = ArchiveFormat;
	using archive_header PHOENIX_DEPRECATED("renamed to ArchiveHeader") = ArchiveHeader;
	using archive_object PHOENIX_DEPRECATED("renamed to ArchiveObject") = ArchiveObject;
	using archive_entry_type PHOENIX_DEPRECATED("renamed to ArchiveEntryType") = ArchiveEntryType;
	using archive_entry PHOENIX_DEPRECATED("renamed to ArchiveEntry") = ArchiveEntry;
	using archive_object_end PHOENIX_DEPRECATED("renamed to ArchiveObjectEnd") = ArchiveObjectEnd;
	using archive_visitor PHOENIX_DEPRECATED("renamed to ArchiveVisitor") = ArchiveVisitor;
	using archive_reader PHOENIX_DEPRECATED("renamed to ArchiveReader") = ArchiveReader;
} // namespace phoenix
