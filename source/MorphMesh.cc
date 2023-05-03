// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/MorphMesh.hh"
#include "phoenix/Buffer.hh"

namespace phoenix {
	enum class MorphMeshChunkType {
		UNKNOWN,
		SOURCE = 0xE010,
		HEADER = 0xE020,
		ANIMATION = 0xE030,
		MESH = 0xB100,
		MORPH = 0xB1FF
	};

	MorphMesh MorphMesh::parse(Buffer& in) {
		MorphMesh msh {};
		MorphMeshChunkType type = MorphMeshChunkType::UNKNOWN;

		do {
			type = static_cast<MorphMeshChunkType>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			switch (type) {
			case MorphMeshChunkType::SOURCE: {
				auto count = chunk.get_ushort();
				msh.sources.resize(count);

				for (int32_t i = 0; i < count; ++i) {
					msh.sources[i].file_date = Date::parse(chunk);
					msh.sources[i].file_name = chunk.get_line();
				}

				break;
			}
			case MorphMeshChunkType::HEADER:
				/* version = */ (void) chunk.get_uint();
				msh.name = chunk.get_line();
				break;
			case MorphMeshChunkType::MESH:
				msh.mesh = MultiResolutionMesh::parse_from_section(chunk);
				msh.morph_positions.resize(msh.mesh.positions.size());
				break;
			case MorphMeshChunkType::MORPH:
				for (auto& morph_position : msh.morph_positions) {
					morph_position = chunk.get_vec3();
				}
				break;
			case MorphMeshChunkType::ANIMATION: {
				auto animation_count = chunk.get_ushort();
				msh.animations.reserve(animation_count);

				for (int32_t i = 0; i < animation_count; ++i) {
					auto& anim = msh.animations.emplace_back();
					anim.name = chunk.get_line(false);
					anim.blend_in = chunk.get_float();
					anim.blend_out = chunk.get_float();
					anim.duration = chunk.get_float();
					anim.layer = chunk.get_int();
					anim.speed = chunk.get_float();
					anim.flags = chunk.get();

					auto vertex_count = chunk.get_uint();
					anim.frame_count = chunk.get_uint();

					anim.vertices.resize(vertex_count);
					anim.samples.resize(anim.frame_count * vertex_count);

					for (std::uint32_t j = 0; j < vertex_count; ++j) {
						anim.vertices[j] = chunk.get_uint();
					}

					for (std::uint32_t j = 0; j < vertex_count * anim.frame_count; ++j) {
						anim.samples[j] = chunk.get_vec3();
					}
				}
				break;
			}
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("MorphMesh: ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (in.remaining() != 0);

		return msh;
	}
} // namespace phoenix
