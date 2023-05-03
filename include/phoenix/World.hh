// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "Mesh.hh"
#include "world/BspTree.hh"
#include "world/VobTree.hh"
#include "world/WayNet.hh"

#include <memory>
#include <vector>

namespace phoenix {
	class Buffer;

	/// \brief Represents a ZenGin world.
	class World {
	public:
		/// \brief Parses a world from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).
		///
		/// \param[in,out] buf The buffer to read from.
		/// \param version The Gothic version to assume when loading the world
		/// \return The parsed world object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API static World parse(Buffer& buf, GameVersion version);

		/// \brief Parses a world from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib). This function will try to determine the world version
		/// automatically. If it can't be detected, _phoenix_ will assume `game_version::gothic_1` and log
		/// an error message.
		///
		/// Using this function over #parse(buffer&, game_version) has the potential to lead to longer load times
		/// but it has not been proven that such a case actually exists. If you absolutely need the performance and
		/// you already know the game version you are trying to load, consider using #parse(buffer&, game_version)
		/// instead.
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed world object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] PHOENIX_API static World parse(Buffer& buf);

		/// \brief Parses a world from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from (by rvalue-reference).
		/// \param version The Gothic version to assume when loading the world
		/// \return The parsed world object.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API inline static World parse(Buffer&& buf, GameVersion version) {
			return World::parse(buf, version);
		}

		/// \brief Parses a world from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed world object.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] PHOENIX_API inline static World parse(Buffer&& buf) {
			return World::parse(buf);
		}

		/// \brief The list of VObs defined in this world.
		std::vector<std::unique_ptr<VirtualObject>> world_vobs;

		/// \brief The mesh of the world.
		Mesh world_mesh;

		/// \brief The BSP-tree of this world.
		BspTree world_bsp_tree;

		/// \brief The way-net of this world.
		WayNet world_way_net;
	};

	using world PHOENIX_DEPRECATED("renamed to World") = World;
} // namespace phoenix
