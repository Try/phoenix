// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "phoenix.hh"
#include "phoenix/material.hh"

#include <string>
#include <vector>

namespace phoenix {
	class Buffer;

	struct ScriptSyntaxError : public ParserError {
	public:
		ScriptSyntaxError(std::string&& location, std::string&& msg);
	};

	enum class MdsEventType : uint8_t {
		UNKNOWN = 0,
		ITEM_CREATE = 1,
		ITEM_INSERT = 2,
		ITEM_REMOVE = 3,
		ITEM_DESTROY = 4,
		ITEM_PLACE = 5,
		ITEM_EXCHANGE = 6,
		SET_FIGHT_MODE = 7,
		MUNITION_PLACE = 8,
		MUNITION_REMOVE = 9,
		SOUND_DRAW = 10,
		SOUND_UNDRAW = 11,
		MESH_SWAP = 12,
		TORCH_DRAW = 13,
		TORCH_INVENTORY = 14,
		TORCH_DROP = 15,
		HIT_LIMB = 16,
		HIT_DIRECTION = 17,
		DAMAGE_MULTIPLIER = 18,
		PARRY_FRAME = 19,
		OPTIMAL_FRAME = 20,
		HIT_END = 21,
		COMBO_WINDOW = 22,

		// Deprecated entries.
		unknown PHOENIX_DEPRECATED("renamed to MdsEventType::UNKNOWN") = UNKNOWN,
		create_item PHOENIX_DEPRECATED("renamed to MdsEventType::ITEM_CREATE") = ITEM_CREATE,
		insert_item PHOENIX_DEPRECATED("renamed to MdsEventType::ITEM_INSERT") = ITEM_INSERT,
		remove_item PHOENIX_DEPRECATED("renamed to MdsEventType::ITEM_REMOVE") = ITEM_REMOVE,
		destroy_item PHOENIX_DEPRECATED("renamed to MdsEventType::ITEM_DESTROY") = ITEM_DESTROY,
		place_item PHOENIX_DEPRECATED("renamed to MdsEventType::ITEM_PLACE") = ITEM_PLACE,
		exchange_item PHOENIX_DEPRECATED("renamed to MdsEventType::ITEM_EXCHANGE") = ITEM_EXCHANGE,
		fight_mode PHOENIX_DEPRECATED("renamed to MdsEventType::SET_FIGHT_MODE") = SET_FIGHT_MODE,
		place_munition PHOENIX_DEPRECATED("renamed to MdsEventType::MUNITION_PLACE") = MUNITION_PLACE,
		remove_munition PHOENIX_DEPRECATED("renamed to MdsEventType::MUNITION_REMOVE") = MUNITION_REMOVE,
		draw_sound PHOENIX_DEPRECATED("renamed to MdsEventType::SOUND_DRAW") = SOUND_DRAW,
		undraw_sound PHOENIX_DEPRECATED("renamed to MdsEventType::SOUND_UNDRAW") = SOUND_UNDRAW,
		swap_mesh PHOENIX_DEPRECATED("renamed to MdsEventType::MESH_SWAP") = MESH_SWAP,
		draw_torch PHOENIX_DEPRECATED("renamed to MdsEventType::TORCH_DRAW") = TORCH_DRAW,
		inventory_torch PHOENIX_DEPRECATED("renamed to MdsEventType::TORCH_INVENTORY") = TORCH_INVENTORY,
		drop_torch PHOENIX_DEPRECATED("renamed to MdsEventType::TORCH_DROP") = TORCH_DROP,
		hit_limb PHOENIX_DEPRECATED("renamed to MdsEventType::HIT_LIMB") = HIT_LIMB,
		hit_direction PHOENIX_DEPRECATED("renamed to MdsEventType::HIT_DIRECTION") = HIT_DIRECTION,
		dam_multiply PHOENIX_DEPRECATED("renamed to MdsEventType::DAMAGE_MULTIPLIER") = DAMAGE_MULTIPLIER,
		par_frame PHOENIX_DEPRECATED("renamed to MdsEventType::PARRY_FRAME") = PARRY_FRAME,
		opt_frame PHOENIX_DEPRECATED("renamed to MdsEventType::OPTIMAL_FRAME") = OPTIMAL_FRAME,
		hit_end PHOENIX_DEPRECATED("renamed to MdsEventType::HIT_END") = HIT_END,
		window PHOENIX_DEPRECATED("renamed to MdsEventType::COMBO_WINDOW") = COMBO_WINDOW,
	};

