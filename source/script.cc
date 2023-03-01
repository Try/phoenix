// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/script.hh"

#include <string>

namespace phoenix {
	DaedalusSymbolNotFound::DaedalusSymbolNotFound(std::string&& sym_name)
	    : DaedalusScriptError("symbol not found: " + sym_name), name(sym_name) {}

	DaedalusMemberRegistrationError::DaedalusMemberRegistrationError(const DaedalusSymbol* s, std::string&& msg)
	    : DaedalusScriptError("cannot register member " + s->name() + ": " + msg), sym(s) {}

	DaedalusInvalidRegistrationDataType::DaedalusInvalidRegistrationDataType(const DaedalusSymbol* s,
	                                                                         std::string&& provided)
	    : DaedalusMemberRegistrationError(s,
	                                      "wrong datatype: provided '" + provided + "' expected " +
	                                          DAEDALUS_DATA_TYPE_NAMES[(std::uint32_t) s->type()]) {}

	DaedalusIllegalTypeAccess::DaedalusIllegalTypeAccess(const DaedalusSymbol* s, DaedalusDataType expected_dt)
	    : DaedalusIllegalAccess("illegal access of type " + std::to_string(static_cast<int32_t>(expected_dt)) +
	                            " on DaedalusSymbol " + s->name() + " which is another type (" +
	                            std::to_string(static_cast<int32_t>(s->type())) + ")"),
	      sym(s), expected(expected_dt) {}

	DaedalusIllegalIndexAccess::DaedalusIllegalIndexAccess(const DaedalusSymbol* s, std::uint8_t idx)
	    : DaedalusIllegalAccess("illegal access of out-of-bounds index " + std::to_string(idx) + " while reading " +
	                            s->name()),
	      sym(s), index(idx) {}

	DaedalusIllegalConstAccess::DaedalusIllegalConstAccess(const DaedalusSymbol* s)
	    : DaedalusIllegalAccess("illegal mutable access of const symbol " + s->name()), sym(s) {}

	DaedalusIllegalInstanceAccess::DaedalusIllegalInstanceAccess(const DaedalusSymbol* s, std::uint32_t parent)
	    : DaedalusIllegalAccess("illegal access of member " + s->name() +
	                            " which does not have the same parent "
	                            "class as the context instance (" +
	                            std::to_string(s->parent()) + " != " + std::to_string(parent) + ")"),
	      sym(s), expected_parent(parent) {}

	DaedalusUnboundMemberAccess::DaedalusUnboundMemberAccess(const DaedalusSymbol* s)
	    : DaedalusIllegalAccess("illegal access of unbound member " + s->name()), sym(s) {}

	DaedalusNoContextError::DaedalusNoContextError(const DaedalusSymbol* s)
	    : DaedalusIllegalAccess("illegal access of member " + s->name() + " without a context set."), sym(s) {}

	DaedalusIllegalContextType::DaedalusIllegalContextType(const DaedalusSymbol* s, const std::type_info& ctx)
	    : DaedalusIllegalAccess("cannot access member " + s->name() + " on context instance of type " + ctx.name() +
	                            " because this symbol is registered to instances of type " + s->registered_to().name()),
	      sym(s), context_type(ctx) {}

	DaedalusInstruction DaedalusInstruction::decode(Buffer& in) {
		DaedalusInstruction s {};
		s.op = static_cast<DaedalusOpcode>(in.get());
		s.size = 1;

		switch (s.op) {
		case DaedalusOpcode::BL:
		case DaedalusOpcode::BZ:
		case DaedalusOpcode::B:
			s.address = in.get_uint();
			s.size += sizeof(std::uint32_t);
			break;
		case DaedalusOpcode::PUSHI:
			s.immediate = in.get_int();
			s.size += sizeof(std::uint32_t);
			break;
		case DaedalusOpcode::BE:
		case DaedalusOpcode::PUSHV:
		case DaedalusOpcode::PUSHVI:
		case DaedalusOpcode::GMOVI:
			s.symbol = in.get_uint();
			s.size += sizeof(std::uint32_t);
			break;
		case DaedalusOpcode::PUSHVV:
			s.symbol = in.get_uint();
			s.index = in.get();
			s.size += sizeof(std::uint32_t) + sizeof(std::uint8_t);
			break;
		default:
			break;
		}

		return s;
	}

