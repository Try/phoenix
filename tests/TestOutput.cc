// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include <phoenix/Output.hh>

#include <doctest/doctest.h>

TEST_SUITE("OutputWriter") {
	TEST_CASE("OutputWriter(write)") {
		std::vector<std::byte> storage {};
		auto output = phoenix::Output::to_vector(&storage);

		phoenix::OutputWriter writer {output.get()};

		SUBCASE("write") {
			writer.write(reinterpret_cast<std::byte const*>("Hello, world!"), 13);
			CHECK_EQ(storage.size(), 13);
			CHECK_EQ(storage[0], std::byte {'H'});
			CHECK_EQ(storage[1], std::byte {'e'});
			CHECK_EQ(storage[11], std::byte {'d'});
			CHECK_EQ(storage[12], std::byte {'!'});
		}

		SUBCASE("write_byte") {
			writer.write_byte(std::byte {'H'});
			CHECK_EQ(storage.size(), 1);
			CHECK_EQ(storage[0], std::byte {'H'});
		}

		SUBCASE("write_char") {
			writer.write_char('H');
			CHECK_EQ(storage.size(), 1);
			CHECK_EQ(storage[0], std::byte {'H'});
		}
		SUBCASE("write_short") {
			writer.write_short(0x1234);
			CHECK_EQ(storage.size(), 2);
			CHECK_EQ(storage[0], std::byte {0x34});
			CHECK_EQ(storage[1], std::byte {0x12});
		}

		SUBCASE("write_ushort") {
			writer.write_ushort(0xFEDC);
			CHECK_EQ(storage.size(), 2);
			CHECK_EQ(storage[0], std::byte {0xDC});
			CHECK_EQ(storage[1], std::byte {0xFE});
		}

		SUBCASE("write_int") {
			writer.write_int(0x12345678);
			CHECK_EQ(storage.size(), 4);
			CHECK_EQ(storage[0], std::byte {0x78});
			CHECK_EQ(storage[1], std::byte {0x56});
			CHECK_EQ(storage[2], std::byte {0x34});
			CHECK_EQ(storage[3], std::byte {0x12});
		}

		SUBCASE("write_ushort") {
			writer.write_uint(0xFEDCBA98);
			CHECK_EQ(storage.size(), 4);
			CHECK_EQ(storage[0], std::byte {0x98});
			CHECK_EQ(storage[1], std::byte {0xBA});
			CHECK_EQ(storage[2], std::byte {0xDC});
			CHECK_EQ(storage[3], std::byte {0xFE});
		}

		SUBCASE("write_float") {
			writer.write_float(420.69f);
			CHECK_EQ(storage.size(), 4);
			CHECK_EQ(storage[0], std::byte {0x52});
			CHECK_EQ(storage[1], std::byte {0x58});
			CHECK_EQ(storage[2], std::byte {0xD2});
			CHECK_EQ(storage[3], std::byte {0x43});
		}

		SUBCASE("write_string") {
			writer.write_string("Hello, world!");
			CHECK_EQ(storage.size(), 13);
			CHECK_EQ(storage[0], std::byte {'H'});
			CHECK_EQ(storage[1], std::byte {'e'});
			CHECK_EQ(storage[11], std::byte {'d'});
			CHECK_EQ(storage[12], std::byte {'!'});
		}

		SUBCASE("write_line") {
			writer.write_line("Hello, world!");
			CHECK_EQ(storage.size(), 14);
			CHECK_EQ(storage[0], std::byte {'H'});
			CHECK_EQ(storage[1], std::byte {'e'});
			CHECK_EQ(storage[11], std::byte {'d'});
			CHECK_EQ(storage[12], std::byte {'!'});
			CHECK_EQ(storage[13], std::byte {'\n'});
		}

		SUBCASE("write_vec2") {
			writer.write_vec2({420.69f, 69.42f});
			CHECK_EQ(storage.size(), 4 * 2);
			// TODO
		}

		SUBCASE("write_vec3") {
			writer.write_vec3({420.69f, 69.42f, 0.f});
			CHECK_EQ(storage.size(), 4 * 3);
			// TODO
		}

		SUBCASE("write_mat3") {
			// TODO
		}

		SUBCASE("write_mat4") {
			// TODO
		}
	}
}
