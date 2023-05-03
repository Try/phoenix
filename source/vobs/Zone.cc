// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/vobs/Zone.hh"
#include "phoenix/Archive.hh"

namespace phoenix::vobs {
	void ZoneMusic::parse(ZoneMusic& obj, ArchiveReader& ctx, GameVersion version) {
		VirtualObject::parse(obj, ctx, version);
		obj.enabled = ctx.read_bool();   // enabled
		obj.priority = ctx.read_int();   // priority
		obj.ellipsoid = ctx.read_bool(); // ellipsoid
		obj.reverb = ctx.read_float();   // reverbLevel
		obj.volume = ctx.read_float();   // volumeLevel
		obj.loop = ctx.read_bool();      // loop

		if (ctx.is_save_game()) {
			// In save-games, zones contain extra variables
			obj.s_local_enabled = ctx.read_bool();       // local_enabled
			obj.s_day_entrance_done = ctx.read_bool();   // dayEntranceDone
			obj.s_night_entrance_done = ctx.read_bool(); // nightEntranceDone
		}
	}

	void ZoneFarPlane::parse(ZoneFarPlane& obj, ArchiveReader& ctx, GameVersion version) {
		VirtualObject::parse(obj, ctx, version);
		obj.vob_far_plane_z = ctx.read_float();        // vobFarPlaneZ
		obj.inner_range_percentage = ctx.read_float(); // innerRangePerc
	}

	void ZoneFog::parse(ZoneFog& obj, ArchiveReader& ctx, GameVersion version) {
		VirtualObject::parse(obj, ctx, version);
		obj.range_center = ctx.read_float();           // fogRangeCenter
		obj.inner_range_percentage = ctx.read_float(); // innerRangePerc
		obj.color = ctx.read_color();                  // fogColor

		if (version == GameVersion::GOTHIC_2) {
			obj.fade_out_sky = ctx.read_bool();   // fadeOutSky
			obj.override_color = ctx.read_bool(); // overrideColor
		}
	}
} // namespace phoenix::vobs
