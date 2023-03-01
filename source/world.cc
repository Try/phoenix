// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/world.hh"
#include "phoenix/archive.hh"
#include "phoenix/phoenix.hh"

namespace phoenix {
	[[maybe_unused]] static constexpr uint32_t BSP_VERSION_G1 = 0x2090000;
	static constexpr uint32_t BSP_VERSION_G2 = 0x4090000;

	/// \brief Tries to determine the serialization version of a game world.
	///
	/// This function might be very slow. If the VOb tree or way-net or both come before the mesh section in the
	/// archive, they have to be skipped since only the `MeshAndBsp` section of the world can be used to reliably
	/// determine the version being used.
	///
	/// \param buf A buffer containing the world's data.
	/// \return The game version associated with that world.
	GameVersion determine_world_version(Buffer&& buf) {
		auto archive = ArchiveReader::open(buf);

		if (archive->is_save_game()) {
			throw phoenix::ParserError {"World", "cannot automatically detect world version for save-games!s"};
		}

		ArchiveObject chnk {};
		archive->read_object_begin(chnk);

		while (!archive->read_object_end()) {
			archive->read_object_begin(chnk);

			if (chnk.object_name == "MeshAndBsp") {
				auto bsp_version = buf.get_uint();

				if (bsp_version == BSP_VERSION_G2) {
					return GameVersion::GOTHIC_2;
				} else {
					return GameVersion::GOTHIC_1;
				}
			}

			archive->skip_object(true);
		}

		PX_LOGE("World: failed to determine world version. Assuming Gothic 1.");
		return GameVersion::GOTHIC_1;
	}

	World World::parse(Buffer& in, GameVersion version) {
		try {
			World wld;

			auto archive = ArchiveReader::open(in);

			ArchiveObject chnk {};
			archive->read_object_begin(chnk);

			if (chnk.class_name != "oCWorld:zCWorld") {
				throw ParserError {"World", "'oCWorld:zCWorld' chunk expected, got '" + chnk.class_name + "'"};
			}

			while (!archive->read_object_end()) {
				archive->read_object_begin(chnk);
				PX_LOGI("World: parsing object [",
				        chnk.object_name,
				        " ",
				        chnk.class_name,
				        " ",
				        chnk.version,
				        " ",
				        chnk.index,
				        "]");

				if (chnk.object_name == "MeshAndBsp") {
					auto bsp_version = in.get_uint();
					(void) /* size = */ in.get_uint();

					std::uint16_t chunk_type = 0;
					auto mesh_data = in.slice();

					do {
						chunk_type = in.get_ushort();
						in.skip(in.get_uint());
					} while (chunk_type != 0xB060);

					wld.world_bsp_tree = BspTree::parse(in, bsp_version);
					wld.world_mesh = Mesh::parse(mesh_data, wld.world_bsp_tree.leaf_polygons);
				} else if (chnk.object_name == "VobTree") {
					auto count = archive->read_int();
					wld.world_vobs.reserve(count);

					for (int32_t i = 0; i < count; ++i) {
						auto child = parse_vob_tree(*archive, version);
						if (child == nullptr)
							continue;
						wld.world_vobs.push_back(std::move(child));
					}
				} else if (chnk.object_name == "WayNet") {
					wld.world_way_net = WayNet::parse(*archive);
				} else if (chnk.object_name == "CutscenePlayer") {
					// TODO: only present in save-games

					if (!archive->read_object_begin(chnk)) {
						PX_LOGW("World: object [",
						        chnk.object_name,
						        " ",
						        chnk.class_name,
						        " ",
						        chnk.version,
						        " ",
						        chnk.index,
						        "] encountered but unable to parse");
						archive->skip_object(true);
						continue;
					}

					(void) archive->read_int(); // lastProcessDay
					(void) archive->read_int(); // lastProcessHour
					(void) archive->read_int(); // playListCount

					archive->read_object_end();
				} else if (chnk.object_name == "SkyCtrl") {
					// TODO: only present in save-games

					if (!archive->read_object_begin(chnk)) {
						PX_LOGW("World: object [",
						        chnk.object_name,
						        " ",
						        chnk.class_name,
						        " ",
						        chnk.version,
						        " ",
						        chnk.index,
						        "] encountered but unable to parse");
						archive->skip_object(true);
						continue;
					}

					(void) archive->read_float(); // masterTime
					(void) archive->read_float(); // rainWeight
					(void) archive->read_float(); // rainStart
					(void) archive->read_float(); // rainStop
					(void) archive->read_float(); // rainSctTimer
					(void) archive->read_float(); // rainSndVol
					(void) archive->read_float(); // dayCtr

					if (version == GameVersion::GOTHIC_2) {
						(void) archive->read_float(); // fadeScale
						(void) archive->read_bool();  // renderLightning
						(void) archive->read_bool();  // isRaining
						(void) archive->read_int();   // rainCtr
					}

					archive->read_object_end();
				} else if (chnk.object_name == "EndMarker" && archive->get_header().save) {
					// TODO: save games contain a list of NPCs after the end marker
					// First, Consume the end-maker fully
					archive->read_object_end();

					// Then, read all the NPCs
					auto npc_count = archive->read_int(); // npcCount
					for (auto i = 0; i < npc_count; ++i) {
						// FIXME: npc::parse(npcs[i], *archive, version)
						archive->skip_object(false);
					}

					// After that, read all NPC spawn locations
					auto npc_spawn_count = archive->read_int(); // NoOfEntries
					for (auto i = 0; i < npc_spawn_count; ++i) {
						archive->skip_object(false);  // npc zReference
						(void) archive->read_vec3();  // spawnPos
						(void) archive->read_float(); // timer
					}

					(void) archive->read_bool(); // spawningEnabled

					if (version == GameVersion::GOTHIC_2) {
						(void) archive->read_int(); // spawnFlags
					}

					if (!archive->read_object_end()) {
						PX_LOGW("World: Npc list not fully parsed");
						archive->skip_object(true);
					}

					// We have fully consumed the world block. From here we should just die.
					break;
				}

				if (!archive->read_object_end()) {
					PX_LOGW("World: object [",
					        chnk.object_name,
					        " ",
					        chnk.class_name,
					        " ",
					        chnk.version,
					        " ",
					        chnk.index,
					        "] not fully parsed");
					archive->skip_object(true);
				}
			}

			return wld;
		} catch (const BufferError& exc) {
			throw ParserError {"World", exc, "eof reached"};
		}
	}

	World World::parse(Buffer& buf) {
		auto version = determine_world_version(buf.duplicate());
		return World::parse(buf, version);
	}
} // namespace phoenix
