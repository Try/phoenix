// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/buffer.hh"
#include "phoenix/softskin_mesh.hh"

namespace phoenix {
	enum class SoftSkinChunkType { UNKNOWN, HEADER = 0xE100, END = 0xE110, MESH = 0xB100, NODES = 0xB1FF };

	SoftSkinMesh SoftSkinMesh::parse(Buffer& in) {
		SoftSkinMesh msh {};
		SoftSkinChunkType type = SoftSkinChunkType::UNKNOWN;
		bool end_mesh = false;

		do {
			type = static_cast<SoftSkinChunkType>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			switch (type) {
			case SoftSkinChunkType::HEADER:
				(void) /* version = */ chunk.get_uint();
				break;
			case SoftSkinChunkType::MESH:
				msh.mesh = MultiResolutionMesh::parse_from_section(chunk);
				break;
			case SoftSkinChunkType::NODES: {
				// weights
				auto weight_buffer_size = chunk.get_uint();
				auto weight_buffer_end = chunk.position() + weight_buffer_size;

				msh.weights.resize(msh.mesh.positions.size());
				for (uint32_t i = 0; i < msh.mesh.positions.size(); ++i) {
					auto count = chunk.get_uint();
					msh.weights[i].resize(count);

					for (std::uint32_t j = 0; j < count; ++j) {
						auto& weight = msh.weights[i][j];
						weight.weight = chunk.get_float();
						weight.position = chunk.get_vec3();
						weight.node_index = chunk.get();
					}
				}

				if (chunk.position() != weight_buffer_end) {
					PX_LOGW("SoftSkinMesh: ",
					        weight_buffer_end - chunk.position(),
					        " bytes remaining in weight section");
					chunk.position(weight_buffer_end);
				}

				// wedge normals
				msh.wedge_normals.resize(chunk.get_uint());

				for (std::uint32_t i = 0; i < msh.wedge_normals.size(); ++i) {
					auto& normal = msh.wedge_normals[i];
					normal.normal = chunk.get_vec3();
					normal.index = chunk.get_uint();
				}

				// nodes
				msh.nodes.resize(chunk.get_ushort());

				for (std::uint32_t i = 0; i < msh.nodes.size(); ++i) {
					msh.nodes[i] = chunk.get_int();
				}

				for (std::uint32_t i = 0; i < msh.nodes.size(); ++i) {
					msh.bboxes.push_back(OrientedBoundingBox::parse(chunk));
				}

				break;
			}
			case SoftSkinChunkType::END:
				end_mesh = true;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("SoftSkinMesh: ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (!end_mesh);

		return msh;
	}
} // namespace phoenix
