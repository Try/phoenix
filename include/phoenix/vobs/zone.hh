// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include "vob.hh"

namespace phoenix::vobs {
	/// \brief A VOb which defines the background music in a certain zone.
	struct ZoneMusic : public VirtualObject {
		bool enabled {false};
		std::int32_t priority {0};
		bool ellipsoid {false};
		float reverb {0};
		float volume {0};
		bool loop {false};

		// Save-game only variables
		bool s_local_enabled {true};
		bool s_day_entrance_done {false};
		bool s_night_entrance_done {false};

		/// \brief Parses a zone music VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		PHOENIX_API static void parse(ZoneMusic& obj, ArchiveReader& ctx, GameVersion version);
	};

	/// \brief A VOb which defines the far plane settings in a certain zone.
	struct ZoneFarPlane : public VirtualObject {
		float vob_far_plane_z;
		float inner_range_percentage;

		/// \brief Parses a zone far plane VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		PHOENIX_API static void parse(ZoneFarPlane& obj, ArchiveReader& ctx, GameVersion version);
	};

	/// \brief A VOb which defines the fog in a certain zone.
	struct ZoneFog : public VirtualObject {
		float range_center {0};
		float inner_range_percentage {0};
		glm::u8vec4 color {};
		bool fade_out_sky {false};
		bool override_color {false};

		/// \brief Parses a zone fog VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		PHOENIX_API static void parse(ZoneFog& obj, ArchiveReader& ctx, GameVersion version);
	};

	using zone_music PHOENIX_DEPRECATED("renamed to ZoneMusic") = ZoneMusic;
	using zone_far_plane PHOENIX_DEPRECATED("renamed to ZoneFarPlane") = ZoneFarPlane;
	using zone_fog PHOENIX_DEPRECATED("renamed to ZoneFog") = ZoneFog;
} // namespace phoenix::vobs