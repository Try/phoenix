// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include "VirtualObject.hh"

namespace phoenix {
	enum class CameraMotion : std::uint32_t {
		UNDEFINED = 0,
		SMOOTH = 1,
		LINEAR = 2,
		STEP = 3,
		SLOW = 4,
		FAST = 5,
		CUSTOM = 6,

		// Deprecated entries.
		undefined PHOENIX_DEPRECATED("renamed to CameraMotion::UNDEFINED") = UNDEFINED,
		smooth PHOENIX_DEPRECATED("renamed to CameraMotion::SMOOTH") = SMOOTH,
		linear PHOENIX_DEPRECATED("renamed to CameraMotion::LINEAR") = LINEAR,
		step PHOENIX_DEPRECATED("renamed to CameraMotion::STEP") = STEP,
		slow PHOENIX_DEPRECATED("renamed to CameraMotion::SLOW") = SLOW,
		fast PHOENIX_DEPRECATED("renamed to CameraMotion::FAST") = FAST,
		custom PHOENIX_DEPRECATED("renamed to CameraMotion::CUSTOM") = CUSTOM,
	};

	enum class CameraTrajectory : std::uint32_t {
		WORLD = 0,
		OBJECT = 1,

		// Deprecated entries.
		world PHOENIX_DEPRECATED("renamed to CameraTrajectory::WORLD") = WORLD,
		object PHOENIX_DEPRECATED("renamed to CameraTrajectory::OBJECT") = OBJECT,
	};

	enum class CameraLerpType : std::uint32_t {
		UNDEFINED = 0,
		PATH = 1,
		PATH_IGNORE_ROLL = 2,
		PATH_ROTATION_SAMPLES = 3,

		// Deprecated entries.
		undefined PHOENIX_DEPRECATED("renamed to CameraLerpType::UNDEFINED") = UNDEFINED,
		path PHOENIX_DEPRECATED("renamed to CameraLerpType::PATH") = PATH,
		path_ignore_roll PHOENIX_DEPRECATED("renamed to CameraLerpType::PATH_IGNORE_ROLL") = PATH_IGNORE_ROLL,
		path_rotation_samples PHOENIX_DEPRECATED("renamed to CameraLerpType::PATH_ROTATION_SAMPLES") =
		    PATH_ROTATION_SAMPLES,
	};

	enum class CameraLoop : std::uint32_t {
		NONE = 0,
		RESTART = 1,
		PINGPONG = 2,

		// Deprecated entries.
		none PHOENIX_DEPRECATED("renamed to CameraLoop::NONE") = NONE,
		restart PHOENIX_DEPRECATED("renamed to CameraLoop::RESTART") = RESTART,
		pingpong PHOENIX_DEPRECATED("renamed to CameraLoop::PINGPONG") = PINGPONG,
	};

	using camera_motion PHOENIX_DEPRECATED("renamed to CameraMotion") = CameraMotion;
	using camera_trajectory PHOENIX_DEPRECATED("renamed to CameraTrajectory") = CameraTrajectory;
	using camera_lerp_mode PHOENIX_DEPRECATED("renamed to CameraLerpType") = CameraLerpType;
	using camera_loop PHOENIX_DEPRECATED("renamed to CameraLoop") = CameraLoop;

	namespace vobs {
		/// \brief A VOb which describes the trajectory of a camera during a cutscene.
		struct CameraTrajectoryFrame : public VirtualObject {
			float time;
			float roll_angle;
			float fov_scale;
			CameraMotion motion_type;
			CameraMotion motion_type_fov;
			CameraMotion motion_type_roll;
			CameraMotion motion_type_time_scale;
			float tension;
			float cam_bias;
			float continuity;
			float time_scale;
			bool time_fixed;
			glm::mat4x4 original_pose;

			/// \brief Parses a camera trajectory VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static std::unique_ptr<CameraTrajectoryFrame> parse(ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb which defined the movement of the camera during a cutscene.
		struct CutsceneCamera : public VirtualObject {
			CameraTrajectory trajectory_for;
			CameraTrajectory target_trajectory_for;
			CameraLoop loop_mode;
			CameraLerpType lerp_mode;
			bool ignore_for_vob_rotation;
			bool ignore_for_vob_rotation_target;
			bool adapt;
			bool ease_first;
			bool ease_last;
			float total_duration;
			std::string auto_focus_vob;
			bool auto_player_movable;
			bool auto_untrigger_last;
			float auto_untrigger_last_delay;
			std::int32_t position_count;
			std::int32_t target_count;

			std::vector<std::unique_ptr<CameraTrajectoryFrame>> frames;

			// Save-game only variables
			bool s_paused {false};
			bool s_started {false};
			bool s_goto_time_mode {false};
			float s_cs_time {0};

			/// \brief Parses a cutscene camera VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(CutsceneCamera& obj, ArchiveReader& ctx, GameVersion version);
		};

		using camera_trj_frame PHOENIX_DEPRECATED("renamed to CameraTrajectoryFrame") = CameraTrajectoryFrame;
		using cs_camera PHOENIX_DEPRECATED("renamed to CutsceneCamera") = CutsceneCamera;
	} // namespace vobs
} // namespace phoenix
