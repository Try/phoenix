// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include <phoenix/buffer.hh>
#include <phoenix/texture.hh>

#include <doctest/doctest.h>

using namespace phoenix;

TEST_SUITE("Texture") {
	TEST_CASE("Texture(parse:?)") {
		auto in = Buffer::mmap("./samples/erz.tex");
		auto texture = Texture::parse(in);

		CHECK_EQ(texture.height(), 128);
		CHECK_EQ(texture.width(), 128);

		CHECK_EQ(texture.mipmap_width(0), 128);
		CHECK_EQ(texture.mipmap_height(0), 128);
		CHECK_EQ(texture.mipmap_width(1), 64);
		CHECK_EQ(texture.mipmap_height(1), 64);
		CHECK_EQ(texture.mipmap_width(2), 32);
		CHECK_EQ(texture.mipmap_height(2), 32);
		CHECK_EQ(texture.average_color(), 0xff443a3c);

		CHECK_EQ(texture.ref_height(), 128);
		CHECK_EQ(texture.ref_width(), 128);

		CHECK_EQ(texture.mipmaps(), 5);
		CHECK_EQ(texture.format(), phoenix::tex_dxt1);
	}

	TEST_CASE("Texture(parse:g1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("Texture(parse:g2)" * doctest::skip()) {
		// TODO: Stub
	}
}

// TODO: Check other formats as well (DXT3, DXT5, ...)