	/// \brief A set of fight stances the player can take.
	enum class MdsFightMode : uint8_t {
		FIST = 0,          ///< The player fights with his fists.
		SINGLE_HANDED = 1, ///< The player wields a one-handed weapon.
		DUAL_HANDED = 2,   ///< The player wields a two-handed weapon.
		BOW = 3,           ///< The player wields a bow.
		CROSSBOW = 4,      ///< The player wields a crossbow.
		MAGIC = 5,         ///< The player casts a magic spell.
		NONE = 6,          ///< The player is not in a fighting stance.
		INVALID = 0xFF,    ///< A fight mode which acts as an `unset` marker. Added for OpenGothic compatibility.

		// Deprecated entries.
		fist PHOENIX_DEPRECATED("renamed to MdsFightMode::FIST") = FIST,
		one_handed PHOENIX_DEPRECATED("renamed to MdsFightMode::SINGLE_HANDED") = SINGLE_HANDED,
		two_handed PHOENIX_DEPRECATED("renamed to MdsFightMode::DUAL_HANDED") = DUAL_HANDED,
		bow PHOENIX_DEPRECATED("renamed to MdsFightMode::BOW") = BOW,
		crossbow PHOENIX_DEPRECATED("renamed to MdsFightMode::CROSSBOW") = CROSSBOW,
		magic PHOENIX_DEPRECATED("renamed to MdsFightMode::MAGIC") = MAGIC,
		none PHOENIX_DEPRECATED("renamed to MdsFightMode::NONE") = NONE,
		invalid PHOENIX_DEPRECATED("renamed to MdsFightMode::INVALID") = INVALID,
	};

	enum class AnimationFlags : uint8_t {
		NONE = 0,
		MOVE = 1,
		ROTATE = 2,
		QUEUE = 4,
		FLY = 8,
		IDLE = 16,
		INPLACE = 32,

		af_none PHOENIX_DEPRECATED("renamed to AnimationFlags::NONE") = NONE,
		af_move PHOENIX_DEPRECATED("renamed to AnimationFlags::MOVE") = MOVE,
		af_rotate PHOENIX_DEPRECATED("renamed to AnimationFlags::ROTATE") = ROTATE,
		af_queue PHOENIX_DEPRECATED("renamed to AnimationFlags::QUEUE") = QUEUE,
		af_fly PHOENIX_DEPRECATED("renamed to AnimationFlags::FLY") = FLY,
		af_idle PHOENIX_DEPRECATED("renamed to AnimationFlags::IDLE") = IDLE,
		af_inplace PHOENIX_DEPRECATED("renamed to AnimationFlags::INPLACE") = INPLACE,
	};

	[[nodiscard]] PHOENIX_API bool operator&(AnimationFlags a, AnimationFlags b);
	[[nodiscard]] PHOENIX_API AnimationFlags operator|(AnimationFlags a, AnimationFlags b);
	PHOENIX_API AnimationFlags& operator|=(AnimationFlags& a, AnimationFlags b);

	/// \brief The way the animation is to be played.
	enum class AnimationDirection : uint8_t {
		FORWARD = 0,  ///< The animation samples are played from first to last.
		BACKWARD = 1, ///< The animation samples are played from last to first.

		// Deprecated entries.
		forward = FORWARD,
		backward = BACKWARD,
	};

	/// \brief The `meshAndTree` tag
	/// \remark MDS syntax: `meshAndTree(<string> [DONT_USE_MESH])`
	struct MdsSkeleton {
		std::string name;
		bool disable_mesh {false};
	};

	/// \brief The `modelTag` tag
	/// \remark MDS syntax: `modelTag(<string> <string>)`
	struct MdsModelTag {
		std::string bone;
	};

	/// \brief The `*eventTag` tag
	/// \remark MDS syntax: `*eventTag(<int> <string> [<string>] [<string>] [ATTACH])`
	struct MdsEventTag {
		std::int32_t frame;
		MdsEventType type;
		std::string slot {};
		std::string slot2 {};
		std::string item {};
		std::vector<int32_t> frames {};
		MdsFightMode fight_mode {MdsFightMode::NONE};
		bool attached {false};
	};

