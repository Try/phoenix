// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/model_hierarchy.hh"
#include "phoenix/buffer.hh"

namespace phoenix {
	enum class ModelHierarchyChunkType { UNKNOWN, HIERARCHY = 0xD100, SOURCE = 0xD110, END = 0xD120 };

	ModelHierarchy ModelHierarchy::parse(Buffer& in) {
		ModelHierarchy hierarchy {};

		ModelHierarchyChunkType type = ModelHierarchyChunkType::UNKNOWN;
		bool end_hierarchy = false;

		do {
			type = static_cast<ModelHierarchyChunkType>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			switch (type) {
			case ModelHierarchyChunkType::HIERARCHY: {
				(void) /* version = */ chunk.get_uint(); // Should be 3 for G2?
				auto node_count = chunk.get_ushort();

				for (int32_t i = 0; i < node_count; ++i) {
					auto& node = hierarchy.nodes.emplace_back();
					node.name = chunk.get_line(false);
					node.parent_index = chunk.get_short();
					node.transform = chunk.get_mat4x4();
				}

				hierarchy.bbox = AxisAlignedBoundingBox::parse(chunk);
				hierarchy.collision_bbox = AxisAlignedBoundingBox::parse(chunk);
				hierarchy.root_translation = chunk.get_vec3();
				hierarchy.checksum = chunk.get_uint();
				break;
			}
			case ModelHierarchyChunkType::SOURCE:
				(void) /* date = */ Date::parse(chunk);
				(void) /* path = */ chunk.get_line(false);
				break;
			case ModelHierarchyChunkType::END:
				end_hierarchy = true;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("ModelHierarchy: ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (!end_hierarchy);

		return hierarchy;
	}
} // namespace phoenix
