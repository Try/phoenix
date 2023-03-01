// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/vobs/sound.hh"
#include "phoenix/archive.hh"

namespace phoenix::vobs {
	void Sound::parse(Sound& obj, ArchiveReader& ctx, GameVersion version) {
		VirtualObject::parse(obj, ctx, version);
		obj.volume = ctx.read_float();                                          // sndVolume
		obj.mode = static_cast<SoundMode>(ctx.read_enum());                     // sndMode
		obj.random_delay = ctx.read_float();                                    // sndRandDelay
		obj.random_delay_var = ctx.read_float();                                // sndRandDelayVar
		obj.initially_playing = ctx.read_bool();                                // sndStartOn
		obj.ambient3d = ctx.read_bool();                                        // sndAmbient3D
		obj.obstruction = ctx.read_bool();                                      // sndObstruction
		obj.cone_angle = ctx.read_float();                                      // sndConeAngle
		obj.volume_type = static_cast<SoundTriggerVolumeType>(ctx.read_enum()); // sndVolType
		obj.radius = ctx.read_float();                                          // sndRadius
		obj.sound_name = ctx.read_string();                                     // sndName

		if (ctx.is_save_game()) {
			// In save-games, sounds contain extra variables
			obj.s_is_running = ctx.read_bool();        // soundIsRunning
			obj.s_is_allowed_to_run = ctx.read_bool(); // soundAllowedToRun
		}
	}

	void SoundDaytime::parse(SoundDaytime& obj, ArchiveReader& ctx, GameVersion version) {
		Sound::parse(obj, ctx, version);
		obj.start_time = ctx.read_float();   // sndStartTime
		obj.end_time = ctx.read_float();     // sndEndTime
		obj.sound_name2 = ctx.read_string(); // sndName2
	}
} // namespace phoenix::vobs
