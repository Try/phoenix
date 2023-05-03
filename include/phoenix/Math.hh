// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"

#include <glm/vec3.hpp>

#include <vector>

namespace phoenix {
	class Buffer;

	/// \brief Represents a axis-aligned bounding box (AABB)
	struct AxisAlignedBoundingBox {
		/// \brief The coordinates of the minimum corner of the bounding box.
		glm::vec3 min;

		/// \brief The coordinates of the maximum corner of the bounding box.
		glm::vec3 max;

		/// \brief Parses a bounding box from the given buffer.
		/// \param[in,out] in The buffer to parse from.
		/// \return The bounding box parsed.
		PHOENIX_API static AxisAlignedBoundingBox parse(Buffer& in);
	};

	/// \brief Represents an oriented bounding box.
	///
	/// In contrast to regular bounding boxes, [oriented bounding
	/// boxes](https://en.wikipedia.org/wiki/Minimum_bounding_box#Arbitrarily_oriented_minimum_bounding_box) may be
	/// rotated in the coordinate system and don't have to align with its axes.
	struct OrientedBoundingBox {
		glm::vec3 center;
		glm::vec3 axes[3];
		glm::vec3 half_width;

		std::vector<OrientedBoundingBox> children;

		/// \brief Calculates an axis-aligned bounding box from this oriented bounding box.
		/// \todo Write a test for this.
		/// \return An AABB which contains this OBB.
		[[nodiscard]] PHOENIX_API AxisAlignedBoundingBox as_bbox() const;

		/// \brief Parses an oriented bounding box from a buffer.
		/// \param[in,out] in The buffer to parse from.
		/// \return The parsed bounding box.
		[[nodiscard]] PHOENIX_API static OrientedBoundingBox parse(Buffer& in);
	};

	using bounding_box PHOENIX_DEPRECATED("renamed to AxisAlignedBoundingBox") = AxisAlignedBoundingBox;
	using obb PHOENIX_DEPRECATED("renamed to OrientedBoundingBox") = OrientedBoundingBox;
} // namespace phoenix
