// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/model.hh>

namespace phoenix {
	model model::parse(buffer& buf) {
		return phoenix::parse<model>(buf);
	}

	model model::parse(buffer&& buf) {
		return phoenix::parse<model>(buf);
	}

	template <>
	model parse<>(buffer& buf) {
		model tmp {};
		tmp.hierarchy = phoenix::parse<model_hierarchy>(buf);
		tmp.mesh = phoenix::parse<model_mesh>(buf);
		return tmp;
	}
} // namespace phoenix
