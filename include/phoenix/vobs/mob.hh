// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include "misc.hh"
#include "vob.hh"

namespace phoenix {
	/// \brief The different sounds a material can make.
	enum class SoundMaterialType : std::uint32_t {
		WOOD = 0,
		STONE = 1,
		METAL = 2,
		LEATHER = 3,
		CLAY = 4,
		GLASS = 5,

		// Deprecated entries.
		wood PHOENIX_DEPRECATED("renamed to SoundMaterialType::WOOD") = WOOD,
		stone PHOENIX_DEPRECATED("renamed to SoundMaterialType::STONE") = STONE,
		metal PHOENIX_DEPRECATED("renamed to SoundMaterialType::METAL") = METAL,
		leather PHOENIX_DEPRECATED("renamed to SoundMaterialType::LEATHER") = LEATHER,
		clay PHOENIX_DEPRECATED("renamed to SoundMaterialType::CLAY") = CLAY,
		glass PHOENIX_DEPRECATED("renamed to SoundMaterialType::GLASS") = GLASS,
	};

	namespace vobs {
		struct MovableObject : public VirtualObject {
			std::string name;
			std::int32_t hp;
			std::int32_t damage;
			bool movable;
			bool takable;
			bool focus_override;
			SoundMaterialType material;
			std::string visual_destroyed;
			std::string owner;
			std::string owner_guild;
			bool destroyed;

			/// \brief Parses an interactive VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(MovableObject& obj, ArchiveReader& ctx, GameVersion version);
		};

		struct InteractiveObject : public MovableObject {
			std::int32_t state;
			std::string target;
			std::string item;
			std::string condition_function;
			std::string on_state_change_function;
			bool rewind;

			/// \brief Parses a interactive VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(InteractiveObject& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb representing a campfire.
		struct Fire : public InteractiveObject {
			std::string slot;
			std::string vob_tree;

			/// \brief Parses a campfire VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see mob::parse
			PHOENIX_API static void parse(Fire& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb representing a container.
		struct Container : public InteractiveObject {
			bool locked;
			std::string key;
			std::string pick_string;
			std::string contents;

			// Save-game only variables
			std::vector<std::unique_ptr<Item>> s_items;

			/// \brief Parses a container VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see mob::parse
			/// \see mob_container::parse
			PHOENIX_API static void parse(Container& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb representing a door.
		struct Door : public InteractiveObject {
			bool locked;
			std::string key;
			std::string pick_string;

			/// \brief Parses a door VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see mob::parse
			/// \see mob_container::parse
			PHOENIX_API static void parse(Door& obj, ArchiveReader& ctx, GameVersion version);
		};

		using mob PHOENIX_DEPRECATED("renamed to MovableObject") = MovableObject;
		using mob_inter PHOENIX_DEPRECATED("renamed to InteractiveObject") = InteractiveObject;
		using mob_fire PHOENIX_DEPRECATED("renamed to Fire") = Fire;
		using mob_container PHOENIX_DEPRECATED("renamed to Container") = Container;
		using mob_door PHOENIX_DEPRECATED("renamed to Door") = Door;
	} // namespace vobs

	using sound_material PHOENIX_DEPRECATED("renamed to SoundMaterialType") = SoundMaterialType;
} // namespace phoenix
