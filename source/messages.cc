// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/messages.hh"
#include "phoenix/archive.hh"

namespace phoenix {
	CutsceneLibrary CutsceneLibrary::parse(Buffer& buf) {
		auto archive = ArchiveReader::open(buf);
		CutsceneLibrary msgs {};

		ArchiveObject obj;
		if (!archive->read_object_begin(obj)) {
			throw ParserError {"CutsceneLibrary", "root object missing"};
		}

		if (obj.class_name != "zCCSLib") {
			throw ParserError {"CutsceneLibrary", "root object is not 'zCCSLib'"};
		}

		auto item_count = archive->read_int(); // NumOfItems
		msgs.blocks.reserve(static_cast<std::uint64_t>(item_count));

		for (int32_t i = 0; i < item_count; ++i) {
			if (!archive->read_object_begin(obj) || obj.class_name != "zCCSBlock") {
				throw ParserError {"CutsceneLibrary", "expected 'zCCSBlock' but didn't find it"};
			}

			auto& itm = msgs.blocks.emplace_back();
			itm.name = archive->read_string();      // blockName
			auto block_count = archive->read_int(); // numOfBlocks
			(void) archive->read_float();           // subBlock0

			if (block_count != 1) {
				throw ParserError {"CutsceneLibrary",
				                   "expected only one block but got " + std::to_string(block_count) + " for " +
				                       itm.name};
			}

			if (!archive->read_object_begin(obj) || obj.class_name != "zCCSAtomicBlock") {
				throw ParserError {"CutsceneLibrary", "expected atomic block not found for " + itm.name};
			}

			if (!archive->read_object_begin(obj) || obj.class_name != "oCMsgConversation:oCNpcMessage:zCEventMessage") {
				throw ParserError {"CutsceneLibrary", "expected oCMsgConversation not found for " + itm.name};
			}

			itm.message.type = archive->read_enum();
			itm.message.text = archive->read_string();
			itm.message.name = archive->read_string();

			if (!archive->read_object_end()) {
				archive->skip_object(true);
				PX_LOGW("CutsceneLibrary: oCMsgConversation(\"", itm.name, "\") not fully parsed");
			}

			if (!archive->read_object_end()) {
				// FIXME: in Gothic I cutscene libraries, there is a `synchronized` attribute here
				archive->skip_object(true);
				PX_LOGW("CutsceneLibrary: zCCSAtomicBlock(\"", itm.name, "\") not fully parsed");
			}

			if (!archive->read_object_end()) {
				archive->skip_object(true);
				PX_LOGW("CutsceneLibrary: zCCSBlock(\"", itm.name, "\") not fully parsed");
			}
		}

		if (!archive->read_object_end()) {
			PX_LOGW("CutsceneLibrary: not fully parsed");
		}

		// Prepare blocks for binary search in block_by_name
		std::sort(msgs.blocks.begin(), msgs.blocks.end(), [](const auto& a, const auto& b) { return a.name < b.name; });
		return msgs;
	}

	const CutsceneBlock* CutsceneLibrary::block_by_name(std::string_view name) const {
		auto result = std::lower_bound(this->blocks.begin(), this->blocks.end(), name, [](const auto& it, auto n) {
			return it.name < n;
		});

		if (result == this->blocks.end()) {
			return nullptr;
		}

		return &*result;
	}
} // namespace phoenix
