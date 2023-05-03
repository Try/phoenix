// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/buffer.hh"
#include "phoenix/world/bsp_tree.hh"

namespace phoenix {
	static constexpr auto version_g1 = 0x2090000;
	[[maybe_unused]] static constexpr auto version_g2 = 0x4090000;

	enum class BspChunkType : std::uint16_t {
		UNKNOWN,
		HEADER = 0xC000,
		POLYGONS = 0xC010,
		TREE = 0xC040,
		OUTDOOR = 0xC050,
		LIGHT = 0xC045,
		END = 0xC0FF
	};

	static void _parse_bsp_nodes(Buffer& in,
	                             std::vector<BspNode>& nodes,
	                             std::vector<std::uint64_t>& indices,
	                             std::uint32_t version,
	                             std::int32_t parent_index,
	                             bool leaf = false) {
		std::uint64_t self_index = nodes.size();

		auto& node = nodes.emplace_back();
		node.parent_index = parent_index;
		node.bbox = AxisAlignedBoundingBox::parse(in);
		node.polygon_index = in.get_uint();
		node.polygon_count = in.get_uint();

		if (leaf) {
			indices.push_back(self_index);
		} else {
			auto flags = in.get();

			node.plane = {};
			node.plane.w = in.get_float();
			node.plane.x = in.get_float();
			node.plane.y = in.get_float();
			node.plane.z = in.get_float();

			if (version == version_g1) {
				(void) in.get(); // "lod-flag"
			}

			if ((flags & 0x01) != 0) {
				node.front_index = std::int32_t(nodes.size());
				_parse_bsp_nodes(in, nodes, indices, version, std::int32_t(self_index), (flags & 0x04) != 0);
			}

			if ((flags & 0x02) != 0) {
				node.back_index = std::int32_t(nodes.size());
				_parse_bsp_nodes(in, nodes, indices, version, std::int32_t(self_index), (flags & 0x08) != 0);
			}
		}
	}

	BspTree BspTree::parse(Buffer& in, std::uint32_t version) {
		BspTree bsp {};
		BspChunkType type = BspChunkType::UNKNOWN;
		bool finished = false;

		do {
			type = static_cast<BspChunkType>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			PX_LOGI("BspTree: parsing chunk ", std::hex, std::uint16_t(type));

			switch (type) {
			case BspChunkType::HEADER:
				(void) chunk.get_ushort();
				bsp.mode = static_cast<BspTreeType>(chunk.get_uint());
				break;
			case BspChunkType::POLYGONS:
				bsp.polygon_indices.resize(chunk.get_uint());

				for (uint32_t& index : bsp.polygon_indices) {
					index = chunk.get_uint();
				}
				break;
			case BspChunkType::TREE: {
				uint32_t node_count = chunk.get_uint();
				uint32_t leaf_count = chunk.get_uint();

				bsp.nodes.reserve(node_count);
				bsp.leaf_node_indices.reserve(leaf_count);

				_parse_bsp_nodes(chunk, bsp.nodes, bsp.leaf_node_indices, version, -1);

				for (auto idx : bsp.leaf_node_indices) {
					auto& node = bsp.nodes[idx];

					for (uint32_t i = 0; i < node.polygon_count; ++i) {
						bsp.leaf_polygons.insert(bsp.polygon_indices[node.polygon_index + i]);
					}
				}

				assert(node_count == bsp.nodes.size());
				assert(leaf_count == bsp.leaf_node_indices.size());
				break;
			}
			case BspChunkType::LIGHT: {
				bsp.light_points.resize(bsp.leaf_node_indices.size());

				for (std::uint32_t i = 0; i < bsp.light_points.size(); ++i) {
					bsp.light_points[i] = chunk.get_vec3();
				}
				break;
			}
			case BspChunkType::OUTDOOR: {
				auto sector_count = chunk.get_uint();
				bsp.sectors.reserve(sector_count);

				for (std::uint32_t i = 0; i < sector_count; ++i) {
					auto& sector = bsp.sectors.emplace_back();

					sector.name = chunk.get_line(false);

					auto node_count = chunk.get_uint();
					auto polygon_count = chunk.get_uint();

					sector.node_indices.resize(node_count);
					sector.portal_polygon_indices.resize(polygon_count);

					for (std::uint32_t j = 0; j < node_count; ++j) {
						sector.node_indices[j] = chunk.get_uint();
					}

					for (std::uint32_t j = 0; j < polygon_count; ++j) {
						sector.portal_polygon_indices[j] = chunk.get_uint();
					}
				}

				auto portal_count = chunk.get_uint();
				bsp.portal_polygon_indices.resize(portal_count);

				for (std::uint32_t i = 0; i < portal_count; ++i) {
					bsp.portal_polygon_indices[i] = chunk.get_uint();
				}
				break;
			}
			case BspChunkType::END:
				(void) chunk.get();
				finished = true;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("BspTree: ", chunk.remaining(), " bytes remaining in section ", std::hex, std::uint16_t(type));
			}
		} while (!finished);

		return bsp;
	}
} // namespace phoenix
