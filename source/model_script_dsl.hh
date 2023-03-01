// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "phoenix/buffer.hh"
#include "phoenix/model_script.hh"
#include "phoenix/phoenix.hh"

#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace phoenix::mds {
	MdsEventTag
	make_event_tag(int32_t, std::string&&, std::optional<std::string>&&, std::optional<std::string>&&, bool);
}

namespace phoenix {
	enum class MdsToken {
		keyword = 0,
		integer = 1,
		float_ = 2,
		string = 3,
		rbrace = 6,
		lbrace = 7,
		colon = 8,
		eof = 9,
		null = 10,
	};

	class MdsTokenizer {
	public:
		explicit MdsTokenizer(Buffer buf);

		MdsToken next();

		void backtrack() {
			this->_m_buffer.reset();
		}

		[[nodiscard]] std::string const& token_value() const {
			return _m_value;
		}

		[[nodiscard]] bool eof() const {
			return _m_buffer.remaining() == 0;
		}

		[[nodiscard]] std::string format_location() const;

	private:
		Buffer _m_buffer;
		uint32_t _m_line {1}, _m_column {1};
		std::string _m_value;
	};

	class MdsParser {
	public:
		explicit MdsParser(Buffer buf);

		ModelScript parse_script();
		MdsSkeleton parse_meshAndTree();
		std::string parse_registerMesh();
		void parse_aniEnum(ModelScript& into);
		void parse_events(MdsAnimation& ani);
		void ignore_block();
		MdsEventTag parse_eventTag();
		MdsSoundEffect parse_eventSFX();
		MdsParticleEffect parse_eventPFX();
		MdsSoundEffectGround parse_eventSFXGrnd();
		MdsParticleEffectStop parse_eventPFXStop();
		MdsMorphAnimation parse_eventMMStartAni();
		MdsCameraTremor parse_eventCamTremor();
		MdsAnimation parse_ani();
		MdsAnimationCombine parse_aniComb();
		MdsAnimationAlias parse_aniAlias();
		MdsAnimationBlend parse_aniBlend();
		std::string parse_aniDisable();
		MdsModelTag parse_modelTag();

	private:
		[[nodiscard]] bool eof() const {
			return this->_m_stream.eof();
		}

		template <MdsToken kind>
		void expect();

		[[nodiscard]] std::string expect_string();
		[[nodiscard]] std::string expect_keyword();
		void expect_keyword(std::string_view value);
		[[nodiscard]] float expect_number();
		[[nodiscard]] int expect_int();
		[[nodiscard]] AnimationFlags expect_flags();

		template <MdsToken kind>
		bool maybe();

		[[nodiscard]] std::optional<int> maybe_int();
		[[nodiscard]] std::optional<float> maybe_number();
		[[nodiscard]] std::optional<std::string> maybe_string();
		[[nodiscard]] bool maybe_keyword(std::string_view value);
		[[nodiscard]] std::optional<float> maybe_named(std::string_view name);

	private:
		MdsTokenizer _m_stream;
	};
} // namespace phoenix
