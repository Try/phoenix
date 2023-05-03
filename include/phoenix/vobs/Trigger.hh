// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Animation.hh"
#include "../Api.hh"
#include "VirtualObject.hh"

namespace phoenix {
	enum class MoverBehavior : uint32_t {
		TOGGLE = 0,
		TRIGGER_CONTROL = 1,
		OPEN_TIME = 2,
		LOOP = 3,
		SINGLE_KEYS = 4,

		// Deprecated entries.
		toggle PHOENIX_DEPRECATED("renamed to MoverBehavior::TOGGLE") = TOGGLE,
		trigger_control PHOENIX_DEPRECATED("renamed to MoverBehavior::TRIGGER_CONTROL") = TRIGGER_CONTROL,
		open_timed PHOENIX_DEPRECATED("renamed to MoverBehavior::OPEN_TIME") = OPEN_TIME,
		loop PHOENIX_DEPRECATED("renamed to MoverBehavior::LOOP") = LOOP,
		single_keys PHOENIX_DEPRECATED("renamed to MoverBehavior::SINGLE_KEYS") = SINGLE_KEYS,
	};

	enum class MoverLerpType : uint32_t {
		CURVE = 0,
		LINEAR = 1,

		// Deprecated entries.
		curve PHOENIX_DEPRECATED("renamed to MoverLerpType::CURVE") = CURVE,
		linear PHOENIX_DEPRECATED("renamed to MoverLerpType::LINEAR") = LINEAR,
	};

	enum class MoverSpeedType : uint32_t {
		CONSTANT = 0,
		SLOW_START_END = 1,
		SLOW_START = 2,
		SLOW_END = 3,
		SEGMENT_SLOW_START_END = 4,
		SEGMENT_SLOW_START = 5,
		SEGMENT_SLOW_END = 6,

		// Deprecated entries.
		seg_constant PHOENIX_DEPRECATED("renamed to MoverSpeedType::CONSTANT") = CONSTANT,
		slow_start_end PHOENIX_DEPRECATED("renamed to MoverSpeedType::SLOW_START_END") = SLOW_START_END,
		slow_start PHOENIX_DEPRECATED("renamed to MoverSpeedType::SLOW_START") = SLOW_START,
		slow_end PHOENIX_DEPRECATED("renamed to MoverSpeedType::SLOW_END") = SLOW_END,
		seg_slow_start_end PHOENIX_DEPRECATED("renamed to MoverSpeedType::SEGMENT_SLOW_START_END") =
		    SEGMENT_SLOW_START_END,
		seg_slow_start PHOENIX_DEPRECATED("renamed to MoverSpeedType::SEGMENT_SLOW_START") = SEGMENT_SLOW_START,
		seg_slow_end PHOENIX_DEPRECATED("renamed to MoverSpeedType::SEGMENT_SLOW_END") = SEGMENT_SLOW_END,
	};

	enum class TriggerBatchMode {
		ALL = 0,
		NEXT = 1,
		RANDOM = 2,

		// Deprecated entries.
		all PHOENIX_DEPRECATED("renamed to TriggerBatchMode::ALL") = ALL,
		next PHOENIX_DEPRECATED("renamed to TriggerBatchMode::NEXT") = NEXT,
		random PHOENIX_DEPRECATED("renamed to TriggerBatchMode::RANDOM") = RANDOM,
	};

	namespace vobs {
		/// \brief A basic trigger VOb which does something upon the player interacting with it.
		struct Trigger : public VirtualObject {
			std::string target;
			std::uint8_t flags;
			std::uint8_t filter_flags;
			std::string vob_target;
			std::int32_t max_activation_count;
			float retrigger_delay_sec;
			float damage_threshold;
			float fire_delay_sec;

			// Save-game only variables
			float s_next_time_triggerable {0};
			int s_count_can_be_activated {0};
			bool s_is_enabled {true};

