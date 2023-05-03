// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/phoenix.hh"
#include "phoenix/buffer.hh"

#include <iostream>
#include <utility>

namespace phoenix {
	Error::Error(std::string&& msg) : std::exception(), message(std::move(msg)) {}

	ParserError::ParserError(std::string&& type)
	    : error("failed parsing resource of type " + type), resource_type(type) {}

	ParserError::ParserError(std::string&& type, std::string&& ctx)
	    : error("failed parsing resource of type " + type + " [context: " + ctx + "]"), resource_type(std::move(type)),
	      context(std::move(ctx)) {}

	ParserError::ParserError(std::string&& type, const std::exception& other_exc)
	    : error("failed parsing resource of type " + type + " due to [" + other_exc.what() + "]"),
	      resource_type(std::move(type)), cause(other_exc) {}

	ParserError::ParserError(std::string&& type, const std::exception& other_exc, std::string&& ctx)
	    : error("failed parsing resource of type " + type + " due to [" + other_exc.what() + "] [context: " + ctx +
	            "]"),
	      resource_type(std::move(type)), context(std::move(ctx)), cause(other_exc) {}

	std::optional<std::function<void(LogLevel, const std::string&)>> Logging::callback {};

	void Logging::use_logger(std::function<void(LogLevel, const std::string&)>&& cb) {
		Logging::callback = std::forward<decltype(cb)>(cb);
	}

	void Logging::use_default_logger() {
		Logging::callback = [](LogLevel lvl, const std::string& message) {
			switch (lvl) {
			case LogLevel::ERROR:
				std::cerr << "[phoenix] [error] " << message << "\n";
				break;
			case LogLevel::WARNING:
				std::cerr << "[phoenix] [warn ] " << message << "\n";
				break;
			case LogLevel::INFO:
				std::cerr << "[phoenix] [info ] " << message << "\n";
				break;
			case LogLevel::DEBUG:
				std::cerr << "[phoenix] [debug] " << message << "\n";
				break;
			}
		};
	}

	bool iequals(std::string_view a, std::string_view b) {
		return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char c1, char c2) {
			return std::tolower(c1) == std::tolower(c2);
		});
	}

	bool icompare(std::string_view a, std::string_view b) {
		return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), [](char c1, char c2) {
			return std::tolower(c1) < std::tolower(c2);
		});
	}

	Date Date::parse(Buffer& buf) {
		Date dt {};
		dt.year = buf.get_uint();
		dt.month = buf.get_ushort();
		dt.day = buf.get_ushort();
		dt.hour = buf.get_ushort();
		dt.minute = buf.get_ushort();
		dt.second = buf.get_ushort();
		(void) buf.get_ushort(); // padding
		return dt;
	}
} // namespace phoenix
