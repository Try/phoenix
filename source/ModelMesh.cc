// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/ModelMesh.hh"
#include "phoenix/Buffer.hh"

namespace phoenix {
	enum class ModelMeshChunkType {
		UNKNOWN,
		HEADER = 0xD000,
		SOURCE = 0xD010,
		NODES = 0xD020,
		SOFTSKINS = 0xD030,
		END = 0xD040,
		ATTACHMENTS = 0xB100,
	};

	ModelMesh ModelMesh::parse(Buffer& in) {
		ModelMesh msh {};
		ModelMeshChunkType type = ModelMeshChunkType::UNKNOWN;
		bool end_mesh = false;

		std::vector<std::string> attachment_names {};

		do {
			type = static_cast<ModelMeshChunkType>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			switch (type) {
			case ModelMeshChunkType::HEADER:
				(void) /* version = */ chunk.get_uint();
				break;
			case ModelMeshChunkType::SOURCE: {
				(void) /* date = */ Date::parse(chunk);
				(void) /* source file = */ chunk.get_line(false);
				break;
			}
			case ModelMeshChunkType::NODES: {
				auto node_count = chunk.get_ushort();

				for (std::uint32_t i = 0; i < node_count; ++i) {
					attachment_names.push_back(chunk.get_line());
				}
				break;
			}
			case ModelMeshChunkType::ATTACHMENTS:
				msh.attachments[attachment_names[msh.attachments.size()]] =
				    MultiResolutionMesh::parse_from_section(chunk);
				break;
			case ModelMeshChunkType::SOFTSKINS: {
				msh.checksum = chunk.get_uint();
				auto count = chunk.get_ushort();
				msh.meshes.reserve(count);

				// Quirk: the meshes are not embedded within the chunk (as in: the stored length does not contain
				//        the size of these meshes) so they have to be read directly from `in`.
				for (int32_t i = 0; i < count; ++i) {
					msh.meshes.push_back(SoftSkinMesh::parse(in));
				}
				break;
			}
			case ModelMeshChunkType::END:
				end_mesh = true;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("ModelMesh: ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (!end_mesh);

		return msh;
	}
} // namespace phoenix
