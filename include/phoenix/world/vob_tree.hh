// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../phoenix.hh"
#include "../vobs/vob.hh"

#include <memory>

namespace phoenix {
	class ArchiveReader;

	/// \brief Parses a VOB tree from the given reader.
	/// \param in The reader to read from.
	/// \param version The version of Gothic being used.
	/// \return The tree parsed.
	PHOENIX_API std::unique_ptr<VirtualObject> parse_vob_tree(ArchiveReader& in, GameVersion version);
} // namespace phoenix
