// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "proto_mesh.hh"

namespace phoenix {
	class Buffer;

	/// \brief An animation used by morph meshes
	struct MorphAnimation {
		/// \brief The name of the animation.
		std::string name;

		/// \brief The animation layer the animation is played at.
		std::int32_t layer;
		float blend_in;
		float blend_out;

		/// \brief The duration of the animation in seconds.
		float duration;
		float speed;
		std::uint8_t flags;

		/// \brief The number of frames of the animation.
		std::uint32_t frame_count;

		/// \brief The set of vertices affected by the animation.
		std::vector<std::uint32_t> vertices;

		/// \brief The set of samples of the animation.
		std::vector<glm::vec3> samples;
	};

	/// \brief A reference to a morph mesh source file.
	struct MorphSource {
		/// \brief The date of file creation.
		Date file_date;

		/// \brief The name of the source file.
		std::string file_name;
	};

	/// \brief Represents a *ZenGin* morph mesh.
	///
	/// <p>Morph meshes represents meshes which can deform using a set of animations. With these meshes, the positions
	/// of the vertices of the underlying phoenix::proto_mesh are actually changed while an animation plays.</p>
	class MorphMesh {
	public:
		/// \brief Parses a morph mesh from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed morph mesh.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_API static MorphMesh parse(Buffer& buf);

		/// \brief Parses a morph mesh from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed morph mesh.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API inline static MorphMesh parse(Buffer&& buf) {
			return MorphMesh::parse(buf);
		}

	public:
		/// \brief The name of the mesh.
		std::string name {};

		/// \brief The underlying mesh.
		MultiResolutionMesh mesh {};

		/// \brief All morph positions associated with the mesh.
		std::vector<glm::vec3> morph_positions {};

		/// \brief All animations associated with the mesh.
		std::vector<MorphAnimation> animations {};

		/// \brief A list of source files this morph mesh was compiled from.
		std::vector<MorphSource> sources {};
	};

	using morph_mesh PHOENIX_DEPRECATED("renamed to MorphMesh") = MorphMesh;
	using morph_source PHOENIX_DEPRECATED("renamed to MorphSource") = MorphSource;
	using morph_animation PHOENIX_DEPRECATED("renamed to MorphAnimation") = MorphAnimation;
} // namespace phoenix
