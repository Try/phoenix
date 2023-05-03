// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"

#include <functional>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#if PHOENIX_LOG_LEVEL > 0
	#define PX_LOGE(...) phoenix::Logging::log(phoenix::LogLevel::ERROR, __VA_ARGS__)
#else
	#define PX_LOGE(...)
#endif

#if PHOENIX_LOG_LEVEL > 1
	#define PX_LOGW(...) phoenix::Logging::log(phoenix::LogLevel::WARNING, __VA_ARGS__)
#else
	#define PX_LOGW(...)
#endif

#if PHOENIX_LOG_LEVEL > 2
	#define PX_LOGI(...) phoenix::Logging::log(phoenix::LogLevel::INFO, __VA_ARGS__)
#else
	#define PX_LOGI(...)
#endif

#if PHOENIX_LOG_LEVEL > 3
	#define PX_LOGD(...) phoenix::logging::log(phoenix::LogLevel::DEBUG, __VA_ARGS__)
#else
	#define PX_LOGD(...)
#endif

namespace phoenix {
	class Buffer;
	class ArchiveReader;

	/// \brief An enum for providing a game version hint to some functions
	enum class GameVersion {
		GOTHIC_1 = 0, ///< Represents any patch of Gothic
		GOTHIC_2 = 1, ///< Represents any patch of Gothic II, including _Night of the Raven_.

		// Deprecated entries.
		gothic_1 PHOENIX_DEPRECATED("renamed to GameVersion::GOTHIC_1") = GOTHIC_1,
		gothic_2 PHOENIX_DEPRECATED("renamed to GameVersion::GOTHIC_2") = GOTHIC_2,
	};

	/// \brief Tests whether two strings are equal when ignoring case.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if both strings are equal when ignoring case.
	PHOENIX_API bool iequals(std::string_view a, std::string_view b);

	/// \brief Tests whether \p a is lexicographically less than \p b.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if \p a is lexicographically less than \p b.
	PHOENIX_API bool icompare(std::string_view a, std::string_view b);

	/// \brief A basic datetime structure used by the *ZenGin*.
	struct Date {
		/// \brief Parses a date from a buffer.
		/// \param buf The buffer to read from
		/// \return The date.
		PHOENIX_API static Date parse(Buffer& buf);

		std::uint32_t year;
		std::uint16_t month;
		std::uint16_t day;
		std::uint16_t hour;
		std::uint16_t minute;
		std::uint16_t second;
	};

	enum class LogLevel {
		ERROR = 0,
		WARNING = 1,
		INFO = 2,
		DEBUG = 3,

		// Deprecated entries.
		error PHOENIX_DEPRECATED("renamed to LogLevel::ERROR") = ERROR,
		warn PHOENIX_DEPRECATED("renamed to LogLevel::WARNING") = WARNING,
		info PHOENIX_DEPRECATED("renamed to LogLevel::INFO") = INFO,
		debug PHOENIX_DEPRECATED("renamed to LogLevel::DEBUG") = DEBUG,
	};

	/// \brief Logging manager for phoenix.
	class Logging {
	public:
		using level PHOENIX_DEPRECATED("renamed to LogLevel") = LogLevel;

		/// \brief Supply a custom logger callback to be used for log output from phoenix.
		/// \param callback The callback to use.
		PHOENIX_API static void use_logger(std::function<void(LogLevel, const std::string&)>&& callback);

		/// \brief Use the default logger callback for phoenix.
		PHOENIX_API static void use_default_logger();

		/// \brief Send a logging event to the underlying log callback.
		/// \param lvl The level of the log message.
		/// \param message The message to send.
		template <typename... T>
		static void log(LogLevel lvl, const T&... msg) {
			if (Logging::callback) {
				std::stringstream msg_buffer {};
				Logging::_build_log_message(msg_buffer, msg...);
				(*Logging::callback)(lvl, msg_buffer.str());
			}
		}

	private:
		template <typename T, typename... Ts>
		static void _build_log_message(std::stringstream& stream, const T& v, const Ts&... vals) {
			stream << v;

			if constexpr (sizeof...(vals) > 0) {
				_build_log_message(stream, vals...);
			}
		}

		static std::optional<std::function<void(LogLevel, const std::string&)>> callback;
	};

	/// \brief Base class for all exceptions.
	class Error : public std::exception {
	public:
		PHOENIX_API explicit Error(std::string&& message);

		[[nodiscard]] PHOENIX_API inline const char* what() const noexcept override {
			return message.c_str();
		}

	public:
		const std::string message;
	};

	/// \brief An error representing a parsing failure of any kind.
	class ParserError : public Error {
	public:
		PHOENIX_INTERNAL explicit ParserError(std::string&& resource_type);
		PHOENIX_API explicit ParserError(std::string&& resource_type, std::string&& context);
		PHOENIX_INTERNAL explicit ParserError(std::string&& resource_type, const std::exception& cause);
		PHOENIX_INTERNAL explicit ParserError(std::string&& resource_type,
		                                      const std::exception& cause,
		                                      std::string&& context);

	public:
		const std::string resource_type;
		const std::optional<std::string> context {std::nullopt};
		const std::optional<std::exception> cause {std::nullopt};
	};

	template <typename T>
	T parse([[maybe_unused]] Buffer& buf) {
		throw ParserError {"unknown", "parsing routine not implemented"};
	}

	template <typename T>
	inline T parse(Buffer&& buf) {
		return parse<T>(buf);
	}

	template <typename T>
	T parse([[maybe_unused]] ArchiveReader& buf) {
		throw ParserError {"unknown", "parsing routine not implemented"};
	}

	using date PHOENIX_DEPRECATED("renamed to Date") = Date;
	using game_version PHOENIX_DEPRECATED("renamed to GameVersion") = GameVersion;
	using logging PHOENIX_DEPRECATED("renamed to Logging") = Logging;
	using error PHOENIX_DEPRECATED("renamed to Error") = Error;
	using parser_error PHOENIX_DEPRECATED("renamed to ParserError") = ParserError;
} // namespace phoenix
