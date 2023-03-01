// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include "vob.hh"

namespace phoenix {
	enum class MessageFilterAction : uint32_t {
		NONE = 0,
		TRIGGER = 1,
		UNTRIGGER = 2,
		ENABLE = 3,
		DISABLE = 4,
		TOGGLE = 5,

		// Deprecated entries.
		none PHOENIX_DEPRECATED("renamed to MessageFilterAction::NONE") = NONE,
		trigger PHOENIX_DEPRECATED("renamed to MessageFilterAction::TRIGGER") = TRIGGER,
		untrigger PHOENIX_DEPRECATED("renamed to MessageFilterAction::UNTRIGGER") = UNTRIGGER,
		enable PHOENIX_DEPRECATED("renamed to MessageFilterAction::ENABLE") = ENABLE,
		disable PHOENIX_DEPRECATED("renamed to MessageFilterAction::DISABLE") = DISABLE,
		toggle PHOENIX_DEPRECATED("renamed to MessageFilterAction::TOGGLE") = TOGGLE,
	};

	enum class MoverMessageType : uint32_t {
		FIXED_DIRECT = 0,
		FIXED_ORDER = 1,
		NEXT = 2,
		PREVIOUS = 3,

		// Deprecated entries.
		fixed_direct PHOENIX_DEPRECATED("renamed to MoverMessageType::FIXED_DIRECT") = FIXED_DIRECT,
		fixed_order PHOENIX_DEPRECATED("renamed to MoverMessageType::FIXED_ORDER") = FIXED_ORDER,
		next PHOENIX_DEPRECATED("renamed to MoverMessageType::NEXT") = NEXT,
		previous PHOENIX_DEPRECATED("renamed to MoverMessageType::PREVIOUS") = PREVIOUS,
	};

	enum class TouchCollisionType : std::uint32_t {
		NONE = 0,
		BOX = 1,
		POINT = 2,

		// Deprecated entries.
		none PHOENIX_DEPRECATED("renamed to TouchCollisionType::NONE") = NONE,
		box PHOENIX_DEPRECATED("renamed to TouchCollisionType::BOX") = BOX,
		point PHOENIX_DEPRECATED("renamed to TouchCollisionType::POINT") = POINT,
	};

	using message_filter_action PHOENIX_DEPRECATED("renamed to MessageFilterAction") = MessageFilterAction;
	using mover_message_type PHOENIX_DEPRECATED("renamed to MoverMessageType") = MoverMessageType;
	using collision_type PHOENIX_DEPRECATED("renamed to TouchCollisionType") = TouchCollisionType;

	namespace vobs {
		/// \brief An animated VOb.
		struct Animate : public VirtualObject {
			bool start_on {false};

			// Save-game only variables
			bool s_is_running {false};

			/// \brief Parses an animated VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(Animate& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb representing an in-game item.
		struct Item : public VirtualObject {
			std::string instance;

			// Save-game only variables
			int s_amount;
			int s_flags;

			/// \brief Parses an item VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(Item& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb representing a [lens flare](https://en.wikipedia.org/wiki/Lens_flare).
		struct LensFlare : public VirtualObject {
			std::string fx;

			/// \brief Parses a lens flare VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(LensFlare& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb representing a particle system controller.
		struct ParticleEffectController : public VirtualObject {
			std::string pfx_name;
			bool kill_when_done;
			bool initially_running;

			/// \brief Parses a particle system controller VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(ParticleEffectController& obj, ArchiveReader& ctx, GameVersion version);
		};

		struct MessageFilter : public VirtualObject {
			std::string target;
			MessageFilterAction on_trigger;
			MessageFilterAction on_untrigger;

			/// \brief Parses a message filter VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(MessageFilter& obj, ArchiveReader& ctx, GameVersion version);
		};

		struct CodeMaster : public VirtualObject {
			std::string target;
			bool ordered;
			bool first_false_is_failure;
			std::string failure_target;
			bool untriggered_cancels;
			std::vector<std::string> slaves;

