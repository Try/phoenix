// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include "../Math.hh"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace phoenix {
	class Buffer;

	enum class BspTreeType : std::uint32_t {
		INDOOR = 0,
		OUTDOOR = 1,

		// Deprecated entries.
		indoor PHOENIX_DEPRECATED("renamed to BspTreeType::INDOOR") = INDOOR,
		outdoor PHOENIX_DEPRECATED("renamed to BspTreeType::OUTDOOR") = OUTDOOR,
	};

	/// \brief Represents a BSP tree node.
	struct BspNode {
		glm::vec4 plane;
		AxisAlignedBoundingBox bbox;
		std::uint32_t polygon_index;
		std::uint32_t polygon_count;

		std::int32_t front_index {-1};
		std::int32_t back_index {-1};
		std::int32_t parent_index {-1};

		[[nodiscard]] PHOENIX_API inline bool is_leaf() const noexcept {
			return front_index == -1 && back_index == -1;
		}
	};

	/// \brief Represents a BSP sector.
	struct BspSector {
		std::string name;
		std::vector<std::uint32_t> node_indices;
		std::vector<std::uint32_t> portal_polygon_indices;
	};

	/// \brief Represents a binary space partitioning tree as implemented in the ZenGin.
	///
	/// [Binary space partitioning](https://en.wikipedia.org/wiki/Binary_space_partitioning) is used for rapidly
	/// drawing three-dimensional scenes and performing ray-casts.
	class BspTree {
	public:
		/// \brief Parses a BSP tree from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).
		///
		/// \param[in,out] buf The buffer to read from.
		/// \param version The version identifier of the tree in the buffer.
		/// \return The parsed BSP tree.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_INTERNAL static BspTree parse(Buffer& in, std::uint32_t version);

		/// \brief Parses a BSP tree from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \param version The version identifier of the tree in the buffer.
		/// \return The parsed BSP tree.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_INTERNAL inline static BspTree parse(Buffer&& in, std::uint32_t version) {
			return parse(in, version);
		}

	public:
		/// \brief The mode of the tree (either indoor or outdoor).
		BspTreeType mode;

		/// \brief A list of polygon indices.
		std::vector<std::uint32_t> polygon_indices;

		/// \brief All BSP leaf polygon indices.
		std::unordered_set<std::uint32_t> leaf_polygons;

		/// \brief All BSP light points.
		std::vector<glm::vec3> light_points;

		/// \brief All BSP sectors.
		std::vector<BspSector> sectors;

		/// \brief Polygon indices of portals.
		std::vector<std::uint32_t> portal_polygon_indices;

		/// \brief All BSP nodes associated with the tree.
		std::vector<BspNode> nodes;

		/// \brief All BSP leaf node indices.
		std::vector<std::uint64_t> leaf_node_indices;
	};

	using bsp_tree PHOENIX_DEPRECATED("renamed to BspTree") = BspTree;
	using bsp_sector PHOENIX_DEPRECATED("renamed to BspSector") = BspSector;
	using bsp_node PHOENIX_DEPRECATED("renamed to BspNode") = BspNode;
	using bsp_tree_mode PHOENIX_DEPRECATED("renamed to BspTreeType") = BspTreeType;
} // namespace phoenix
