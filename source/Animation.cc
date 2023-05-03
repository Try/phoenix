// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/Animation.hh"
#include "phoenix/Buffer.hh"

#include <cmath>

namespace phoenix {
	/// \brief The highest number representable by a single rotation component.
	static const float SAMPLE_ROTATION_RANGE = float(1 << 16) - 1.0f;

	/// \brief The scaling factor applied to each rotation component.
	static const float SAMPLE_ROTATION_SCALE = (1.0f / SAMPLE_ROTATION_RANGE) * 2.1f;

	/// \brief The number half way to `SAMPLE_ROTATION_RANGE`.
	static const std::uint16_t SAMPLE_ROTATION_MID = (1 << 15) - 1;

	enum class AnimationChunkType {
		MARKER = 0xa000u,
		SOURCE = 0xa010u,
		HEADER = 0xa020u,
		EVENTS = 0xa030u,
		SAMPLES = 0xa090u,
	};

	bool AnimationSample::operator==(const AnimationSample& other) const noexcept {
		return this->position == other.position && this->rotation == other.rotation;
	}

	/// \brief Reads the position of a single animation sample from the given buffer.
	/// \param in The buffer to read from.
	/// \param scale The scaling factor to apply (taken from the animation's header).
	/// \param minimum The value of the smallest position component in the animation (part of its header).
	/// \return A vector containing the parsed position.
	/// \see http://phoenix.lmichaelis.de/engine/formats/animation/#sample-positions
	static glm::vec3 read_sample_position(Buffer& in, float scale, float minimum) {
		glm::vec3 v {};
		v.x = (float) in.get_ushort() * scale + minimum;
		v.y = (float) in.get_ushort() * scale + minimum;
		v.z = (float) in.get_ushort() * scale + minimum;
		return v;
	}

	/// \brief Reads the rotation of a single animation sample from the given buffer.
	/// \param in The buffer to read from.
	/// \return A quaternion containing the parsed rotation.
	/// \see http://phoenix.lmichaelis.de/engine/formats/animation/#sample-rotations
	static glm::quat read_sample_quaternion(Buffer& in) {
		glm::quat v {};
		v.x = ((float) in.get_ushort() - SAMPLE_ROTATION_MID) * SAMPLE_ROTATION_SCALE;
		v.y = ((float) in.get_ushort() - SAMPLE_ROTATION_MID) * SAMPLE_ROTATION_SCALE;
		v.z = ((float) in.get_ushort() - SAMPLE_ROTATION_MID) * SAMPLE_ROTATION_SCALE;

		float len_q = v.x * v.x + v.y * v.y + v.z * v.z;

		if (len_q > 1.0f) {
			float l = 1.0f / ::sqrtf(len_q);
			v.x *= l;
			v.y *= l;
			v.z *= l;
			v.w = 0;
		} else {
			// We know the quaternion has to be a unit quaternion, so we can calculate the missing value.
			v.w = ::sqrtf(1.0f - len_q);
		}

		return v;
	}

	Animation Animation::parse(Buffer& buf) {
		Animation anim {};
		AnimationChunkType type;

		do {
			type = static_cast<AnimationChunkType>(buf.get_ushort());
			auto chunk = buf.extract(buf.get_uint());

			switch (type) {
			case AnimationChunkType::MARKER:
				break;
			case AnimationChunkType::SOURCE:
				// Quirk: This was intended to be a date but the original code uses an uninitialized variable here
				//        so the actual data stored does not make any sense.
				(void) Date::parse(chunk);

				anim.source_path = chunk.get_line(false);
				anim.source_script = chunk.get_line(false);
				break;
			case AnimationChunkType::HEADER:
				(void) /* version = */ chunk.get_ushort(); // Should be 0xc for G2
				anim.name = chunk.get_line(false);
				anim.layer = chunk.get_uint();
				anim.frame_count = chunk.get_uint();
				anim.node_count = chunk.get_uint();
				anim.fps = chunk.get_float();
				anim.fps_source = chunk.get_float();
				anim.sample_position_min = chunk.get_float();
				anim.sample_position_scale = chunk.get_float();
				anim.bbox = AxisAlignedBoundingBox::parse(chunk);
				anim.next = chunk.get_line(false);
				break;
			case AnimationChunkType::EVENTS:
				anim.events.reserve(chunk.get_uint());

				for (std::uint32_t i = 0; i < anim.events.size(); ++i) {
					auto& event = anim.events.emplace_back();
					event.type = static_cast<AnimationEventType>(chunk.get_uint());
					event.frame = chunk.get_uint();
					event.tag = chunk.get_line();

					for (auto& j : event.content) {
						j = chunk.get_line();
					}

					for (float& value : event.values) {
						value = chunk.get_float();
					}

					event.probability = chunk.get_float();
				}

				break;
			case AnimationChunkType::SAMPLES:
				anim.checksum = chunk.get_uint();
				anim.node_indices.resize(anim.node_count);

				for (std::uint32_t i = 0; i < anim.node_count; ++i) {
					anim.node_indices[i] = chunk.get_uint();
				}

				anim.samples.resize(anim.node_count * anim.frame_count);

				for (std::size_t i = 0; i < anim.samples.size(); ++i) {
					anim.samples[i].rotation = read_sample_quaternion(chunk);
					anim.samples[i].position =
					    read_sample_position(chunk, anim.sample_position_scale, anim.sample_position_min);
				}

				break;
			default:
				break;
			}

			if (chunk.remaining() > 0) {
				PX_LOGW("Animation(\"",
				        anim.name,
				        "\"): ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (buf.remaining() != 0);

		return anim;
	}

	Animation Animation::parse(Buffer&& in) {
		return Animation::parse(in);
	}
} // namespace phoenix