			// Save-game only variables
			uint8_t s_num_triggered_slaves;

			/// \brief Parses a code master VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(CodeMaster& obj, ArchiveReader& ctx, GameVersion version);
		};

		struct MoverController : public VirtualObject {
			std::string target;
			MoverMessageType message;
			std::int32_t key;

			/// \brief Parses a mover controller VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(MoverController& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb which represents a damage source.
		struct TouchDamage : public VirtualObject {
			float damage;

			bool barrier;
			bool blunt;
			bool edge;
			bool fire;
			bool fly;
			bool magic;
			bool point;
			bool fall;

			float repeat_delay_sec;
			float volume_scale;
			TouchCollisionType collision;

			/// \brief Parses a touch damage VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(TouchDamage& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb which represents an earthquake-like effect.
		struct Earthquake : public VirtualObject {
			float radius;
			float duration;
			glm::vec3 amplitude;

			/// \brief Parses an earthquake VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(Earthquake& obj, ArchiveReader& ctx, GameVersion version);
		};

		struct Npc : public VirtualObject {
			struct talent {
				int talent;
				int value;
				int skill;
			};

			struct slot {
				bool used;
				std::string name;
				int item_index;
				bool in_inventory;
			};

			std::string npc_instance;
			glm::vec3 model_scale;
			float model_fatness;

			std::vector<std::string> overlays;

			int flags;
			int guild;
			int guild_true;
			int level;
			int xp;
			int xp_next_level;
			int lp;

			std::vector<talent> talents;

			int fight_tactic;
			int fight_mode;
			bool wounded;
			bool mad;
			int mad_time;
			bool player;

			int attributes[8];
			int hcs[4];
			int missions[5];

			std::string start_ai_state;
			int aivar[100];
			std::string script_waypoint;
			int attitude;
			int attitude_temp;
			int name_nr;
			bool move_lock;

			std::string packed[9];
			std::vector<std::unique_ptr<Item>> items;
			std::vector<slot> slots;

			bool current_state_valid;
			std::string current_state_name;
			int current_state_index;
			bool current_state_is_routine;

			bool next_state_valid;
			std::string next_state_name;
			int next_state_index;
			bool next_state_is_routine;

			int last_ai_state;
			bool has_routine;
			bool routine_changed;
			bool routine_overlay;
			int routine_overlay_count;
			int walkmode_routine;
			bool weaponmode_routine;
			bool start_new_routine;
			int ai_state_driven;
			glm::vec3 ai_state_pos;
			std::string current_routine;
			bool respawn;
			int respawn_time;

			int protection[8];

			int bs_interruptable_override {0};
			int npc_type {0};
			int spell_mana {0};

			/// \brief Parses an NPC VOb from the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(Npc& obj, ArchiveReader& ctx, GameVersion version);
		};

		using animate PHOENIX_DEPRECATED("renamed to Animate") = Animate;
		using item PHOENIX_DEPRECATED("renamed to Item") = Item;
		using lens_flare PHOENIX_DEPRECATED("renamed to LensFlare") = LensFlare;
		using pfx_controller PHOENIX_DEPRECATED("renamed to ParticleEffectController") = ParticleEffectController;
		using code_master PHOENIX_DEPRECATED("renamed to CodeMaster") = CodeMaster;
		using message_filter PHOENIX_DEPRECATED("renamed to MessageFilter") = MessageFilter;
		using mover_controller PHOENIX_DEPRECATED("renamed to MoverController") = MoverController;
		using touch_damage PHOENIX_DEPRECATED("renamed to TouchDamage") = TouchDamage;
		using earthquake PHOENIX_DEPRECATED("renamed to Earthquake") = Earthquake;
		using npc PHOENIX_DEPRECATED("renamed to Npc") = Npc;
	} // namespace vobs
} // namespace phoenix