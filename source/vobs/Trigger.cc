// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/vobs/Trigger.hh"
#include "phoenix/Animation.hh"
#include "phoenix/Archive.hh"

namespace phoenix::vobs {
	void Trigger::parse(Trigger& obj, ArchiveReader& ctx, GameVersion version) {
		VirtualObject::parse(obj, ctx, version);
		obj.target = ctx.read_string();                 // triggerTarget
		obj.flags = ctx.read_raw_bytes(1).get();        // flags
		obj.filter_flags = ctx.read_raw_bytes(1).get(); // filterFlags
		obj.vob_target = ctx.read_string();             // respondToVobName
		obj.max_activation_count = ctx.read_int();      // numCanBeActivated
		obj.retrigger_delay_sec = ctx.read_float();     // retriggerWaitSec
		obj.damage_threshold = ctx.read_float();        // damageThreshold
		obj.fire_delay_sec = ctx.read_float();          // fireDelaySec

		obj.s_count_can_be_activated = obj.max_activation_count;

		if (ctx.is_save_game()) {
			// In save-games, triggers contain extra variables
			obj.s_next_time_triggerable = ctx.read_float(); // nextTimeTriggerable
			ctx.skip_object(false);                         // [savedOtherVob % 0 0]
			obj.s_count_can_be_activated = ctx.read_int();  // countCanBeActivated

			if (version == GameVersion::GOTHIC_2) {
				obj.s_is_enabled = ctx.read_bool(); // isEnabled
			}
		}
	}

	void Mover::parse(Mover& obj, ArchiveReader& ctx, GameVersion version) {
		Trigger::parse(obj, ctx, version);
		obj.behavior = static_cast<MoverBehavior>(ctx.read_enum()); // moverBehavior
		obj.touch_blocker_damage = ctx.read_float();                // touchBlockerDamage
		obj.stay_open_time_sec = ctx.read_float();                  // stayOpenTimeSec
		obj.locked = ctx.read_bool();                               // moverLocked
		obj.auto_link = ctx.read_bool();                            // autoLinkEnabled

		if (version == GameVersion::GOTHIC_2) {
			obj.auto_rotate = ctx.read_bool(); // autoRotate
		}

		auto keyframe_count = ctx.read_word(); // numKeyframes
		if (keyframe_count > 0) {
			obj.speed = ctx.read_float();                                  // moveSpeed
			obj.lerp_mode = static_cast<MoverLerpType>(ctx.read_enum());   // posLerpType
			obj.speed_mode = static_cast<MoverSpeedType>(ctx.read_enum()); // speedType

			auto sample_reader = ctx.read_raw_bytes(keyframe_count * sizeof(float) * 7); // keyframes

			for (int32_t i = 0; i < keyframe_count; ++i) {
				auto position = sample_reader.get_vec3();

				auto x = sample_reader.get_float();
				auto y = sample_reader.get_float();
				auto z = sample_reader.get_float();
				auto w = sample_reader.get_float();

				obj.keyframes.push_back(AnimationSample {position, glm::quat {w, x, y, z}});
			}
		}

		if (ctx.is_save_game()) {
			// In save-games, movers contain extra variables
			obj.s_act_key_pos_delta = ctx.read_vec3();    // actKeyPosDelta
			obj.s_act_keyframe_f = ctx.read_float();      // actKeyframeF
			obj.s_act_keyframe = ctx.read_int();          // actKeyframe
			obj.s_next_keyframe = ctx.read_int();         // nextKeyframe
			obj.s_move_speed_unit = ctx.read_float();     // moveSpeedUnit
			obj.s_advance_dir = ctx.read_float();         // advanceDir
			obj.s_mover_state = ctx.read_enum();          // moverState
			obj.s_trigger_event_count = ctx.read_int();   // numTriggerEvents
			obj.s_stay_open_time_dest = ctx.read_float(); // stayOpenTimeDest
		}

		obj.sfx_open_start = ctx.read_string();    // sfxOpenStart
		obj.sfx_open_end = ctx.read_string();      // sfxOpenEnd
		obj.sfx_transitioning = ctx.read_string(); // sfxMoving
		obj.sfx_close_start = ctx.read_string();   // sfxCloseStart
		obj.sfx_close_end = ctx.read_string();     // sfxCloseEnd
		obj.sfx_lock = ctx.read_string();          // sfxLock
		obj.sfx_unlock = ctx.read_string();        // sfxUnlock
		obj.sfx_use_locked = ctx.read_string();    // sfxUseLocked
	}

	void TriggerList::parse(TriggerList& obj, ArchiveReader& ctx, GameVersion version) {
		Trigger::parse(obj, ctx, version);
		obj.mode = static_cast<TriggerBatchMode>(ctx.read_enum()); // listProcess

		auto target_count = ctx.read_byte(); // numTarget
		for (int32_t i = 0; i < target_count; ++i) {
			obj.targets.emplace_back(target {
			    ctx.read_string(), // triggerTarget[i]
			    ctx.read_float()   // fireDelay[i]
			});
		}

		if (ctx.is_save_game()) {
			// In save-games, trigger lists contain extra variables
			obj.s_act_target = ctx.read_byte();      // actTarget
			obj.s_send_on_trigger = ctx.read_bool(); // sendOnTrigger
		}
	}

	void TriggerScript::parse(TriggerScript& obj, ArchiveReader& ctx, GameVersion version) {
		Trigger::parse(obj, ctx, version);
		obj.function = ctx.read_string(); // scriptFunc
	}

	void TriggerChangeLevel::parse(TriggerChangeLevel& obj, ArchiveReader& ctx, GameVersion version) {
		Trigger::parse(obj, ctx, version);
		obj.level_name = ctx.read_string(); // levelName
		obj.start_vob = ctx.read_string();  // startVobName
	}

	void TriggerWorldStart::parse(TriggerWorldStart& obj, ArchiveReader& ctx, GameVersion version) {
		VirtualObject::parse(obj, ctx, version);
		obj.target = ctx.read_string();  // triggerTarget
		obj.fire_once = ctx.read_bool(); // fireOnlyFirstTime

		if (ctx.is_save_game() && version == GameVersion::GOTHIC_2) {
			// In Gothic 2 save-games, world start triggers contain extra variables
			obj.s_has_fired = ctx.read_bool(); // hasFired
		}
	}

	void TriggerUntouch::parse(TriggerUntouch& obj, ArchiveReader& ctx, GameVersion version) {
		VirtualObject::parse(obj, ctx, version);
		obj.target = ctx.read_string(); // triggerTarget
	}
} // namespace phoenix::vobs
