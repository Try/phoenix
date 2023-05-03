// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "Math.hh"

#include <glm/mat4x4.hpp>

#include <string>
#include <tuple>
#include <vector>

namespace phoenix {
	class Buffer;

	/// \brief A node in the hierarchy tree.
	struct ModelHierarchyNode {
		/// \brief The index of this node's parent node.
		std::int16_t parent_index;

		/// \brief The name of this node.
		std::string name;

		/// \brief The position and rotation of this node in its base state.
		glm::mat4x4 transform;
	};

	/// \brief Represents a *ZenGin* model hierarchy.
	///
	/// <p>Model hierarchy files represent the skeletal structure of a mesh. These skeletons are used to animate mostly
	/// animals and humans in the game which is commonly referred to as rigging.</p>
	class ModelHierarchy {
	public:
		/// \brief Parses a model hierarchy from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model hierarchy object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_API static ModelHierarchy parse(Buffer& in);

		/// \brief Parses a model hierarchy from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model hierarchy object.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API inline static ModelHierarchy parse(Buffer&& in) {
			return ModelHierarchy::parse(in);
		}

	public:
		/// \brief The list of nodes this hierarchy consists of.
		std::vector<ModelHierarchyNode> nodes {};

		/// \brief The bounding box of this hierarchy.
		AxisAlignedBoundingBox bbox {};

		/// \brief The collision bounding box of this hierarchy.
		AxisAlignedBoundingBox collision_bbox {};

		/// \brief The translation of the root node of this hierarchy.
		glm::vec3 root_translation {};

		/// \brief The checksum of this hierarchy.
		std::uint32_t checksum;
	};

	using model_hierarchy_node PHOENIX_DEPRECATED("renamed to ModelHierarchyNode") = ModelHierarchyNode;
	using model_hierarchy PHOENIX_DEPRECATED("renamed to ModelHierarchy") = ModelHierarchy;
} // namespace phoenix