	/// \brief The `*eventPFX` tag
	/// \remark MDS syntax: `*eventPFX(<int> [<int>] <string> <string> [ATTACH])`
	struct MdsParticleEffect {
		std::int32_t frame;
		std::int32_t index {0};
		std::string name;
		std::string position;
		bool attached {false};
	};

	/// \brief The `*eventCamTremor` tag
	/// \remark MDS syntax: `*eventCamTremor(<int> <int> <int> <int> <int>)`
	struct MdsCameraTremor {
		std::int32_t frame {0};
		std::int32_t field1 {0};
		std::int32_t field2 {0};
		std::int32_t field3 {0};
		std::int32_t field4 {0};
	};

	/// \brief The `*eventPFXStop` tag
	/// \remark MDS syntax: `*eventPFXStop(<int> <int>)`
	struct MdsParticleEffectStop {
		std::int32_t frame;
		std::int32_t index;
	};

	/// \brief The `*eventSFX` tag
	/// \remark MDS syntax: `*eventSFX(<int> <string> [R:<float>] [EMPTY_SLOT])`
	struct MdsSoundEffect {
		std::int32_t frame;
		std::string name;
		float range {1000.0f};
		bool empty_slot {false};
	};

	/// \brief The `*eventSFXGrnd` tag
	/// \remark MDS syntax: `*eventSFXGrnd(<int> <string>)`
	struct MdsSoundEffectGround {
		std::int32_t frame;
		std::string name;
		float range {1000.0f};
		bool empty_slot {false};
	};

	/// \brief The `*eventMMStartAni` tag
	/// \remark MDS syntax: `*eventMMStartAni(<int> <string> [<string>])`
	struct MdsMorphAnimation {
		std::int32_t frame;
		std::string animation;
		std::string node {};
	};

	/// \brief The `aniAlias` tag
	/// \remark MDS syntax: `ani(<string> <int> <string> <float> <float> <flags> <string> <F|R> <int> <int>
	///         [FPS:<float>] [CVS:<float>])`
	struct MdsAnimation {
		std::string name;
		std::uint32_t layer;
		std::string next;
		float blend_in;
		float blend_out;
		AnimationFlags flags {AnimationFlags::NONE};
		std::string model;
		AnimationDirection direction;
		std::int32_t first_frame;
		std::int32_t last_frame;
		float fps;
		float speed;
		float collision_volume_scale;

		std::vector<MdsEventTag> events {};
		std::vector<MdsParticleEffect> pfx {};
		std::vector<MdsParticleEffectStop> pfx_stop {};
		std::vector<MdsSoundEffect> sfx {};
		std::vector<MdsSoundEffectGround> sfx_ground {};
		std::vector<MdsMorphAnimation> morph {};
		std::vector<MdsCameraTremor> tremors {};
	};

	/// \brief The `aniAlias` tag
	/// \remark MDS syntax: `aniAlias(<string> <int> <string> <float> <float> <flags> <string> [<F|R>])`
	struct MdsAnimationAlias {
		std::string name;
		std::uint32_t layer;
		std::string next;
		float blend_in;
		float blend_out;
		AnimationFlags flags {AnimationFlags::NONE};
		std::string alias;
		AnimationDirection direction;
	};

	/// \brief The `aniBlend` tag
	/// \remark MDS syntax: `aniBlend(<string> [<int>] <string> [<float> <float>])`
	struct MdsAnimationBlend {
		std::string name;
		std::string next;
		float blend_in {0};
		float blend_out {0};
	};

	/// \brief The `aniComb` tag
	/// \remark MDS syntax: `aniComb(<string> <int> <string> <float> <float> <flags> <string> <int>)`
	struct MdsAnimationCombine {
		std::string name;
		std::uint32_t layer;
		std::string next;
		float blend_in;
		float blend_out;
		AnimationFlags flags {AnimationFlags::NONE};
		std::string model;
		std::int32_t last_frame;
	};

