// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "material.hh"
#include "math.hh"
#include "phoenix.hh"
#include "proto_mesh.hh"
#include "texture.hh"

#include <memory>
#include <optional>
#include <unordered_set>

namespace phoenix {
	class Buffer;

	/// \brief Represents a light map.
	struct LightMap {
		std::shared_ptr<Texture> image;
		glm::vec3 normals[2];
		glm::vec3 origin;
	};

	/// \brief Represents a vertex feature.
	struct VertexFeature {
		/// \brief The texture coordinates of the polygon.
		glm::vec2 texture;

		/// \brief A value indicating the light level of the polygon.
		uint32_t light;

		/// The normal vector of the polygon.
		glm::vec3 normal;
	};

	/// \brief Flags set for a polygon of a mesh.
	struct PolygonFlags {
		std::uint8_t is_portal : 2;
		std::uint8_t is_occluder : 1;
		std::uint8_t is_sector : 1;
		std::uint8_t should_relight : 1;
		std::uint8_t is_outdoor : 1;
		std::uint8_t is_ghost_occluder : 1;
		std::uint8_t is_dynamically_lit : 1;
		std::int16_t sector_index : 16;

		uint8_t is_lod : 1;
		uint8_t normal_axis : 2;

		PHOENIX_API bool operator==(const PolygonFlags& b) const;
	};

	/// \brief List of data indices for polygons of meshes.
	struct PolygonList {
		/// \brief The index of the material for each polygon in mesh::materials
		std::vector<uint32_t> material_indices {};

		/// \brief The index of the light map for each polygon in mesh::lightmaps. May be -1.
		std::vector<int32_t> lightmap_indices {};

		/// \brief The index of the vertex features of each polygon in mesh::features.
		std::vector<uint32_t> feature_indices {};

		/// \brief The indices of each vertex for a polygon. Three consecutive values form one polygon.
		std::vector<uint32_t> vertex_indices {};

		/// \brief The flags for each polygon.
		std::vector<PolygonFlags> flags {};
	};

	/// \brief Represents a *ZenGin* basic mesh.
	///
	/// <p>Found in files with the `MSH` extension and as the world meshes in world archives, these meshes contain a
	/// set of polygons and accompanying data describing a static mesh in three dimensions.</p>
	class Mesh {
	public:
		/// \brief Parses a mesh from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in,out] buf The buffer to read from.
		/// \param include_polygons A list of polygon indices to include in the final mesh. All other polygons are
		///                         discarded. This is mainly used for world meshes which include level-of-detail
		///                         polygons.
		/// \return The parsed mesh object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&, const std::vector<std::uint32_t>&)
		[[nodiscard]] PHOENIX_API static Mesh
		parse(Buffer& buf, std::optional<std::unordered_set<std::uint32_t>> const& include_polygons = std::nullopt);

		/// \brief Parses a mesh from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \param include_polygons A list of polygon indices to include in the final mesh. All other polygons are
		///                         discarded. This is mainly used for world meshes which include level-of-detail
		///                         polygons.
		/// \return The parsed mesh object.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&, const std::vector<std::uint32_t>&)
		[[nodiscard]] PHOENIX_API inline static Mesh
		parse(Buffer&& buf, std::optional<std::unordered_set<std::uint32_t>> const& include_polygons = std::nullopt) {
			return Mesh::parse(buf, include_polygons);
		}

	public:
		/// \brief The creation date of this mesh.
		Date date {};

		/// \brief The name of this mesh
		std::string name {};

		/// \brief The bounding box of this mesh.
		AxisAlignedBoundingBox bbox {};

		/// \brief The oriented bbox tree of this mesh.
		OrientedBoundingBox obb {};

		/// \brief A list of materials used by this mesh.
		std::vector<Material> materials {};

		/// \brief A list of vertices of this mesh.
		std::vector<glm::vec3> vertices {};

		/// \brief A list of vertex features of this mesh.
		std::vector<VertexFeature> features {};

		/// \brief All shared lightmaps associated with this mesh
		std::vector<LightMap> lightmaps {};

		/// \brief A list of polygons of this mesh.
		PolygonList polygons {};
	};

	using light_map PHOENIX_DEPRECATED("renamed to LightMap") = LightMap;
	using vertex_feature PHOENIX_DEPRECATED("renamed to VertexFeature") = VertexFeature;
	using polygon_flags PHOENIX_DEPRECATED("renamed to PolygonFlags") = PolygonFlags;
	using polygon_list PHOENIX_DEPRECATED("renamed to PolygonList") = PolygonList;
	using mesh PHOENIX_DEPRECATED("renamed to Mesh") = Mesh;
} // namespace phoenix
