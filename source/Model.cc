// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/Model.hh"

namespace phoenix {
	Model Model::parse(Buffer& buf) {
		Model tmp {};

		tmp.hierarchy = ModelHierarchy::parse(buf);
		tmp.mesh = ModelMesh::parse(buf);
		return tmp;
	}
} // namespace phoenix