	namespace mds {
		PHOENIX_DEPRECATED("use AnimationFlags::NONE") static constexpr auto const af_none = AnimationFlags::NONE;
		PHOENIX_DEPRECATED("use AnimationFlags::MOVE") static constexpr auto const af_move = AnimationFlags::MOVE;
		PHOENIX_DEPRECATED("use AnimationFlags::ROTATE") static constexpr auto const af_rotate = AnimationFlags::ROTATE;
		PHOENIX_DEPRECATED("use AnimationFlags::QUEUE") static constexpr auto const af_queue = AnimationFlags::QUEUE;
		PHOENIX_DEPRECATED("use AnimationFlags::FLY") static constexpr auto const af_fly = AnimationFlags::FLY;
		PHOENIX_DEPRECATED("use AnimationFlags::IDLE") static constexpr auto const af_idle = AnimationFlags::IDLE;
		PHOENIX_DEPRECATED("use AnimationFlags::INPLACE")
		static constexpr auto const af_inplace = AnimationFlags::INPLACE;

		PHOENIX_INTERNAL AnimationFlags animation_flags_from_string(std::string_view str);

		using animation_direction PHOENIX_DEPRECATED("renamed to AnimationDirection") = AnimationDirection;
		using animation_flags PHOENIX_DEPRECATED("use AnimationFlags instead") = AnimationFlags;
		using event_fight_mode PHOENIX_DEPRECATED("renamed to MdsFightMode") = MdsFightMode;
		using event_tag_type PHOENIX_DEPRECATED("renamed to MdsEventType") = MdsEventType;
		using skeleton PHOENIX_DEPRECATED("renamed to MdsSkeleton") = MdsSkeleton;
		using model_tag PHOENIX_DEPRECATED("renamed to MdsModelTag") = MdsModelTag;
		using event_tag PHOENIX_DEPRECATED("renamed to MdsEventTag") = MdsEventTag;
		using event_pfx PHOENIX_DEPRECATED("renamed to MdsParticleEffect") = MdsParticleEffect;
		using event_camera_tremor PHOENIX_DEPRECATED("renamed to MdsCameraTremor") = MdsCameraTremor;
		using event_pfx_stop PHOENIX_DEPRECATED("renamed to MdsParticleEffectStop") = MdsParticleEffectStop;
		using event_sfx PHOENIX_DEPRECATED("renamed to MdsSoundEffect") = MdsSoundEffect;
		using event_sfx_ground PHOENIX_DEPRECATED("renamed to MdsSoundEffectGround") = MdsSoundEffectGround;
		using event_morph_animate PHOENIX_DEPRECATED("renamed to MdsMorphAnimation") = MdsMorphAnimation;
		using animation PHOENIX_DEPRECATED("renamed to MdsAnimation") = MdsAnimation;
		using animation_alias PHOENIX_DEPRECATED("renamed to MdsAnimationAlias") = MdsAnimationAlias;
		using animation_blending PHOENIX_DEPRECATED("renamed to MdsAnimationBlend") = MdsAnimationBlend;
		using animation_combination PHOENIX_DEPRECATED("renamed to MdsAnimationCombine") = MdsAnimationCombine;
	} // namespace mds

	/// \brief Represents a *ZenGin* model script.
	///
	/// <p>Model scripts contain animations related to a model and actions the animation controller should take during
	/// or after an animation plays (such as playing a sound).</p>
	class ModelScript {
	public:
		/// \brief Parses a model script from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model script.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_API static ModelScript parse(Buffer& buf);

		/// \brief Parses a model script from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model script.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API inline static ModelScript parse(Buffer&& buf) {
			return ModelScript::parse(buf);
		}

	public:
		/// \brief The model skeleton this model script was made for.
		MdsSkeleton skeleton {};

		/// \brief A list of meshes which can be used with this model script.
		std::vector<std::string> meshes {};

		/// \brief A list of animation names which are disabled.
		std::vector<std::string> disabled_animations {};

		std::vector<MdsAnimationCombine> combinations {};
		std::vector<MdsAnimationBlend> blends {};
		std::vector<MdsAnimationAlias> aliases {};
		std::vector<MdsModelTag> model_tags {};
		std::vector<MdsAnimation> animations {};
	};

	using syntax_error PHOENIX_DEPRECATED("renamed to ScriptSyntaxError") = ScriptSyntaxError;
	using model_script PHOENIX_DEPRECATED("renamed to ModelScript") = ModelScript;
} // namespace phoenix
