// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/vobs/mob.hh"
#include "phoenix/archive.hh"

namespace phoenix::vobs {
	void MovableObject::parse(MovableObject& obj, ArchiveReader& ctx, GameVersion version) {
		VirtualObject::parse(obj, ctx, version);
		obj.name = ctx.read_string();                                   // focusName
		obj.hp = ctx.read_int();                                        // hitpoints
		obj.damage = ctx.read_int();                                    // damage
		obj.movable = ctx.read_bool();                                  // moveable
		obj.takable = ctx.read_bool();                                  // takeable
		obj.focus_override = ctx.read_bool();                           // focusOverride
		obj.material = static_cast<SoundMaterialType>(ctx.read_enum()); // soundMaterial
		obj.visual_destroyed = ctx.read_string();                       // visualDestroyed
		obj.owner = ctx.read_string();                                  // owner
		obj.owner_guild = ctx.read_string();                            // ownerGuild
		obj.destroyed = ctx.read_bool();                                // isDestroyed
	}

	void InteractiveObject::parse(InteractiveObject& obj, ArchiveReader& ctx, GameVersion version) {
		MovableObject::parse(obj, ctx, version);
		obj.state = ctx.read_int();                       // stateNum
		obj.target = ctx.read_string();                   // triggerTarget
		obj.item = ctx.read_string();                     // useWithItem
		obj.condition_function = ctx.read_string();       // conditionFunc
		obj.on_state_change_function = ctx.read_string(); // onStateFunc
		obj.rewind = ctx.read_bool();                     // rewind
	}

	void Container::parse(Container& obj, ArchiveReader& ctx, GameVersion version) {
		InteractiveObject::parse(obj, ctx, version);
		obj.locked = ctx.read_bool();        // locked
		obj.key = ctx.read_string();         // keyInstance
		obj.pick_string = ctx.read_string(); // pickLockStr
		obj.contents = ctx.read_string();    // contains

		if (ctx.is_save_game()) {
			// In save-games, containers contain extra variables
			auto item_count = ctx.read_int(); // NumOfEntries
			obj.s_items.resize(item_count);

			ArchiveObject itm;
			for (auto i = 0; i < item_count; ++i) {
				if (!ctx.read_object_begin(itm) || itm.class_name != "oCItem:zCVob") {
					throw ParserError {"vobs::Container"};
				}

				obj.s_items[i] = std::make_unique<vobs::Item>();
				Item::parse(*obj.s_items[i], ctx, version);

				if (!ctx.read_object_end()) {
					PX_LOGW("vob_tree: oCItem:zCVob object not fully parsed");
					ctx.skip_object(true);
				}
			}
		}
	}

	void Door::parse(Door& obj, ArchiveReader& ctx, GameVersion version) {
		InteractiveObject::parse(obj, ctx, version);
		obj.locked = ctx.read_bool();        // locked
		obj.key = ctx.read_string();         // keyInstance
		obj.pick_string = ctx.read_string(); // pickLockStr
	}

	void Fire::parse(Fire& obj, ArchiveReader& ctx, GameVersion version) {
		InteractiveObject::parse(obj, ctx, version);
		obj.slot = ctx.read_string();     // fireSlot
		obj.vob_tree = ctx.read_string(); // fireVobtreeName
	}
} // namespace phoenix::vobs