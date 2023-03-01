// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "model_hierarchy.hh"
#include "model_mesh.hh"

#include <string>

namespace phoenix {
	class Buffer;

	/// \brief Represents a *ZenGin* model.
	///
	/// <p>*ZenGin* models contain a phoenix::model_mesh and a phoenix::model_hierarchy bundled into one file. Try are
	/// typically found in files with the `MDL` extension.</p>
	class Model {
	public:
		/// \brief Parses a model from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_API static Model parse(Buffer& buf);

		/// \brief Parses a model from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model object.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API inline static Model parse(Buffer&& buf) {
			return Model::parse(buf);
		}

	public:
		/// \brief The phoenix::model_hierarchy associated with this model.
		ModelHierarchy hierarchy {};

		/// \brief The phoenix::model_mesh associated with this model.
		ModelMesh mesh {};
	};

	using model PHOENIX_DEPRECATED("renamed to Model") = Model;
} // namespace phoenix
