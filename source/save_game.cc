// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/save_game.hh"
#include "phoenix/archive.hh"

#include <optional>
#include <set>

namespace phoenix::unstable {
	SaveInfo SaveInfo::parse(Buffer&& buf) {
		SaveInfo info;
		auto archive = ArchiveReader::open(buf);

		ArchiveObject hdr;
		if (!archive->read_object_begin(hdr) || hdr.class_name != "oCSavegameInfo") {
			throw ParserError {"SaveInfo", "expected oCSavegameInfo object not found"};
		}

		info.title = archive->read_string();          // Title
		info.world = archive->read_string();          // WorldName
		info.time_day = archive->read_int();          // TimeDay
		info.time_hour = archive->read_int();         // TimeHour
		info.time_minute = archive->read_int();       // TimeMin
		info.save_date = archive->read_string();      // SaveDate
		info.version_major = archive->read_int();     // VersionMajor
		info.version_minor = archive->read_int();     // VersionMinor
		info.play_time_seconds = archive->read_int(); // PlayTimeSeconds

		if (!archive->read_object_end()) {
			// Gothic II stores more information.
			info.version_point = archive->read_int();       // VersionPoint
			info.version_int = archive->read_int();         // VersionInt
			info.version_app_name = archive->read_string(); // VersionAppName
		}

		if (!archive->read_object_end()) {
			PX_LOGW("SaveInfo: ", hdr.class_name, " not fully parsed");
		}

		return info;
	}

	SaveScriptState SaveScriptState::parse(Buffer&& buf, bool g2) {
		SaveScriptState sav;
		auto ar = ArchiveReader::open(buf);

		sav.day = ar->read_int();    // day
		sav.hour = ar->read_int();   // hour
		sav.minute = ar->read_int(); // min

		auto entry_count = ar->read_int(); // NumOfEntries
		sav.infos.resize(entry_count);

		for (auto i = 0; i < entry_count; ++i) {
			sav.infos[i].told = ar->read_bool();   // Told
			sav.infos[i].name = ar->read_string(); // InstName
		}

		ar->read_int();                    // NumOfEntries
		auto topic_count = ar->read_int(); // LOGMANAGERTOPICCOUNT
		sav.log.resize(topic_count);

		for (auto i = 0; i < topic_count; ++i) {
			auto& topic = sav.log[i];
			topic.description = ar->read_string();                          // TOPICDESCRIPTION
			topic.section = static_cast<SaveTopicSection>(ar->read_enum()); // TOPICSECTION
			topic.status = static_cast<SaveTopicStatus>(ar->read_enum());   // TOPICSTATUS
			topic.entries.resize(ar->read_int());                           // LOGTOPICENTRYCOUNT

			(void) ar->read_int(); // LOGMANAGERENTRYCOUNT

			for (auto j = 0u; j < topic.entries.size(); ++j) {
				topic.entries[j] = ar->read_string(); // ENTRYDESCRIPTION
			}
		}

		ArchiveObject obj;
		if (!ar->read_object_begin(obj) || obj.class_name != "oCCSManager:zCCSManager") {
			throw ParserError {"SaveInfo", "expected oCCSManager:zCCSManager object not found"};
		}

		ar->read_int(); // poolCount

		if (!ar->read_object_end()) {
			PX_LOGE("SaveGame: ", obj.class_name, " not fully parsed");
			ar->skip_object(true);
		}

		auto symbol_count = ar->read_int(); // numSymbols
		sav.symbols.resize(symbol_count);

		for (auto i = 0; i < symbol_count; ++i) {
			SaveSymbolState sym;
			sym.name = ar->read_string(); // symName0

			// For Gothic II saves, there is additional data stored
			if (g2) {
				auto value_count = ar->read_int(); // symName0cnt

				for (auto j = 0; j < value_count; ++j) {
					sym.values.push_back(ar->read_int()); // symValue0_0
				}
			} else {
				sym.values.push_back(ar->read_int()); // symValue0
			}
		}

		// A two-dimensional array of the form int[42][42] containing guild attitudes
		auto raw = ar->read_raw_bytes(42 * 42); // guildTable

		for (int i = 0; i < 42; ++i) {
			for (int j = 0; j < 42; ++j) {
				sav.guild_attitudes[i][j] = raw.get();
			}
		}

		if (buf.remaining() > 0) {
			PX_LOGE("SaveGame: not fully parsed");
		}

		return sav;
	}

	std::optional<std::filesystem::path> find_file_matching(const std::set<std::filesystem::path>& choices,
	                                                        std::string_view filename) {
		auto result = std::find_if(choices.begin(), choices.end(), [filename](const std::filesystem::path& path) {
			return phoenix::iequals(path.filename().string(), filename);
		});

		if (result == choices.end()) {
			return std::nullopt;
		}

		return *result;
	}

	SaveGame SaveGame::parse(const std::filesystem::path& path) {
		SaveGame sav;
		sav._m_root_path = path;

		if (!std::filesystem::is_directory(path)) {
			throw ParserError {"SaveGame", "save game path does not exist or is not a directory"};
		}

		std::set<std::filesystem::path> entries {};
		for (auto& file : std::filesystem::directory_iterator(path)) {
			entries.emplace(file.path());
		}

		// Load SAVEINFO.SAV
		{
			PX_LOGI("SaveGame: loading SAVEINFO.SAV");
			auto file_save_info = find_file_matching(entries, "SAVEINFO.SAV");
			if (!file_save_info) {
				throw ParserError {"SaveGame",
				                   "expected SAVEINFO.SAV not found. this is probably not a Gothic savegame"};
			}

			sav.metadata = SaveInfo::parse(Buffer::mmap(*file_save_info));
			sav.current_world = sav.metadata.world + ".ZEN";
		}

		// Load THUMB.SAV
		{
			PX_LOGI("SaveGame: loading THUMB.SAV");
			auto file_thumb = find_file_matching(entries, "THUMB.SAV");
			if (file_thumb) {
				sav.thumbnail = Texture::parse(Buffer::mmap(*file_thumb));
			}
		}

		// Load SAVEDAT.SAV
		{
			PX_LOGI("SaveGame: loading SAVEDAT.SAV");
			auto file_save_dat = find_file_matching(entries, "SAVEDAT.SAV");
			if (!file_save_dat) {
				throw ParserError {"SaveGame",
				                   "expected SAVEDAT.SAV not found. this is probably not a Gothic savegame"};
			}

			sav.script = SaveScriptState::parse(Buffer::mmap(*file_save_dat), !sav.metadata.version_app_name.empty());
		}

		return sav;
	}

	std::optional<Buffer> SaveGame::open_world_save(std::string_view world_name) const {
		auto path = _m_root_path / world_name;
		path.replace_extension("SAV");

		if (!std::filesystem::exists(path))
			return std::nullopt;
		return Buffer::mmap(path);
	}
} // namespace phoenix::unstable
