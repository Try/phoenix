// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include "../Buffer.hh"
#include "../Texture.hh"

namespace phoenix {
	/// \brief Converts a texture to the DDS format.
	/// \param tex The texture to convert.
	/// \return A buffer containing the DDS file.
	[[nodiscard]] PHOENIX_API Buffer texture_to_dds(const Texture& tex);
} // namespace phoenix
