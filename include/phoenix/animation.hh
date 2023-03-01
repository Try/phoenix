// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "math.hh"

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace phoenix {
	class Buffer;

	/// \brief A single sample of an Animation.
	///
	/// Each sample contains a position and a rotation for a given skeletal node. These nodes are defined
	/// in a skeleton, also called a 'model hierarchy' which must be loaded in addition to the Animation.
	///
	/// \see phoenix::Animation
	struct AnimationSample {
		glm::vec3 position;
		glm::quat rotation;

		[[nodiscard]] PHOENIX_API bool operator==(const AnimationSample& other) const noexcept;
	};

	/// \brief Types of animation events.
	enum class AnimationEventType : std::uint32_t {
		/// \brief Also known as "*eventTag"
		TAG = 0,

		/// \brief Also known as "*eventSfx"
		SOUND_EFFECT = 1,

		/// \brief Also known as "*eventSfxGrnd"
		SOUND_EFFECT_GROUND = 2,

		/// \brief Also known as "*aniBatch".
		/// \note The Gothic ModKit states that this feature is "unused, untested".
		BATCH = 3,

		/// \brief Also known as "*eventSwapMesh"
		SWAP_MESH = 4,

		/// \brief Also known as "*eventHeading"
		/// \note The Gothic ModKit state that "this ani-event is currently disabled because the
		///       implementation of the R-flag makes it obsolete".
		HEADING = 5,

		/// \brief Also known as "*eventPFX"
		PARTICLE_EFFECT = 6,

		/// \brief Also known as "*eventPFXGrnd"
		/// \note I can't find any relevant references to this event type. Possibly unused.
		PARTICLE_EFFECT_GROUND = 7,

		/// \brief Also known as "*eventPFXStop"
		PARTICLE_EFFECT_STOP = 8,

		/// \brief Also known as "*eventSetMesh"
		/// \note I can't find any relevant references to this event type. Possibly unused.
		SET_MESH = 9,

		/// \brief Also known as "*eventMMStartAni"
		MORPH_MESH_ANIMATION = 10,

		/// \brief Also known as "*eventCamTremor"
		CAMERA_TREMOR = 11,

		// Deprecated entries.
		tag PHOENIX_DEPRECATED("renamed to AnimationEventType::TAG") = TAG,
		sound PHOENIX_DEPRECATED("renamed to AnimationEventType::SOUND_EFFECT") = SOUND_EFFECT,
		sound_ground PHOENIX_DEPRECATED("renamed to AnimationEventType::SOUND_EFFECT_GROUND") = SOUND_EFFECT_GROUND,
		animation_batch PHOENIX_DEPRECATED("renamed to AnimationEventType::BATCH") = BATCH,
		swap_mesh PHOENIX_DEPRECATED("renamed to AnimationEventType::SWAP_MESH") = SWAP_MESH,
		heading PHOENIX_DEPRECATED("renamed to AnimationEventType::HEADING") = HEADING,
		pfx PHOENIX_DEPRECATED("renamed to AnimationEventType::PARTICLE_EFFECT") = PARTICLE_EFFECT,
		pfx_ground PHOENIX_DEPRECATED("renamed to AnimationEventType::PARTICLE_EFFECT_GROUND") = PARTICLE_EFFECT_GROUND,
		pfx_stop PHOENIX_DEPRECATED("renamed to AnimationEventType::PARTICLE_FX_STOP") = PARTICLE_EFFECT_STOP,
		set_mesh PHOENIX_DEPRECATED("renamed to AnimationEventType::SET_MESH") = SET_MESH,
		start_animation PHOENIX_DEPRECATED("renamed to AnimationEventType::MORPH_MESH_ANIMATION") =
		    MORPH_MESH_ANIMATION,
		tremor PHOENIX_DEPRECATED("renamed to AnimationEventType::CAMERA_TREMOR") = CAMERA_TREMOR,
	};

	/// \brief Represents an animation event.
	struct AnimationEvent {
		AnimationEventType type;

		union {
			std::uint32_t frame;
			PHOENIX_DEPRECATED("renamed to AnimationEvent::frame") std::uint32_t no;
		};

		std::string tag;
		std::string content[4];
		float values[4];
		float probability; // ?
	};

	/// \brief Represents a model animation.
	class Animation {
	public:
		/// \brief Parses an animation from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed animation.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		/// \see https://phoenix.lmichaelis.de/engine/formats/animation/
		[[nodiscard]] PHOENIX_API static Animation parse(Buffer& in);

		/// \brief Parses an animation from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed animation.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API static Animation parse(Buffer&& in);

	public:
		/// \brief The name of the animation
		std::string name {};

		/// \brief The next animation in queue.
		std::string next {};

		/// \brief The layer this animation is played in.
		std::uint32_t layer {};

		/// \brief The number of frames of this animation.
		std::uint32_t frame_count {};

		/// \brief The number of skeleton nodes this animation requires.
		std::uint32_t node_count {};

		/// \brief The number of frames of this animation to play per second.
		float fps {};

		/// \brief The number of frames per second the original model was animated with before being converted.
		float fps_source {};

		union {
			PHOENIX_DEPRECATED("renamed to sample_position_min") float sample_position_range_min {};
			float sample_position_min;
		};

		union {
			PHOENIX_DEPRECATED("renamed to sample_position_scale") float sample_position_scalar {};
			float sample_position_scale;
		};

		/// \brief The bounding box of the animation.
		AxisAlignedBoundingBox bbox {};

		/// \brief The checksum of the model hierarchy this animation was made for.
		std::uint32_t checksum {};

		/// \brief The original path of the animation script this animation was generated from.
		std::string source_path {};

		/// \brief The original model script snippet this animation was generated from.
		std::string source_script {};

		/// \brief The list of animation samples of this animation.
		std::vector<AnimationSample> samples {};

		/// \brief The list of animation events of this animation.
		/// \warning Though I could not find any source specifically mentioning this, all animation file I have seen
		///          **do not contain any events**, meaning **this vector will always be empty**. You should retrieve
		///          the relevant events from the model script file instead.
		std::vector<AnimationEvent> events {};

		/// \brief A list of model hierarchy node indices.
		std::vector<std::uint32_t> node_indices;
	};

	using animation PHOENIX_DEPRECATED("renamed to phoenix::Animation") = Animation;
	using animation_sample PHOENIX_DEPRECATED("renamed to phoenix::AnimationSample") = AnimationSample;
	using animation_event PHOENIX_DEPRECATED("renamed to phoenix::AnimationEvent") = AnimationEvent;
	using animation_event_type PHOENIX_DEPRECATED("renamed to phoenix::AnimationEventType") = AnimationEventType;
} // namespace phoenix
