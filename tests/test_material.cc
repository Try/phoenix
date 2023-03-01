// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include <phoenix/archive.hh>
#include <phoenix/material.hh>

#include <doctest/doctest.h>

using namespace phoenix;

TEST_SUITE("Material") {
	TEST_CASE("Material(parse:g1)") {
		auto in = Buffer::mmap("./samples/G1/DEMON_DIE_BODY.MAT");
		auto archive = ArchiveReader::open(in);

		auto material_count = archive->read_int();
		CHECK_EQ(material_count, 1);

		auto m1 = Material::parse(*archive);

		CHECK_EQ(m1.name, "BODY");
		CHECK_EQ(m1.group, MaterialGroup::UNDEFINED);
		CHECK_EQ(m1.color, glm::u8vec4 {115, 91, 77, 255});
		CHECK_EQ(m1.smooth_angle, 60.0f);
		CHECK_EQ(m1.texture, "DEM_BODY_V0.TGA");
		CHECK_EQ(m1.texture_scale, glm::vec2 {512.0f, 512.0f});
		CHECK_EQ(m1.texture_anim_fps, 0.0f);
		CHECK_EQ(m1.texture_anim_map_mode, phoenix::AnimationMapping::NONE);
		CHECK_EQ(m1.texture_anim_map_dir, glm::vec2 {9.9999997e-005, 0.0f});
		CHECK_FALSE(m1.disable_collision);
		CHECK_FALSE(m1.disable_lightmap);
		CHECK_FALSE(m1.dont_collapse);
		CHECK_EQ(m1.detail_object, "");
		CHECK_EQ(m1.default_mapping, glm::vec2 {2.34375f, 2.34375f});

		// The other fields of `material` are specific to Gothic II
	}

	TEST_CASE("Material(parse:g2)") {
		auto in = Buffer::mmap("./samples/G2/DEMON_DIE_BODY.MAT");
		auto archive = ArchiveReader::open(in);

		auto material_count = archive->read_int();
		CHECK_EQ(material_count, 1);

		auto m1 = Material::parse(*archive);

		CHECK_EQ(m1.name, "BODY");
		CHECK_EQ(m1.group, MaterialGroup::UNDEFINED);
		CHECK_EQ(m1.color, glm::u8vec4 {115, 91, 77, 255});
		CHECK_EQ(m1.smooth_angle, 60.0f);
		CHECK_EQ(m1.texture, "DEM_BODY_V0.TGA");
		CHECK_EQ(m1.texture_scale, glm::vec2 {512.0f, 512.0f});
		CHECK_EQ(m1.texture_anim_fps, 0.0f);
		CHECK_EQ(m1.texture_anim_map_mode, phoenix::AnimationMapping::NONE);
		CHECK_EQ(m1.texture_anim_map_dir, glm::vec2 {0.0f, 0.0f});
		CHECK_FALSE(m1.disable_collision);
		CHECK_FALSE(m1.disable_lightmap);
		CHECK_FALSE(m1.dont_collapse);
		CHECK_EQ(m1.detail_object, "");
		CHECK_EQ(m1.default_mapping, glm::vec2 {2.34375f, 2.34375f});
		CHECK_EQ(m1.alpha_func, AlphaFunction::NONE);
		CHECK_EQ(m1.detail_texture_scale, 1.0f);
		CHECK_FALSE(m1.force_occluder);
		CHECK_FALSE(m1.environment_mapping);
		CHECK_EQ(m1.environment_mapping_strength, 1.0f);
		CHECK_EQ(m1.wave_mode, WaveType::NONE);
		CHECK_EQ(m1.wave_speed, WaveSpeed::NORMAL);
		CHECK_FALSE(m1.force_occluder);
		CHECK_EQ(m1.wave_max_amplitude, 30.0);
		CHECK_EQ(m1.wave_grid_size, 100.0f);
		CHECK_FALSE(m1.ignore_sun);
	}
}