	DaedalusScript DaedalusScript::parse(const std::string& file) {
		auto in = Buffer::mmap(file);
		return parse(in);
	}

	DaedalusScript DaedalusScript::parse(phoenix::Buffer& in) {
		DaedalusScript scr {};

		scr._m_version = in.get();
		auto symbol_count = in.get_uint();

		scr._m_symbols.reserve(symbol_count + 1 /* account for temporary strings */);
		scr._m_symbols_by_name.reserve(symbol_count + 1);
		scr._m_symbols_by_address.reserve(symbol_count);
		in.skip(symbol_count * sizeof(std::uint32_t)); // Sort table
		// The sort table is a list of indexes into the symbol table sorted lexicographically by symbol name!

		for (std::uint32_t i = 0; i < symbol_count; ++i) {
			auto* sym = &scr._m_symbols.emplace_back(DaedalusSymbol::parse(in));
			scr._m_symbols_by_name[sym->name()] = i;
			sym->_m_index = i;

			if (sym->type() == DaedalusDataType::PROTOTYPE || sym->type() == DaedalusDataType::INSTANCE ||
			    (sym->type() == DaedalusDataType::FUNCTION && sym->is_const() && !sym->is_member())) {
				scr._m_symbols_by_address[sym->address()] = i;
			}
		}

		std::uint32_t text_size = in.get_uint();
		scr._m_text = in.extract(text_size);
		return scr;
	}

	DaedalusInstruction DaedalusScript::instruction_at(std::uint32_t address) const {
		_m_text.position(address);
		return DaedalusInstruction::decode(_m_text);
	}

	const DaedalusSymbol* DaedalusScript::find_symbol_by_index(std::uint32_t index) const {
		if (index > _m_symbols.size()) {
			return nullptr;
		}
		return &_m_symbols[index];
	}

