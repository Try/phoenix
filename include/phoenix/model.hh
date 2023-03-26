// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include "phoenix/Api.hh"
#include <string>

#include <phoenix/model_hierarchy.hh>
#include <phoenix/model_mesh.hh>

namespace phoenix {
	/// \brief Represents a *ZenGin* model.
	///
	/// <p>*ZenGin* models contain a phoenix::model_mesh and a phoenix::model_hierarchy bundled into one file. Try are
	/// typically found in files with the `MDL` extension.</p>
	class model {
	public:
		/// \brief Parses a model from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_DEPRECATED("use phoenix::parse<phoenix::model>()") PHOENIX_API static model
		    parse(buffer& buf);

		/// \brief Parses a model from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model object.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_DEPRECATED("use phoenix::parse<phoenix::model>()") PHOENIX_API static model
		    parse(buffer&& buf);

	public:
		/// \brief The phoenix::model_hierarchy associated with this model.
		model_hierarchy hierarchy {};

		/// \brief The phoenix::model_mesh associated with this model.
		model_mesh mesh {};
	};

	/// \brief Parses a model from the data in the given buffer.
	/// \param[in,out] buf The buffer to read from.
	/// \return The parsed model object.
	/// \note After this function returns the position of \p buf will be at the end of the parsed object.
	///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
	///       using buffer::duplicate.
	/// \throws parser_error if parsing fails.
	/// \see #parse(buffer&&)
	template <>
	PHOENIX_API model parse<>(buffer& buf);
} // namespace phoenix
