// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/Vfs.hh>

#include <iostream>

void print_entries(std::vector<phoenix::VfsNode> const& entries) {
	for (auto& e : entries) {
		if (e.type() == phoenix::VfsNodeType::DIRECTORY) {
			print_entries(e.children());
		} else {
			std::cout << "    " << e.name() << "\n";
		}
	}
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Please provide an input file.";
		return -1;
	}

	auto vfs = phoenix::Vfs {};
	vfs.mount_disk(argv[1]);
	print_entries(vfs.root().children());
	return 0;
}