	const DaedalusSymbol* DaedalusScript::find_symbol_by_name(std::string_view name) const {
		std::string up {name};
		std::transform(up.begin(), up.end(), up.begin(), ::toupper);

		if (auto it = _m_symbols_by_name.find(up); it != _m_symbols_by_name.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	const DaedalusSymbol* DaedalusScript::find_symbol_by_address(std::uint32_t address) const {
		if (auto it = _m_symbols_by_address.find(address); it != _m_symbols_by_address.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	DaedalusSymbol* DaedalusScript::find_symbol_by_index(std::uint32_t index) {
		if (index > _m_symbols.size()) {
			return nullptr;
		}
		return &_m_symbols[index];
	}

	DaedalusSymbol* DaedalusScript::find_symbol_by_name(std::string_view name) {
		std::string up {name};
		std::transform(up.begin(), up.end(), up.begin(), ::toupper);

		if (auto it = _m_symbols_by_name.find(up); it != _m_symbols_by_name.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	DaedalusSymbol* DaedalusScript::find_symbol_by_address(std::uint32_t address) {
		if (auto it = _m_symbols_by_address.find(address); it != _m_symbols_by_address.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	void DaedalusScript::enumerate_instances_by_class_name(std::string_view name,
	                                                       const std::function<void(DaedalusSymbol&)>& callback) {
		auto* cls = find_symbol_by_name(name);
		if (cls == nullptr)
			return;

		std::vector<uint32_t> prototypes {};
		for (auto& sym : _m_symbols) {
			if (sym.type() == DaedalusDataType::PROTOTYPE && sym.parent() == cls->index()) {
				prototypes.push_back(sym.index());
			} else if (sym.type() == DaedalusDataType::INSTANCE &&
			           (std::find(prototypes.begin(), prototypes.end(), sym.parent()) != prototypes.end() ||
			            sym.parent() == cls->index())) {
				callback(sym);
			}
		}
	}

	std::vector<DaedalusSymbol*> DaedalusScript::find_parameters_for_function(const DaedalusSymbol* parent) {
		std::vector<DaedalusSymbol*> syms {};

		for (uint32_t i = 0; i < parent->count(); ++i) {
			syms.push_back(find_symbol_by_index(parent->index() + i + 1));
		}

		return syms;
	}

	std::vector<const DaedalusSymbol*>
	DaedalusScript::find_parameters_for_function(const DaedalusSymbol* parent) const {
		std::vector<const DaedalusSymbol*> syms {};

		for (uint32_t i = 0; i < parent->count(); ++i) {
			syms.push_back(find_symbol_by_index(parent->index() + i + 1));
		}

		return syms;
	}

	DaedalusSymbol* DaedalusScript::add_temporary_strings_symbol() {
		DaedalusSymbol sym {};
		sym._m_name = "$PHOENIX_FAKE_STRINGS";
		sym._m_generated = true;
		sym._m_type = DaedalusDataType::STRING;
		sym._m_count = 1;
		sym._m_value = std::unique_ptr<std::string[]> {new std::string[sym._m_count]};
		sym._m_index = static_cast<std::uint32_t>(_m_symbols.size());

		return &_m_symbols.emplace_back(std::move(sym));
	}

	DaedalusSymbol DaedalusSymbol::parse(Buffer& in) {
		DaedalusSymbol sym {};

		if (in.get_uint() != 0) {
			sym._m_name = in.get_line(false);

			// If the name starts with \xFF, this DaedalusSymbol was automatically generated by the compiler
			if (sym._m_name[0] == '\xFF') {
				sym._m_name[0] = '$';
				sym._m_generated = true;
			}
		}

		auto vary = in.get_uint();
		auto properties = in.get_uint();

		sym._m_count = (properties >> 0U) & 0xFFFU;                              // 12 bits
		sym._m_type = static_cast<DaedalusDataType>((properties >> 12U) & 0xFU); // 4 bits
		sym._m_flags = static_cast<std::uint32_t>((properties >> 16U) & 0x3FU);  // 6 bits

		if (sym.is_member()) {
			sym._m_member_offset = vary;
		} else if (sym.type() == DaedalusDataType::CLASS) {
			sym._m_class_size = vary;
		} else if (sym.type() == DaedalusDataType::FUNCTION) {
			sym._m_return_type = static_cast<DaedalusDataType>(vary);
		}

		sym._m_file_index = in.get_uint() & 0x7FFFFU;  // 19 bits
		sym._m_line_start = in.get_uint() & 0x7FFFFU;  // 19 bits
		sym._m_line_count = in.get_uint() & 0x7FFFFU;  // 19 bits
		sym._m_char_start = in.get_uint() & 0xFFFFFFU; // 24 bits
		sym._m_char_count = in.get_uint() & 0xFFFFFFU; // 24 bits

		if (!sym.is_member()) {
			switch (sym._m_type) {
			case DaedalusDataType::FLOAT: {
				std::unique_ptr<float[]> value {new float[sym._m_count]};
				in.get((std::uint8_t*) value.get(), sym._m_count * sizeof(float));
				sym._m_value = std::move(value);
				break;
			}
			case DaedalusDataType::INT: {
				std::unique_ptr<std::int32_t[]> value {new std::int32_t[sym._m_count]};
				in.get((std::uint8_t*) value.get(), sym._m_count * sizeof(std::uint32_t));
				sym._m_value = std::move(value);
				break;
			}
			case DaedalusDataType::STRING: {
				std::unique_ptr<std::string[]> value {new std::string[sym._m_count]};
				for (std::uint32_t i = 0; i < sym._m_count; ++i) {
					value[i] = in.get_line_escaped(false);
				}
				sym._m_value = std::move(value);
				break;
			}
			case DaedalusDataType::CLASS:
				sym._m_class_offset = in.get_int();
				break;
			case DaedalusDataType::INSTANCE:
				sym._m_value = std::shared_ptr<DaedalusInstance> {nullptr};
				sym._m_address = in.get_int();
				break;
			case DaedalusDataType::FUNCTION:
				if (!sym.is_const()) {
					sym._m_value = std::unique_ptr<std::int32_t[]>(new int32_t[1]);
				}
				sym._m_address = in.get_int();
				break;
			case DaedalusDataType::PROTOTYPE:
				sym._m_address = in.get_int();
				break;
			default:
				break;
			}
		}

		sym._m_parent = in.get_int();
		return sym;
	}

	const std::string& DaedalusSymbol::get_string(std::size_t index,
	                                              const std::shared_ptr<DaedalusInstance>& context) const {
		if (type() != DaedalusDataType::STRING) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::STRING);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}
			return *get_member_ptr<std::string>(index, context);
		} else {
			return std::get<std::unique_ptr<std::string[]>>(_m_value)[index];
		}
	}

	float DaedalusSymbol::get_float(std::size_t index, const std::shared_ptr<DaedalusInstance>& context) const {
		if (type() != DaedalusDataType::FLOAT) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::FLOAT);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}
			return *get_member_ptr<float>(index, context);
		} else {
			return std::get<std::unique_ptr<float[]>>(_m_value)[index];
		}
	}

	std::int32_t DaedalusSymbol::get_int(std::size_t index, const std::shared_ptr<DaedalusInstance>& context) const {
		if (type() != DaedalusDataType::INT && type() != DaedalusDataType::FUNCTION) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::INT);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}
			return *get_member_ptr<std::int32_t>(index, context);
		} else {
			return std::get<std::unique_ptr<std::int32_t[]>>(_m_value)[index];
		}
	}

	void DaedalusSymbol::set_string(std::string_view value,
	                                std::size_t index,
	                                const std::shared_ptr<DaedalusInstance>& context) {
		if (type() != DaedalusDataType::STRING) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::STRING);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}
			*get_member_ptr<std::string>(index, context) = value;
		} else {
			std::get<std::unique_ptr<std::string[]>>(_m_value).get()[index] = value;
		}
	}

	void DaedalusSymbol::set_float(float value, std::size_t index, const std::shared_ptr<DaedalusInstance>& context) {
		if (type() != DaedalusDataType::FLOAT) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::FLOAT);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}
			*get_member_ptr<float>(index, context) = value;
		} else {
			std::get<std::unique_ptr<float[]>>(_m_value)[index] = value;
		}
	}

	void
	DaedalusSymbol::set_int(std::int32_t value, std::size_t index, const std::shared_ptr<DaedalusInstance>& context) {
		if (type() != DaedalusDataType::INT && type() != DaedalusDataType::FUNCTION) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::INT);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}
			*get_member_ptr<std::int32_t>(index, context) = value;
		} else {
			std::get<std::unique_ptr<std::int32_t[]>>(_m_value)[index] = value;
		}
	}

	const std::shared_ptr<DaedalusInstance>& DaedalusSymbol::get_instance() {
		if (type() != DaedalusDataType::INSTANCE) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::INSTANCE);
		}

		return std::get<std::shared_ptr<DaedalusInstance>>(_m_value);
	}

	void DaedalusSymbol::set_instance(const std::shared_ptr<DaedalusInstance>& inst) {
		if (type() != DaedalusDataType::INSTANCE) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::INSTANCE);
		}

		std::get<std::shared_ptr<DaedalusInstance>>(_m_value) = inst;
	}
} // namespace phoenix
