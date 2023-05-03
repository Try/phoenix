// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/vobs/Camera.hh"
#include "phoenix/Archive.hh"

namespace phoenix::vobs {
	std::unique_ptr<CameraTrajectoryFrame> CameraTrajectoryFrame::parse(ArchiveReader& ctx, GameVersion version) {
		auto obj = std::make_unique<CameraTrajectoryFrame>();

		VirtualObject::parse(*obj, ctx, version);
		obj->time = ctx.read_float();                                             // time
		obj->roll_angle = ctx.read_float();                                       // angleRollDeg
		obj->fov_scale = ctx.read_float();                                        // camFOVScale
		obj->motion_type = static_cast<CameraMotion>(ctx.read_enum());            // motionType
		obj->motion_type_fov = static_cast<CameraMotion>(ctx.read_enum());        // motionTypeFOV
		obj->motion_type_roll = static_cast<CameraMotion>(ctx.read_enum());       // motionTypeRoll
		obj->motion_type_time_scale = static_cast<CameraMotion>(ctx.read_enum()); // motionTypeTimeScale
		obj->tension = ctx.read_float();                                          // tension
		obj->cam_bias = ctx.read_float();                                         // bias
		obj->continuity = ctx.read_float();                                       // continuity
		obj->time_scale = ctx.read_float();                                       // timeScale
		obj->time_fixed = ctx.read_bool();                                        // timeIsFixed

		auto buf = ctx.read_raw_bytes(sizeof(float) * 4 * 4); // originalPose
		obj->original_pose = buf.get_mat4x4();
		return obj;
	}

	void CutsceneCamera::parse(CutsceneCamera& obj, ArchiveReader& ctx, GameVersion version) {
		VirtualObject::parse(obj, ctx, version);
		obj.trajectory_for = static_cast<CameraTrajectory>(ctx.read_enum());        // camTrjFOR
		obj.target_trajectory_for = static_cast<CameraTrajectory>(ctx.read_enum()); // targetTrjFOR
		obj.loop_mode = static_cast<CameraLoop>(ctx.read_enum());                   // loopMode
		obj.lerp_mode = static_cast<CameraLerpType>(ctx.read_enum());               // splLerpMode
		obj.ignore_for_vob_rotation = ctx.read_bool();                              // ignoreFORVobRotCam
		obj.ignore_for_vob_rotation_target = ctx.read_bool();                       // ignoreFORVobRotTarget
		obj.adapt = ctx.read_bool();                                                // adaptToSurroundings
		obj.ease_first = ctx.read_bool();                                           // easeToFirstKey
		obj.ease_last = ctx.read_bool();                                            // easeFromLastKey
		obj.total_duration = ctx.read_float();                                      // totalTime
		obj.auto_focus_vob = ctx.read_string();                                     // autoCamFocusVobName
		obj.auto_player_movable = ctx.read_bool();                                  // autoCamPlayerMovable
		obj.auto_untrigger_last = ctx.read_bool();                                  // autoCamUntriggerOnLastKey
		obj.auto_untrigger_last_delay = ctx.read_float();                           // autoCamUntriggerOnLastKeyDelay
		obj.position_count = ctx.read_int();                                        // numPos
		obj.target_count = ctx.read_int();                                          // numTargets

		ArchiveObject frame_obj {};
		while (ctx.read_object_begin(frame_obj)) {
			if (frame_obj.class_name != "zCCamTrj_KeyFrame:zCVob") {
				PX_LOGW("CutsceneCamera: unexpected \"", frame_obj.class_name, "\" in \"zCCSCamera:zCVob\"");
				ctx.skip_object(true);
				continue;
			}

			obj.frames.emplace_back(CameraTrajectoryFrame::parse(ctx, version));

			if (!ctx.read_object_end()) {
				PX_LOGW("CutsceneCamera: \"zCCamTrj_KeyFrame\" not fully parsed");
				ctx.skip_object(true);
			}
		}

		if (ctx.is_save_game() && version == GameVersion::GOTHIC_2) {
			// In save-games, cutscene cameras contain extra variables
			obj.s_paused = ctx.read_bool();         // paused
			obj.s_started = ctx.read_bool();        // started
			obj.s_goto_time_mode = ctx.read_bool(); // gotoTimeMode
			obj.s_cs_time = ctx.read_float();       // csTime
		}
	}
} // namespace phoenix::vobs
