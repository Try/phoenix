// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "Math.hh"
#include "MultiResolutionMesh.hh"

#include <vector>

namespace phoenix {
	class Buffer;

	struct SoftSkinWedgeNormal {
		glm::vec3 normal;
		std::uint32_t index;
	};

	struct SoftSkinWeightEntry {
		float weight;
		glm::vec3 position;
		std::uint8_t node_index;
	};

	/// \brief Represents a soft-skin mesh.
	class SoftSkinMesh {
	public:
		/// \brief Parses a soft-skin mesh from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed soft-skin mesh.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_API static SoftSkinMesh parse(Buffer& in);

		/// \brief Parses a soft-skin mesh from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed soft-skin mesh.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API inline static SoftSkinMesh parse(Buffer&& in) {
			return SoftSkinMesh::parse(in);
		}

	public:
		/// \brief The embedded proto-mesh.
		MultiResolutionMesh mesh;

		/// \brief The meshes bounding boxes (there is one for each node).
		std::vector<OrientedBoundingBox> bboxes;

		/// \brief A list of wedge normals.
		std::vector<SoftSkinWedgeNormal> wedge_normals;

		/// \brief Node weights.
		std::vector<std::vector<SoftSkinWeightEntry>> weights;

		/// \brief Nodes.
		std::vector<std::int32_t> nodes;
	};

	using softskin_mesh PHOENIX_DEPRECATED("renamed to SoftSkinMesh") = SoftSkinMesh;
	using wedge_normal PHOENIX_DEPRECATED("renamed to SoftSkinWedgeNormal") = SoftSkinWedgeNormal;
	using weight_entry PHOENIX_DEPRECATED("renamed to SoftSkinWeightEntry") = SoftSkinWeightEntry;
} // namespace phoenix
