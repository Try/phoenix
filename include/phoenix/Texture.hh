// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"

#include <cstdint>
#include <string_view>
#include <vector>

namespace phoenix {
	class Buffer;

	constexpr const std::string_view ZTEX_SIGNATURE = "ZTEX";
	constexpr const std::uint16_t ZTEX_PALETTE_ENTRIES = 0x100;

	/// \brief Texture formats used by the ZenGin.
	enum TextureFormat : uint8_t {
		B8G8R8A8 = 0x0, ///< \brief 32-bit ARGB pixel format with alpha, using 8 bits per channel
		R8G8B8A8 = 0x1, ///< \brief 32-bit ARGB pixel format with alpha, using 8 bits per channel
		A8B8G8R8 = 0x2, ///< \brief 32-bit ARGB pixel format with alpha, using 8 bits per channel
		A8R8G8B8 = 0x3, ///< \brief 32-bit ARGB pixel format with alpha, using 8 bits per channel
		B8G8R8 = 0x4,   ///< \brief 24-bit RGB pixel format with 8 bits per channel
		R8G8B8 = 0x5,   ///< \brief 24-bit RGB pixel format with 8 bits per channel
		A4R4G4B4 = 0x6, ///< \brief 16-bit ARGB pixel format with 4 bits for each channel
		A1R5G5B5 = 0x7, ///< \brief 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved
		                ///< for alpha
		R5G6B5 = 0x8,   ///< \brief 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue
		P8 = 0x9,       ///< \brief 8-bit color indexed
		DXT1 = 0xA,     ///< \brief DXT1 compression texture format
		DXT2 = 0xB,     ///< \brief DXT2 compression texture format
		DXT3 = 0xC,     ///< \brief DXT3 compression texture format
		DXT4 = 0xD,     ///< \brief DXT4 compression texture format
		DXT5 = 0xE,     ///< \brief DXT5 compression texture format

		// Deprecated entries.
		tex_B8G8R8A8 = B8G8R8A8,
		tex_R8G8B8A8 = R8G8B8A8,
		tex_A8B8G8R8 = A8B8G8R8,
		tex_A8R8G8B8 = A8R8G8B8,
		tex_B8G8R8 = B8G8R8,
		tex_R8G8B8 = R8G8B8,
		tex_A4R4G4B4 = A4R4G4B4,
		tex_A1R5G5B5 = A1R5G5B5,
		tex_R5G6B5 = R5G6B5,
		tex_p8 = P8,
		tex_dxt1 = DXT1,
		tex_dxt2 = DXT2,
		tex_dxt3 = DXT3,
		tex_dxt4 = DXT4,
		tex_dxt5 = DXT5,
	};

	/// \brief Simple ARGB quad.
	struct ColorARGB {
		std::uint8_t a, r, g, b;
	};

	/// \brief Represents a ZenGin texture.
	class Texture {
	public:
		Texture(Texture&&) = default;
		Texture(Texture const&) = default;

		Texture& operator=(Texture&&) = default;
		Texture& operator=(Texture const&) = default;

		/// \brief Parses a texture from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed texture.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_API static Texture parse(Buffer& in);

		/// \brief Parses a texture from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed texture.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API inline static Texture parse(Buffer&& in) {
			return parse(in);
		}

		/// \return The format of the texture.
		[[nodiscard]] PHOENIX_API inline TextureFormat format() const noexcept {
			return _m_format;
		}

		/// \return The width in pixels of the first mipmap level.
		[[nodiscard]] PHOENIX_API inline std::uint32_t width() const noexcept {
			return _m_width;
		}

		/// \return The height in pixels of the first mipmap level.
		[[nodiscard]] PHOENIX_API inline std::uint32_t height() const noexcept {
			return _m_height;
		}

		/// \param level The mipmap level to use (beginning from 0).
		/// \return The width in pixels of the given mipmap level.
		[[nodiscard]] PHOENIX_API inline std::uint32_t mipmap_width(std::uint32_t level) const noexcept {
			return _m_width >> level;
		}

		/// \param level The mipmap level to use (beginning from 0).
		/// \return The height in pixels of the given mipmap level.
		[[nodiscard]] PHOENIX_API inline std::uint32_t mipmap_height(std::uint32_t level) const noexcept {
			return _m_height >> level;
		}

		/// \return The width of the texture in-engine.
		[[nodiscard]] PHOENIX_API inline std::uint32_t ref_width() const noexcept {
			return _m_reference_width;
		}

		/// \return The height of the texture in-engine.
		[[nodiscard]] PHOENIX_API inline std::uint32_t ref_height() const noexcept {
			return _m_reference_height;
		}

		/// \return The number of mipmaps of the texture.
		[[nodiscard]] PHOENIX_API inline std::uint32_t mipmaps() const noexcept {
			return _m_mipmap_count;
		}

		/// \return The average color of the texture.
		[[nodiscard]] PHOENIX_API inline std::uint32_t average_color() const noexcept {
			return _m_average_color;
		}

		/// \return The palette of the texture.
		[[nodiscard]] PHOENIX_API inline ColorARGB* palette() const noexcept {
			return (ColorARGB*) _m_palette;
		}

		/// \param mipmap_level The mipmap level to get.
		/// \return The texture data at the given mipmap level.
		[[nodiscard]] PHOENIX_API inline const std::vector<std::uint8_t>&
		data(std::uint32_t mipmap_level = 0) const noexcept {
			return _m_textures.at(_m_mipmap_count - 1 - mipmap_level);
		}

		/// \brief Converts the texture data of the given mipmap-level to RGBA8
		/// \param mipmap_level The mipmap level of the texture to convert
		/// \return The converted texture data.
		/// \attention This method is very expensive as it allocates a new buffer and copies the internal data into it.
		[[nodiscard]] PHOENIX_API std::vector<std::uint8_t> as_rgba8(std::uint32_t mipmap_level = 0) const;

	private:
		Texture() = default;

	private:
		TextureFormat _m_format {};
		ColorARGB _m_palette[ZTEX_PALETTE_ENTRIES] {};
		std::uint32_t _m_width {};
		std::uint32_t _m_height {};
		std::uint32_t _m_reference_width {};
		std::uint32_t _m_reference_height {};
		std::uint32_t _m_mipmap_count {};
		std::uint32_t _m_average_color {};

		// Quirk: largest mipmap (level 0) stored at the end of the vector
		std::vector<std::vector<std::uint8_t>> _m_textures;
	};

	using texture PHOENIX_DEPRECATED("renamed to Texture") = Texture;
	using texture_format PHOENIX_DEPRECATED("renamed to TextureFormat") = TextureFormat;
	using argb PHOENIX_DEPRECATED("renamed to ColorARGB") = ColorARGB;
} // namespace phoenix
