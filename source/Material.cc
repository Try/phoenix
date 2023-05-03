// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/Material.hh"
#include "phoenix/Archive.hh"

#include <sstream>

namespace phoenix {
	static constexpr auto MATERIAL_VERSION_G1_V108k = 17408;

	Material Material::parse(ArchiveReader& in) {
		try {
			Material mat {};

			// name of the material - ignored
			(void) in.read_string();

			ArchiveObject obj {};
			if (!in.read_object_begin(obj)) {
				throw ParserError {"Material", "expected archive object begin which was not found"};
			}

			if (obj.class_name != "zCMaterial") {
				throw ParserError {"Material", "expected archive class zCMaterial; got " + obj.class_name};
			}

			if (obj.version == MATERIAL_VERSION_G1_V108k) {
				mat.name = in.read_string();
				mat.group = static_cast<MaterialGroup>(in.read_enum());
				mat.color = in.read_color();
				mat.smooth_angle = in.read_float();
				mat.texture = in.read_string();

				std::istringstream texture_scale {in.read_string()};
				texture_scale >> mat.texture_scale.x >> mat.texture_scale.y;

				mat.texture_anim_fps = in.read_float();
				mat.texture_anim_map_mode = static_cast<AnimationMapping>(in.read_enum());

				std::istringstream anim_map_dir {in.read_string()};
				anim_map_dir >> mat.texture_anim_map_dir.x >> mat.texture_anim_map_dir.y;

				mat.disable_collision = in.read_bool();
				mat.disable_lightmap = in.read_bool();
				mat.dont_collapse = in.read_bool();
				mat.detail_object = in.read_string();
				mat.default_mapping = in.read_vec2();
				mat.alpha_func = AlphaFunction::DEFAULT;
			} else {
				mat.name = in.read_string();
				mat.group = static_cast<MaterialGroup>(in.read_enum());
				mat.color = in.read_color();
				mat.smooth_angle = in.read_float();
				mat.texture = in.read_string();

				std::istringstream texture_scale {in.read_string()};
				texture_scale >> mat.texture_scale.x >> mat.texture_scale.y;

				mat.texture_anim_fps = in.read_float() / 1000.0f;
				mat.texture_anim_map_mode = static_cast<AnimationMapping>(in.read_enum());

				std::istringstream anim_map_dir {in.read_string()};
				anim_map_dir >> mat.texture_anim_map_dir.x >> mat.texture_anim_map_dir.y;

				mat.disable_collision = in.read_bool();
				mat.disable_lightmap = in.read_bool();
				mat.dont_collapse = in.read_bool();
				mat.detail_object = in.read_string();

				// This section is specific to G2
				mat.detail_texture_scale = in.read_float();
				mat.force_occluder = in.read_bool();
				mat.environment_mapping = in.read_bool();
				mat.environment_mapping_strength = in.read_float();
				mat.wave_mode = static_cast<WaveType>(in.read_enum());
				mat.wave_speed = static_cast<WaveSpeed>(in.read_enum());
				mat.wave_max_amplitude = in.read_float();
				mat.wave_grid_size = in.read_float();
				mat.ignore_sun = in.read_bool();
				mat.alpha_func = static_cast<AlphaFunction>(in.read_enum());

				// The mapping comes last :)
				mat.default_mapping = in.read_vec2();
			}

			if (!in.read_object_end()) {
				PX_LOGW("Material(\"", mat.name, "\"): not fully parsed");
				in.skip_object(true);
			}

			return mat;
		} catch (const BufferError& exc) {
			throw ParserError {"Material", exc, "eof reached"};
		}
	}
} // namespace phoenix
