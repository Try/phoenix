// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include "vob.hh"

namespace phoenix {
	/// \brief Describes how a sound should be played when the player enters its trigger volume.
	enum class SoundMode : uint32_t {
		LOOP = 0,   ///< The sound should be player forever until the player exits the trigger volume.
		ONCE = 1,   ///< The sound should be played once when the player enters the trigger volume.
		RANDOM = 2, ///< While the player is in the trigger volume, the should should play randomly.

		// Deprecated entries.
		loop PHOENIX_DEPRECATED("renamed to SoundMode::LOOP") = LOOP,
		once PHOENIX_DEPRECATED("renamed to SoundMode::ONCE") = ONCE,
		random PHOENIX_DEPRECATED("renamed to SoundMode::RANDOM") = RANDOM,
	};

	/// \brief The trigger volume type for sounds.
	enum class SoundTriggerVolumeType : uint32_t {
		/// \brief The sound is triggered when the player enters a spherical area around the VOb
		///        indicated by its radius setting.
		SPHERICAL = 0,

		/// \brief The sound is triggered when the player enters a ellipsoidal area around the VOb
		///        indicated by its radius setting.
		ELLIPSOIDAL = 1,

		// Deprecated entries.
		spherical PHOENIX_DEPRECATED("renamed to SoundTriggerVolume::SPHERICAL") = SPHERICAL,
		ellipsoidal PHOENIX_DEPRECATED("renamed to SoundTriggerVolume::ELLIPSOIDAL") = ELLIPSOIDAL,
	};

	namespace vobs {
		/// \brief A VOb which emits a sound.
		struct Sound : public VirtualObject {
			float volume {0};
			SoundMode mode {SoundMode::ONCE};
			float random_delay {0};
			float random_delay_var {0};
			bool initially_playing {false};
			bool ambient3d {false};
			bool obstruction {true};
			float cone_angle {0};
			SoundTriggerVolumeType volume_type {SoundTriggerVolumeType::SPHERICAL};
			float radius {0};
			std::string sound_name {};

			// Save-game only variables
			bool s_is_running;
			bool s_is_allowed_to_run;

			/// \brief Parses a sound VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(Sound& obj, ArchiveReader& ctx, GameVersion version);
		};

		/// \brief A VOb which emits a sound only during certain times of the day.
		struct SoundDaytime : public Sound {
			float start_time {0};
			float end_time {0};
			std::string sound_name2 {};

			/// \brief Parses a timed sound VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			PHOENIX_API static void parse(SoundDaytime& obj, ArchiveReader& ctx, GameVersion version);
		};

		using sound PHOENIX_DEPRECATED("renamed to Sound") = Sound;
		using sound_daytime PHOENIX_DEPRECATED("renamed to SoundDaytime") = SoundDaytime;
	} // namespace vobs

	using sound_mode PHOENIX_DEPRECATED("renamed to SoundMode") = SoundMode;
	using sound_trigger_volume PHOENIX_DEPRECATED("renamed to SoundTriggerVolumeType") = SoundTriggerVolumeType;
} // namespace phoenix