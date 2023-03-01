// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <string>

namespace phoenix {
	class ArchiveReader;

	/// \brief Alpha compositing modes used by the *ZenGin*.
	enum class AlphaFunction {
		DEFAULT = 0,
		NONE = 1,
		BLEND = 2,
		ADD = 3,
		SUBTRACT = 4,
		MULTIPLY = 5,
		MULTIPLY_ALT = 6,

		// Deprecated entries.
		default_ PHOENIX_DEPRECATED("renamed to AlphaFunction::DEFAULT") = DEFAULT,
		none PHOENIX_DEPRECATED("renamed to AlphaFunction::NONE") = NONE,
		blend PHOENIX_DEPRECATED("renamed to AlphaFunction::BLEND") = BLEND,
		add PHOENIX_DEPRECATED("renamed to AlphaFunction::ADD") = ADD,
		sub PHOENIX_DEPRECATED("renamed to AlphaFunction::SUBTRACT") = SUBTRACT,
		mul PHOENIX_DEPRECATED("renamed to AlphaFunction::MULTIPLY") = MULTIPLY,
		mul2 PHOENIX_DEPRECATED("renamed to AlphaFunction::MULTIPLY_ALT") = MULTIPLY_ALT,
	};

	/// \brief The general type of a material.
	enum class MaterialGroup : std::uint8_t {
		/// \brief A material group has not been assigned to the material.
		UNDEFINED = 0,
		METAL = 1,
		STONE = 2,
		WOOD = 3,
		EARTH = 4,
		WATER = 5,
		SNOW = 6,

		/// \brief The material group is explicitly not set. Added for [OpenGothic](https://github.com/Try/OpenGothic)
		///        compatibility. It does not exist in real Gothic or Gothic 2 materials.
		NONE = 0xFF,

		// Deprecated entries.
		undefined PHOENIX_DEPRECATED("renamed to MaterialGroup::UNDEFINED") = UNDEFINED,
		metal PHOENIX_DEPRECATED("renamed to MaterialGroup::METAL") = METAL,
		stone PHOENIX_DEPRECATED("renamed to MaterialGroup::STONE") = STONE,
		wood PHOENIX_DEPRECATED("renamed to MaterialGroup::WOOD") = WOOD,
		earth PHOENIX_DEPRECATED("renamed to MaterialGroup::EARTH") = EARTH,
		water PHOENIX_DEPRECATED("renamed to MaterialGroup::WATER") = WATER,
		snow PHOENIX_DEPRECATED("renamed to MaterialGroup::SNOW") = SNOW,
		none PHOENIX_DEPRECATED("renamed to MaterialGroup::NONE") = NONE,
	};

	/// \brief Wave animation speed modes.
	enum class WaveSpeed : std::uint8_t {
		NONE = 0,
		SLOW = 1,
		NORMAL = 2,
		FAST = 3,

		// Deprecated entries.
		none PHOENIX_DEPRECATED("renamed to WaveSpeed::NONE") = NONE,
		slow PHOENIX_DEPRECATED("renamed to WaveSpeed::SLOW") = SLOW,
		normal PHOENIX_DEPRECATED("renamed to WaveSpeed::NORMAL") = NORMAL,
		fast PHOENIX_DEPRECATED("renamed to WaveSpeed::FAST") = FAST,
	};

	/// \brief Wave animation modes.
	enum class WaveType : std::uint8_t {
		NONE = 0,
		GROUND_AMBIENT = 1,
		GROUND = 2,
		WALL_AMBIENT = 3,
		WALL = 4,
		ENVIRONMENT = 5,
		WIND_AMBIENT = 6,
		WIND = 7,

		// Deprecated entries.
		none PHOENIX_DEPRECATED("renamed to WaveType::NONE") = NONE,
		ambient_ground PHOENIX_DEPRECATED("renamed to WaveType::GROUND_AMBIENT") = GROUND_AMBIENT,
		ground PHOENIX_DEPRECATED("renamed to WaveType::GROUND") = GROUND,
		ambient_wall PHOENIX_DEPRECATED("renamed to WaveType::WALL_AMBIENT") = WALL_AMBIENT,
		wall PHOENIX_DEPRECATED("renamed to WaveType::WALL") = WALL,
		env PHOENIX_DEPRECATED("renamed to WaveType::ENVIRONMENT") = ENVIRONMENT,
		ambient_wind PHOENIX_DEPRECATED("renamed to WaveType::WIND_AMBIENT") = WIND_AMBIENT,
		wind PHOENIX_DEPRECATED("renamed to WaveType::WIND") = WIND,
	};

	/// \brief Texture animation mapping modes.
	enum class AnimationMapping {
		NONE = 0,
		LINEAR = 1,

		// Deprecated entries.
		none PHOENIX_DEPRECATED("renamed to AnimationMapping::NONE") = NONE,
		linear PHOENIX_DEPRECATED("renamed to AnimationMapping::LINEAR") = LINEAR,
	};

	/// \brief Represents a *ZenGin* material.
	///
	/// <p>Materials describe the way things look and sound in the *ZenGin*. Among other things, materials control which
	/// texture is used for a model, if the player can collide with it and even some animations. Materials are normally
	/// embedded into meshes.</p>
	class Material {
	public:
		/// \brief Parses a material from the given *ZenGin* archive.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib). Changes include the addition of the enumerations
		/// animation_mapping_mode, wave_mode_type and wave_speed_type and an updated version of alpha_function.</p>
		///
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \return The parsed material object.
		/// \throws ParserError if parsing fails.
		/// \see #parse(ArchiveReader&&) for an owning version this function.
		[[nodiscard]] PHOENIX_API static Material parse(ArchiveReader& ctx);

	public:
		std::string name;
		MaterialGroup group {MaterialGroup::UNDEFINED};
		glm::u8vec4 color {0, 0, 0, 0};
		float smooth_angle {0.0f};
		std::string texture {};
		glm::vec2 texture_scale {};
		float texture_anim_fps {0.0f};
		AnimationMapping texture_anim_map_mode {AnimationMapping::NONE};
		glm::vec2 texture_anim_map_dir {};
		bool disable_collision {false};
		bool disable_lightmap {false};
		bool dont_collapse {false};
		std::string detail_object;
		float detail_texture_scale {0.0f};
		bool force_occluder {false};
		bool environment_mapping {false};
		float environment_mapping_strength {0.0f};
		WaveType wave_mode {WaveType::NONE};
		WaveSpeed wave_speed {WaveSpeed::NONE};
		float wave_max_amplitude {0.0f};
		float wave_grid_size {0.0f};
		bool ignore_sun {false};
		AlphaFunction alpha_func {AlphaFunction::NONE};
		glm::vec2 default_mapping {};
	};

	using alpha_function PHOENIX_DEPRECATED("renamed to AlphaFunction") = AlphaFunction;
	using material_group PHOENIX_DEPRECATED("renamed to MaterialGroup") = MaterialGroup;
	using wave_speed_type PHOENIX_DEPRECATED("renamed to WaveSpeed") = WaveSpeed;
	using wave_mode_type PHOENIX_DEPRECATED("renamed to WaveType") = WaveType;
	using animation_mapping_mode PHOENIX_DEPRECATED("renamed to AnimationMapping") = AnimationMapping;
	using material PHOENIX_DEPRECATED("renamed to Material") = Material;
} // namespace phoenix
