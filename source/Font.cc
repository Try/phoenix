// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/Font.hh"
#include "phoenix/Buffer.hh"

namespace phoenix {
	Font::Font(std::string font_name, std::uint32_t font_height, std::vector<FontGlyph> font_glyphs)
	    : name(std::move(font_name)), height(font_height), glyphs(std::move(font_glyphs)) {}

	Font Font::parse(Buffer& in) {
		try {
			auto version = in.get_line();
			if (version != "1") {
				throw ParserError {"Font", "version mismatch: expected version 1, got " + version};
			}

			auto name = in.get_line(false);
			auto height = in.get_uint();

			std::vector<FontGlyph> glyphs {};
			glyphs.resize(in.get_uint());

			for (auto& g : glyphs) {
				g.width = in.get();
			}

			for (auto& g : glyphs) {
				g.uv[0] = in.get_vec2();
			}

			for (auto& g : glyphs) {
				g.uv[1] = in.get_vec2();
			}

			return Font {
			    name,
			    height,
			    std::move(glyphs),
			};
		} catch (const BufferError& exc) {
			throw ParserError {"Font", exc, "eof reached"};
		}
	}
} // namespace phoenix
