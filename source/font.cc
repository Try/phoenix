// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/font.hh>

namespace phoenix {
	font::font(std::string font_name, std::uint32_t font_height, std::vector<glyph> font_glyphs)
	    : name(std::move(font_name)), height(font_height), glyphs(std::move(font_glyphs)) {}

	font font::parse(buffer& buf) {
		return phoenix::parse<font>(buf);
	}

	font font::parse(buffer&& buf) {
		return phoenix::parse<font>(buf);
	}

	template <>
	font parse<>(buffer& buf) {
		try {
			auto version = buf.get_line();
			if (version != "1") {
				throw parser_error {"font", "version mismatch: expected version 1, got " + version};
			}

			auto name = buf.get_line(false);
			auto height = buf.get_uint();

			std::vector<glyph> glyphs {};
			glyphs.resize(buf.get_uint());

			for (auto& glyph : glyphs) {
				glyph.width = buf.get();
			}

			for (auto& glyph : glyphs) {
				glyph.uv[0] = buf.get_vec2();
			}

			for (auto& glyph : glyphs) {
				glyph.uv[1] = buf.get_vec2();
			}

			return font {
			    name,
			    height,
			    std::move(glyphs),
			};
		} catch (const buffer_error& exc) {
			throw parser_error {"font", exc, "eof reached"};
		}
	}
} // namespace phoenix