			/// \brief Parses a trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(Trigger& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb which can move upon player interaction.
		struct Mover : public Trigger {
			MoverBehavior behavior {MoverBehavior::TOGGLE};
			float touch_blocker_damage {0};
			float stay_open_time_sec {0};
			bool locked {true};
			bool auto_link {false};
			bool auto_rotate {false};

			float speed {0};
			MoverLerpType lerp_mode {MoverLerpType::CURVE};
			MoverSpeedType speed_mode {MoverSpeedType::CONSTANT};

			std::vector<AnimationSample> keyframes {};

			std::string sfx_open_start {};
			std::string sfx_open_end {};
			std::string sfx_transitioning {};
			std::string sfx_close_start {};
			std::string sfx_close_end {};
			std::string sfx_lock {};
			std::string sfx_unlock {};
			std::string sfx_use_locked {};

			// Save-game only variables
			glm::vec3 s_act_key_pos_delta;
			float s_act_keyframe_f;
			int s_act_keyframe;
			int s_next_keyframe;
			float s_move_speed_unit;
			float s_advance_dir;
			uint32_t s_mover_state;
			int s_trigger_event_count;
			float s_stay_open_time_dest;

			/// \brief Parses a mover trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(Mover& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb which can call multiple script function upon being triggered.
		struct TriggerList : public Trigger {
			struct target {
				std::string name {};
				float delay {};

				[[nodiscard]] inline bool operator==(const target& tgt) const noexcept {
					return this->name == tgt.name && this->delay == tgt.delay;
				}
			};

			TriggerBatchMode mode {};
			std::vector<target> targets {};

			// Save-game only variables
			uint8_t s_act_target {0};
			bool s_send_on_trigger {false};

			/// \brief Parses a trigger list VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(TriggerList& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb which calls a script function upon being triggered.
		struct TriggerScript : public Trigger {
			std::string function {};

			/// \brief Parses a script trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(TriggerScript& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb which triggers a level change if the player moves close to it.
		struct TriggerChangeLevel : public Trigger {
			std::string level_name {};
			std::string start_vob {};

			/// \brief Parses a change level trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see trigger::parse
			PHOENIX_API static void parse(TriggerChangeLevel& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb which triggers a world start event.
		struct TriggerWorldStart : public VirtualObject {
			std::string target;
			bool fire_once;

			// Save-game only variables
			bool s_has_fired {false};

			/// \brief Parses a world load trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(TriggerWorldStart& obj, ArchiveReader& ctx, GameVersion version);
		};

		struct TriggerUntouch : public VirtualObject {
			std::string target;

			/// \brief Parses an untouch trigger VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(TriggerUntouch& obj, ArchiveReader& ctx, GameVersion version);
		};

		using trigger PHOENIX_DEPRECATED("renamed to Trigger") = Trigger;
		using trigger_mover PHOENIX_DEPRECATED("renamed to Mover") = Mover;
		using trigger_list PHOENIX_DEPRECATED("renamed to TriggerList") = TriggerList;
		using trigger_script PHOENIX_DEPRECATED("renamed to TriggerScript") = TriggerScript;
		using trigger_change_level PHOENIX_DEPRECATED("renamed to TriggerChangeLevel") = TriggerChangeLevel;
		using trigger_world_start PHOENIX_DEPRECATED("renamed to TriggerWorldStart") = TriggerWorldStart;
		using trigger_untouch PHOENIX_DEPRECATED("renamed to TriggerUntouch") = TriggerUntouch;
	} // namespace vobs

	using mover_behavior PHOENIX_DEPRECATED("renamed to MoverBehavior") = MoverBehavior;
	using mover_lerp_mode PHOENIX_DEPRECATED("renamed to MoverLerpType") = MoverLerpType;
	using mover_speed_mode PHOENIX_DEPRECATED("renamed to MoverSpeedType") = MoverSpeedType;
	using trigger_batch_mode PHOENIX_DEPRECATED("renamed to TriggerBatchMode") = TriggerBatchMode;
} // namespace phoenix
