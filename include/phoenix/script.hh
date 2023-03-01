// Copyright © 2023 GothicKit Contributors, Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "buffer.hh"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#define unset 0xFF'FF'FF'FFU

namespace phoenix {
	/// \brief Daedalus data types.
	enum class DaedalusDataType : std::uint32_t {
		VOID = 0U,      ///< A datatype similar to C++'s `void`.
		FLOAT = 1U,     ///< A 32-bit floating point type similar to C++'s `float`.
		INT = 2U,       ///< A 32-bit signed integer type like std::int32_t
		STRING = 3U,    ///< A [Windows-1252](https://en.wikipedia.org/wiki/Windows-1252) encoded character array.
		CLASS = 4U,     ///< A structure definition.
		FUNCTION = 5U,  ///< A function definition or a function pointer represented as a 32-bit signed integer.
		PROTOTYPE = 6U, ///< A prototype definition.
		INSTANCE = 7U,  ///< An instance definition or instance reference represented as a 32-bit unsigned integer.

		// Deprecated entries.
		void_ PHOENIX_DEPRECATED("renamed to DaedalusDataType::VOID") = VOID,
		float_ PHOENIX_DEPRECATED("renamed to DaedalusDataType::FLOAT") = FLOAT,
		integer PHOENIX_DEPRECATED("renamed to DaedalusDataType::INT") = INT,
		string PHOENIX_DEPRECATED("renamed to DaedalusDataType::STRING") = STRING,
		class_ PHOENIX_DEPRECATED("renamed to DaedalusDataType::CLASS") = CLASS,
		function PHOENIX_DEPRECATED("renamed to DaedalusDataType::FUNCTION") = FUNCTION,
		prototype PHOENIX_DEPRECATED("renamed to DaedalusDataType::PROTOTYPE") = PROTOTYPE,
		instance PHOENIX_DEPRECATED("renamed to DaedalusDataType::INSTANCE") = INSTANCE,
	};

	constexpr const char* const DAEDALUS_DATA_TYPE_NAMES[] = {
	    "void",
	    "float",
	    "int",
	    "string",
	    "class",
	    "function",
	    "prototype",
	    "instance",
	};

	/// \brief Flags of symbols.
	namespace DaedalusSymbolFlag {
		static constexpr auto CONST = 1U << 0U;    ///< The symbol is not mutable.
		static constexpr auto RETURN = 1U << 1U;   ///< The symbol is a function and has a return value.
		static constexpr auto MEMBER = 1U << 2U;   ///< The symbol is a class member.
		static constexpr auto EXTERNAL = 1U << 3U; ///< The symbol refers to an external function.
		static constexpr auto MERGED = 1U << 4U;   ///< Unused.

		// Deprecated entries.
		PHOENIX_DEPRECATED("renamed to DaedalusSymbolFlag::CONST") static constexpr auto const_ = CONST;
		PHOENIX_DEPRECATED("renamed to DaedalusSymbolFlag::RETURN") static constexpr auto return_ = RETURN;
		PHOENIX_DEPRECATED("renamed to DaedalusSymbolFlag::MEMBER") static constexpr auto member = MEMBER;
		PHOENIX_DEPRECATED("renamed to DaedalusSymbolFlag::EXTERNAL") static constexpr auto external = EXTERNAL;
		PHOENIX_DEPRECATED("renamed to DaedalusSymbolFlag::MERGED") static constexpr auto merged = MERGED;
	} // namespace DaedalusSymbolFlag

	/// \brief All opcodes supported by the daedalus interpreter.
	///
	/// <p>In the documentation for each opcode, the following conventions apply.</p>
	/// <ul>
	///   <li>`a` refers to the first value on the stack and is required to be an integer.</li>
	///   <li>`b` refers to the second value on the stack and is required to be an integer.</li>
	///   <li>`m` refers to the first value on the stack which may be of any non-reference type.</li>
	///   <li>`n` refers to the second value on the stack which may be of any non-reference type.</li>
	///   <li>`x` refers to the first value on the stack and is required to be variable reference.</li>
	///	  <li>`y` refers to the second value on the stack and is required to be variable reference.</li>
	/// </ul>
	enum class DaedalusOpcode : std::uint8_t {
		/// \brief Add `a` and `b` and put the result back onto the stack.
		ADD = 0,

		/// \brief Subtract `b` from `a` and put the result back onto the stack.
		SUB = 1,

		/// \brief Multiply `a` and `b` and put the result back onto the stack.
		MUL = 2,

		/// \brief Divide `a` by `b` and put the result back onto the stack.
		DIV = 3,

		/// \brief Divide `a` by `b` and put the remainder back onto the stack.
		MOD = 4,

		/// \brief Compute the bitwise or of `a` and `b` and put the result back onto the stack.
		OR = 5,

		/// \brief Compute the bitwise and of `a` and `b` and put the result back onto the stack.
		ANDB = 6, ///< a & b

		/// \brief Test if `a` is less than `b` and put `1` or `0` onto the stack if
		///        the test is true or false respectively.
		LT = 7,

		/// \brief Test if `a` is greater than `b` and put `1` or `0` onto the stack
		///        if the test is true or false respectively.
		GT = 8,

		/// \brief Write `b` to `x` as an integer.
		MOVI = 9,

		/// \brief Test if `a == 1` or `b == 1` and put `1` or `0` onto the stack if
		/// 		the test is true or false respectively.
		ORR = 11,

		/// \brief Test if `a == 1` and `b == 1` and put `1` or `0` onto the stack if
		///        the test is true or false respectively.
		AND = 12,

		/// \brief Left shift  `a` by `b` bits and put the result back onto the stack.
		LSL = 13,

		/// \brief Right shift  `a` by `b` bits and put the result back onto the stack.
		LSR = 14,

		/// \brief Test if `a` is less than or equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		LTE = 15,

		/// \brief Test if `a` is equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		EQ = 16,

		/// \brief Test if `a` is not equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		NEQ = 17,

		/// \brief Test if `a` is greater than or equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		GTE = 18,

		/// \brief Add `x` and `b` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		ADDMOVI = 19,

		/// \brief Subtract `b` from `x` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		SUBMOVI = 20,

		/// \brief Multiply `x` from `b` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		MULMOVI = 21,

		/// \brief Divide `x` by `b` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		DIVMOVI = 22,

		/// \brief Compute `+a` and put the result back onto the stack.
		PLUS = 30,

		/// \brief Compute `-a` and put the result back onto the stack.
		NEGATE = 31,

		/// \brief Compute `!a` and put the result back onto the stack.
		NOT = 32,

		/// \brief Compute the bitwise complement `a` and put the result back onto the stack.
		CMPL = 33,

		/// \brief Do nothing.
		NOP = 45,

		/// \brief Return from the currently running function
		RSR = 60,

		/// \brief Call the function at the address provided in the instruction.
		BL = 61,

		/// \brief Call the external function at the symbol index provided in the instruction.
		BE = 62,

		/// \brief Push the immediate value provided in the instruction onto the stack as an integer.
		PUSHI = 64,

		/// \brief Push the symbol with the index provided in the instruction onto the stack as a reference.
		PUSHV = 65,

		/// \brief Push the instance with the symbol index provided in the instruction onto the stack as a reference.
		PUSHVI = 67,

		/// \brief Write `m` to `x` as a string.
		MOVS = 70,

		/// \brief Write `m` to `x` as a string reference; not implemented.
		MOVSS = 71,

		/// \brief Write `b` to `x` as a function reference.
		MOVVF = 72,

		/// \brief Write `b` to `x` as a floating point number.
		MOVF = 73,

		/// \brief Write `y` to `x` as an instance reference.
		MOVVI = 74,

		/// \brief Immediately jump to the instruction at the address provided in the instruction.
		B = 75,

		/// \brief Jump to the instruction at the address provided in the instruction if `a == 0`.
		BZ = 76,

		/// \brief Set the global instance reference to the instance with the symbol index provided in the instrucion.
		GMOVI = 80,

		/// \brief Push the element at the given index of the symbol with the index provided in the
		///        instruction onto the stack as a reference.
		PUSHVV = 245,

		add PHOENIX_DEPRECATED("renamed to DaedalusDataType::ADD") = ADD,
		sub PHOENIX_DEPRECATED("renamed to DaedalusDataType::SUB") = SUB,
		mul PHOENIX_DEPRECATED("renamed to DaedalusDataType::MUL") = MUL,
		div PHOENIX_DEPRECATED("renamed to DaedalusDataType::DIV") = DIV,
		mod PHOENIX_DEPRECATED("renamed to DaedalusDataType::MOD") = MOD,
		or_ PHOENIX_DEPRECATED("renamed to DaedalusDataType::OR") = OR,
		andb PHOENIX_DEPRECATED("renamed to DaedalusDataType::ANDB") = ANDB,
		lt PHOENIX_DEPRECATED("renamed to DaedalusDataType::LT") = LT,
		gt PHOENIX_DEPRECATED("renamed to DaedalusDataType::GT") = GT,
		movi PHOENIX_DEPRECATED("renamed to DaedalusDataType::MOVI") = MOVI,
		orr PHOENIX_DEPRECATED("renamed to DaedalusDataType::ORR") = ORR,
		and_ PHOENIX_DEPRECATED("renamed to DaedalusDataType::ADD") = AND,
		lsl PHOENIX_DEPRECATED("renamed to DaedalusDataType::LSL") = LSL,
		lsr PHOENIX_DEPRECATED("renamed to DaedalusDataType::LSR") = LSR,
		lte PHOENIX_DEPRECATED("renamed to DaedalusDataType::LTE") = LTE,
		eq PHOENIX_DEPRECATED("renamed to DaedalusDataType::EQ") = EQ,
		neq PHOENIX_DEPRECATED("renamed to DaedalusDataType::NEQ") = NEQ,
		gte PHOENIX_DEPRECATED("renamed to DaedalusDataType::GTE") = GTE,
		addmovi PHOENIX_DEPRECATED("renamed to DaedalusDataType::ADDMOVI") = ADDMOVI,
		submovi PHOENIX_DEPRECATED("renamed to DaedalusDataType::SUBMOVI") = SUBMOVI,
		mulmovi PHOENIX_DEPRECATED("renamed to DaedalusDataType::MULMOVI") = MULMOVI,
		divmovi PHOENIX_DEPRECATED("renamed to DaedalusDataType::DIVMOVI") = DIVMOVI,
		plus PHOENIX_DEPRECATED("renamed to DaedalusDataType::PLUS") = PLUS,
		negate PHOENIX_DEPRECATED("renamed to DaedalusDataType::NEGATE") = NEGATE,
		not_ PHOENIX_DEPRECATED("renamed to DaedalusDataType::NOT") = NOT,
		cmpl PHOENIX_DEPRECATED("renamed to DaedalusDataType::CMPL") = CMPL,
		nop PHOENIX_DEPRECATED("renamed to DaedalusDataType::NOP") = NOP,
		rsr PHOENIX_DEPRECATED("renamed to DaedalusDataType::RSR") = RSR,
		bl PHOENIX_DEPRECATED("renamed to DaedalusDataType::BL") = BL,
		be PHOENIX_DEPRECATED("renamed to DaedalusDataType::BE") = BE,
		pushi PHOENIX_DEPRECATED("renamed to DaedalusDataType::PUSHI") = PUSHI,
		pushv PHOENIX_DEPRECATED("renamed to DaedalusDataType::PUSHV") = PUSHV,
		pushvi PHOENIX_DEPRECATED("renamed to DaedalusDataType::PUSHVI") = PUSHVI,
		movs PHOENIX_DEPRECATED("renamed to DaedalusDataType::MOVS") = MOVS,
		movss PHOENIX_DEPRECATED("renamed to DaedalusDataType::MOVSS") = MOVSS,
		movvf PHOENIX_DEPRECATED("renamed to DaedalusDataType::MOVVF") = MOVVF,
		movf PHOENIX_DEPRECATED("renamed to DaedalusDataType::MOVF") = MOVF,
		movvi PHOENIX_DEPRECATED("renamed to DaedalusDataType::MOVVI") = MOVVI,
		b PHOENIX_DEPRECATED("renamed to DaedalusDataType::B") = B,
		bz PHOENIX_DEPRECATED("renamed to DaedalusDataType::BZ") = BZ,
		gmovi PHOENIX_DEPRECATED("renamed to DaedalusDataType::GMOVI") = GMOVI,
		pushvv PHOENIX_DEPRECATED("renamed to DaedalusDataType::PUSHVV") = PUSHVV,
	};

	class DaedalusSymbol;

	/// \brief Represents an object associated with an instance in the script.
	///
	/// Every class defined in C++ that can be used as an instance has to inherit from this class.
	class DaedalusInstance {
	public:
		PHOENIX_API virtual ~DaedalusInstance() = default;

		/// \return The index of the symbol this instance is bound to.
		[[nodiscard]] PHOENIX_API inline uint32_t symbol_index() const {
			return _m_symbol_index;
		}

		/// \brief A pointer which may be used by the user of this API
		void* user_ptr = nullptr;

	private:
		friend class DaedalusSymbol;
		friend class DaedalusScript;
		friend class DaedalusVm;

		uint32_t _m_symbol_index {unset};
		const std::type_info* _m_type {nullptr};
	};

	/// \brief The base class for all exceptions thrown by interacting with a script.
	struct DaedalusScriptError : public Error {
		using Error::Error;
	};

	/// \brief An exception thrown if the symbol with a given name could not be found.
	struct DaedalusSymbolNotFound : public DaedalusScriptError {
	public:
		PHOENIX_API explicit DaedalusSymbolNotFound(std::string&& name);

	public:
		std::string name;
	};

	/// \brief An exception thrown if registering a class member was unsuccessful.
	struct DaedalusMemberRegistrationError : public DaedalusScriptError {
	public:
		PHOENIX_API explicit DaedalusMemberRegistrationError(DaedalusSymbol const* sym, std::string&& message);

	public:
		/// \brief The symbol being registered.
		DaedalusSymbol const* sym;
	};

	/// \brief An exception thrown if the type of the member being registered does not match the type provided.
	struct DaedalusInvalidRegistrationDataType final : public DaedalusMemberRegistrationError {
	public:
		PHOENIX_API explicit DaedalusInvalidRegistrationDataType(const DaedalusSymbol* sym, std::string&& given);

	public:
		std::string given;
	};

	/// \brief An exception thrown when the value of a symbol accessed in a way which is not permissible.
	struct DaedalusIllegalAccess : public DaedalusScriptError {
		using DaedalusScriptError::DaedalusScriptError;
	};

	/// \brief An exception thrown when the type of a symbol does not match the type expected.
	struct DaedalusIllegalTypeAccess final : public DaedalusIllegalAccess {
	public:
		PHOENIX_INTERNAL DaedalusIllegalTypeAccess(DaedalusSymbol const* sym, DaedalusDataType expected);

	public:
		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;

		/// \brief The datatype excepted.
		DaedalusDataType expected;
	};

	/// \brief An exception thrown when an out-of-bounds index is accessed.
	struct DaedalusIllegalIndexAccess final : public DaedalusIllegalAccess {
	public:
		PHOENIX_INTERNAL DaedalusIllegalIndexAccess(DaedalusSymbol const* sym, std::uint8_t index);

	public:
		/// \brief The symbol being accessed.
		const DaedalusSymbol* sym;

		/// \brief The index being accessed
		std::uint8_t index;
	};

	/// \brief An exception thrown when a constant symbol is accessed as mutable
	struct DaedalusIllegalConstAccess final : public DaedalusIllegalAccess {
	public:
		PHOENIX_INTERNAL explicit DaedalusIllegalConstAccess(DaedalusSymbol const* sym);

	public:
		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;
	};

	/// \brief An exception thrown when the parent class of a member does not match the class of an instance.
	struct DaedalusIllegalInstanceAccess final : public DaedalusIllegalAccess {
	public:
		PHOENIX_INTERNAL DaedalusIllegalInstanceAccess(DaedalusSymbol const* sym, std::uint32_t expected_parent);

	public:
		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;

		/// \brief The parent which was expected for the member.
		std::uint32_t expected_parent;
	};

	/// \brief An exception thrown when the parent class of a member does not match the class of an instance.
	struct DaedalusUnboundMemberAccess final : public DaedalusIllegalAccess {
	public:
		PHOENIX_API explicit DaedalusUnboundMemberAccess(DaedalusSymbol const* sym);

	public:
		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;
	};

	/// \brief An exception thrown if a member symbol is being access without a context set.
	struct DaedalusNoContextError final : public DaedalusIllegalAccess {
	public:
		PHOENIX_INTERNAL explicit DaedalusNoContextError(DaedalusSymbol const* sym);

	public:
		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;
	};

	/// \brief An exception thrown if a member symbol is being accessed with a context instance it is not bound to.
	struct DaedalusIllegalContextType final : public DaedalusIllegalAccess {
	public:
		PHOENIX_API DaedalusIllegalContextType(DaedalusSymbol const* sym, const std::type_info& context_type);

	public:
		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;

		/// \brief The type of context currently set.
		const std::type_info& context_type;
	};

	/// \brief Represents a compiled daedalus symbol.
	class DaedalusSymbol final {
	public:
		/// \brief Parses a symbol from the given reader.
		/// \param[in,out] in The reader to read the symbol from.
		/// \return The symbol parsed.
		[[nodiscard]] PHOENIX_API static DaedalusSymbol parse(Buffer& in);

		/// \brief Validates that the symbol is a string and retrieves it's value in the given context.
		/// \param index The index of the value to get.
		/// \param context An instance to use as context for getting member variables.
		/// \return The string associated with the symbol.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_string.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		[[nodiscard]] PHOENIX_API const std::string&
		get_string(std::size_t index = 0, const std::shared_ptr<DaedalusInstance>& context = nullptr) const;

		/// \brief Validates that the symbol is a float and retrieves it's value in the given context.
		/// \param index The index of the value to get.
		/// \param context An instance to use as context for getting member variables.
		/// \return The float value associated with the symbol.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_float.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		[[nodiscard]] PHOENIX_API float get_float(std::size_t index = 0,
		                                          const std::shared_ptr<DaedalusInstance>& context = nullptr) const;

		/// \brief Validates that the symbol is an int and retrieves it's value in the given context.
		/// \param index The index of the value to get.
		/// \param context An instance to use as context for getting member variables.
		/// \return The int value associated with the symbol.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_int or dt_function.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		[[nodiscard]] PHOENIX_API std::int32_t
		get_int(std::size_t index = 0, const std::shared_ptr<DaedalusInstance>& context = nullptr) const;

		/// \brief Validates that the symbol is an instance and retrieves it's value
		/// \return The instance associated with the symbol.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_instance
		[[nodiscard]] PHOENIX_API const std::shared_ptr<DaedalusInstance>& get_instance();

		// -=-= Value setters =-=- //

		/// \brief Validates that the symbol is a string and not constant and sets it's value in the given context.
		/// \param value The new value to set.
		/// \param index The index of the value to set
		/// \param context An instance to use as context for setting member variables.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_string.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		PHOENIX_API void set_string(std::string_view value,
		                            std::size_t index = 0,
		                            const std::shared_ptr<DaedalusInstance>& context = nullptr);

		/// \brief Validates that the symbol is a float and not constant and sets it's value in the given context.
		/// \param value The new value to set.
		/// \param index The index of the value to set
		/// \param context An instance to use as context for setting member variables.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_float
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		PHOENIX_API void
		set_float(float value, std::size_t index = 0, const std::shared_ptr<DaedalusInstance>& context = nullptr);

		/// \brief Validates that the symbol is an int and not constant and sets it's value in the given context.
		/// \param value The new value to set.
		/// \param index The index of the value to set
		/// \param context An instance to use as context for setting member variables.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_int or dt_function.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		PHOENIX_API void
		set_int(std::int32_t value, std::size_t index = 0, const std::shared_ptr<DaedalusInstance>& context = nullptr);

		/// \brief Validates that the symbol is an instance and sets it's value
		/// \param inst The instance value to set
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_instance.
		PHOENIX_API void set_instance(const std::shared_ptr<DaedalusInstance>& inst);

		/// \brief Tests whether this symbol holds an instance of the given type.
		/// \tparam T The type of instance to check for.
		/// \return <tt>true</tt> if the symbol contains an instance of the given type, <tt>false</tt> if not.
		template <typename T>
		PHOENIX_API typename std::enable_if<std::is_base_of_v<DaedalusInstance, T>,
		                                    bool>::type inline is_instance_of() { // clang-format on
			return this->type() == DaedalusDataType::INSTANCE && this->get_instance() != nullptr &&
			    this->get_instance()->_m_type == &typeid(T);
		}

		/// \brief Tests whether the symbol is a constant.
		/// \return `true` if the symbol is a constant, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_const() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::CONST) != 0;
		}

		/// \brief Tests whether the symbol is a member variable.
		/// \return `true` if the symbol is a member, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_member() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::MEMBER) != 0;
		}

		/// \brief Tests whether the symbol is an extern symbol.
		/// \return `true` if the symbol is an extern symbol, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_external() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::EXTERNAL) != 0;
		}

		/// \brief Tests whether the symbol is merged.
		/// \return `true` if the symbol is merged, `false` if not.
		/// \note It is currently not known what 'merged' means.
		[[nodiscard]] PHOENIX_API inline bool is_merged() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::MERGED) != 0;
		}

		/// \brief brief Tests whether the symbol is a compiler-generated symbol
		/// \return return `true` if the symbol is generated, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_generated() const noexcept {
			return _m_generated;
		}

		/// \brief brief Tests whether the symbol has a return value.
		/// \return return `true` if the symbol has a return value, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool has_return() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::RETURN) != 0;
		}

		/// \return The name of the symbol.
		[[nodiscard]] PHOENIX_API inline const std::string& name() const noexcept {
			return _m_name;
		}

		/// \return The address of the symbol.
		[[nodiscard]] PHOENIX_API inline std::uint32_t address() const noexcept {
			return _m_address;
		}

		/// \return The index of the parent symbol or unset if the symbol does not have a parent.
		[[nodiscard]] PHOENIX_API inline std::uint32_t parent() const noexcept {
			return _m_parent;
		}

		/// \return The count of values stored in the symbol.
		[[nodiscard]] PHOENIX_API inline std::uint32_t count() const noexcept {
			return _m_count;
		}

		/// \return The type of the symbol.
		[[nodiscard]] PHOENIX_API inline DaedalusDataType type() const noexcept {
			return _m_type;
		}

		/// \return The index of the symbol.
		[[nodiscard]] PHOENIX_API inline std::uint32_t index() const noexcept {
			return _m_index;
		}

		/// \return The return type of the symbol.
		[[nodiscard]] PHOENIX_API inline DaedalusDataType rtype() const noexcept {
			return _m_return_type;
		}

		/// \return The index of the file the symbol was in.
		[[nodiscard]] PHOENIX_API inline std::uint32_t file_index() const noexcept {
			return _m_file_index;
		}

		/// \return The offset in bytes of a member from the start of the instance.
		[[nodiscard]] PHOENIX_API inline std::uint32_t offset_as_member() const noexcept {
			return _m_member_offset;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t line_start() const noexcept {
			return _m_line_start;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t line_count() const noexcept {
			return _m_line_count;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t char_start() const noexcept {
			return _m_char_start;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t char_count() const noexcept {
			return _m_char_count;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t class_size() const noexcept {
			return _m_class_size;
		}

		[[nodiscard]] PHOENIX_API inline const std::type_info& registered_to() const noexcept {
			return *_m_registered_to;
		};

	protected:
		PHOENIX_INTERNAL DaedalusSymbol() = default;

		template <typename T>
		inline const T* get_member_ptr(std::uint8_t index, const std::shared_ptr<DaedalusInstance>& context) const {
			if (!_m_registered_to)
				throw DaedalusUnboundMemberAccess(this);
			if (*_m_registered_to != *context->_m_type)
				throw DaedalusIllegalContextType {this, *context->_m_type};

			std::uint32_t target_offset = offset_as_member() + index * sizeof(T);
			return reinterpret_cast<const T*>(reinterpret_cast<const char*>(context.get()) + target_offset);
		}

		template <typename T>
		inline T* get_member_ptr(std::uint8_t index, const std::shared_ptr<DaedalusInstance>& context) {
			if (!_m_registered_to)
				throw DaedalusUnboundMemberAccess(this);
			if (*_m_registered_to != *context->_m_type)
				throw DaedalusIllegalContextType {this, *context->_m_type};

			std::uint32_t target_offset = offset_as_member() + index * sizeof(T);
			return reinterpret_cast<T*>(reinterpret_cast<char*>(context.get()) + target_offset);
		}

	private:
		friend class DaedalusScript;
		std::string _m_name;
		std::variant<std::unique_ptr<std::int32_t[]>,
		             std::unique_ptr<float[]>,
		             std::unique_ptr<std::string[]>,
		             std::shared_ptr<DaedalusInstance>>
		    _m_value;

		std::uint32_t _m_address {unset};
		std::uint32_t _m_parent {unset};
		std::uint32_t _m_class_offset {unset};
		std::uint32_t _m_count {0};
		DaedalusDataType _m_type {0};
		std::uint32_t _m_flags {0};
		bool _m_generated {false};

		std::uint32_t _m_file_index {0};
		std::uint32_t _m_line_start {0};
		std::uint32_t _m_line_count {0};
		std::uint32_t _m_char_start {0};
		std::uint32_t _m_char_count {0};

		std::uint32_t _m_member_offset {unset};
		std::uint32_t _m_class_size {unset};
		DaedalusDataType _m_return_type {DaedalusDataType::VOID};
		std::uint32_t _m_index {unset};
		const std::type_info* _m_registered_to {nullptr};
	};

	/// \brief Represents a daedalus VM instruction.
	struct DaedalusInstruction {
		DaedalusOpcode op {DaedalusOpcode::NOP};
		std::uint32_t address {0};
		std::uint32_t symbol {0};
		std::int32_t immediate {0};
		std::uint8_t index {0};
		std::uint8_t size {1};

		/// \brief Reads an instruction from a reader.
		/// \param[in,out] in The reader to read from
		/// \return The instruction read.
		PHOENIX_INTERNAL static DaedalusInstruction decode(Buffer& in);
	};

	/// \brief Represents a compiled daedalus script

	class DaedalusScript {
	public:
		PHOENIX_API DaedalusScript(const DaedalusScript& copy) = default;
		PHOENIX_API DaedalusScript(DaedalusScript&& move) = default;

		/// \brief Parses in a compiled daedalus script.
		/// \param path The path of the script file.
		/// \return The script parsed
		[[nodiscard]] PHOENIX_API static DaedalusScript parse(const std::string& path);

		/// \brief Parses in a compiled daedalus script.
		/// \param buf A buffer containing the script data.
		/// \return The script parsed
		[[nodiscard]] PHOENIX_API static DaedalusScript parse(phoenix::Buffer& buf);

		/// \brief Registers a member offset
		/// \param name The name of the member in the script
		/// \param field The field to register
		/// \throws DaedalusSymbolNotFound if there is no symbol with the given name.
		/// \throws DaedalusMemberRegistrationError if the member could not be registered
		/// \throws DaedalusInvalidRegistrationDataType If the datatype of \p _member is different than that of the
		/// symbol.
		template <typename _class, typename _member, int N>
		typename std::enable_if<std::is_same_v<_member, std::string> || std::is_same_v<_member, float> ||
		                            std::is_same_v<_member, std::int32_t> ||
		                            (std::is_enum_v<_member> && sizeof(_member) == 4),
		                        void>::type
		register_member(std::string_view name, _member (_class::*field)[N]) { // clang-format on
			auto* type = &typeid(_class);
			auto* sym = _check_member<_class, _member, N>(name, type);

			_class* base = 0;
			auto member = &(base->*field);
			sym->_m_member_offset = std::uint64_t(member) & 0xFFFFFFFF;
			sym->_m_registered_to = type;
		}

		/// \brief Registers a member offset
		/// \param name The name of the member in the script
		/// \param field The field to register
		/// \throws DaedalusSymbolNotFound if there is no symbol with the given name.
		/// \throws DaedalusMemberRegistrationError if the member could not be registered
		/// \throws DaedalusInvalidRegistrationDataType If the datatype of \p _member is different than that of the
		/// symbol.
		template <typename _class, typename _member>
		typename std::enable_if<std::is_same_v<_member, std::string> || std::is_same_v<_member, float> ||
		                            std::is_same_v<_member, std::int32_t> ||
		                            (std::is_enum_v<_member> && sizeof(_member) == 4),
		                        void>::type
		register_member(std::string_view name, _member _class::*field) {
			auto* type = &typeid(_class);
			auto* sym = _check_member<_class, _member, 1>(name, type);

			_class* base = 0;
			auto member = &(base->*field);
			sym->_m_member_offset = std::uint64_t(member) & 0xFFFFFFFF;
			sym->_m_registered_to = type;
		}

		/// \return All symbols in the script
		[[nodiscard]] PHOENIX_API inline const std::vector<DaedalusSymbol>& symbols() const noexcept {
			return _m_symbols;
		}

		/// \brief Retrieves the symbol with the given \p index
		/// \param index The index of the symbol to get
		/// \return The symbol or `nullptr` if the index was out-of-range.
		[[nodiscard]] PHOENIX_API const DaedalusSymbol* find_symbol_by_index(std::uint32_t index) const;

		/// \brief Looks for parameters of the given function symbol. Only works for external functions.
		/// \param parent The function symbol to get the parameter symbols for.
		/// \return A list of function parameter symbols.
		[[nodiscard]] PHOENIX_API std::vector<const DaedalusSymbol*>
		find_parameters_for_function(const DaedalusSymbol* parent) const;

		/// \brief Retrieves the symbol with the given \p address set
		/// \param index The address of the symbol to get
		/// \return The symbol or `nullptr` if no symbol with that address was found.
		[[nodiscard]] PHOENIX_API const DaedalusSymbol* find_symbol_by_address(std::uint32_t address) const;

		/// \brief Retrieves the symbol with the given \p name.
		/// \param name The name of the symbol to get.
		/// \return The symbol or `nullptr` if no symbol with that name was found.
		[[nodiscard]] PHOENIX_API const DaedalusSymbol* find_symbol_by_name(std::string_view name) const;

		/// \brief Retrieves the symbol with the given \p index
		/// \param index The index of the symbol to get
		/// \return The symbol or `nullptr` if the index was out-of-range.
		[[nodiscard]] PHOENIX_API DaedalusSymbol* find_symbol_by_index(std::uint32_t index);

		/// \brief Retrieves the symbol with the given \p address set
		/// \param index The address of the symbol to get
		/// \return The symbol or `nullptr` if no symbol with that address was found.
		[[nodiscard]] PHOENIX_API DaedalusSymbol* find_symbol_by_address(std::uint32_t address);

		/// \brief Looks for parameters of the given function symbol. Only works for external functions.
		/// \param parent The function symbol to get the parameter symbols for.
		/// \return A list of function parameter symbols.
		[[nodiscard]] PHOENIX_API std::vector<DaedalusSymbol*>
		find_parameters_for_function(const DaedalusSymbol* parent);

		/// \brief Retrieves the symbol with the given \p name.
		/// \param name The name of the symbol to get.
		/// \return The symbol or `nullptr` if no symbol with that name was found.
		[[nodiscard]] PHOENIX_API DaedalusSymbol* find_symbol_by_name(std::string_view name);

		/// \brief Call the given callback function for every instance symbol which is a descendant of the class with
		///        the given name.
		/// \param name The name of the parent class.
		/// \param callback The function to call with each instance symbol.
		PHOENIX_API void enumerate_instances_by_class_name(std::string_view name,
		                                                   const std::function<void(DaedalusSymbol&)>& callback);

		/// \brief Decodes the instruction at \p address and returns it.
		/// \param address The address of the instruction to decode
		/// \return The instruction.
		[[nodiscard]] PHOENIX_API DaedalusInstruction instruction_at(std::uint32_t address) const;

		/// \return The total size of the script.
		[[nodiscard]] PHOENIX_API std::uint32_t size() const noexcept {
			return _m_text.limit() & 0xFFFFFF;
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		PHOENIX_API inline const DaedalusSymbol* find_symbol_by_instance(const DaedalusInstance& inst) const {
			return find_symbol_by_index(inst._m_symbol_index);
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		PHOENIX_API inline DaedalusSymbol* find_symbol_by_instance(const DaedalusInstance& inst) {
			return find_symbol_by_index(inst._m_symbol_index);
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		template <typename T>
		PHOENIX_API typename std::enable_if<std::is_base_of_v<DaedalusInstance, T>, const DaedalusSymbol*>::
		    type inline find_symbol_by_instance(const std::shared_ptr<T>& inst) const { // clang-format on
			return find_symbol_by_index(inst->_m_symbol_index);
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		template <typename T>
		PHOENIX_API typename std::enable_if<std::is_base_of_v<DaedalusInstance, T>, DaedalusSymbol*>::
		    type inline find_symbol_by_instance(const std::shared_ptr<T>& inst) {
			return find_symbol_by_index(inst->_m_symbol_index);
		}

	protected:
		PHOENIX_INTERNAL DaedalusScript() = default;

		template <typename _class, typename _member, int N>
		DaedalusSymbol* _check_member(std::string_view name, const std::type_info* type) {
			auto* sym = find_symbol_by_name(name);

			if (sym == nullptr)
				throw DaedalusSymbolNotFound {std::string {name}};
			if (!sym->is_member())
				throw DaedalusMemberRegistrationError {sym, "not a member"};
			if (sym->count() > N)
				throw DaedalusMemberRegistrationError {sym,
				                                       "incorrect number of elements: given " + std::to_string(N) +
				                                           " expected " + std::to_string(sym->count())};

			// check class registration
			auto* parent = find_symbol_by_index(sym->parent());
			if (parent == nullptr)
				throw DaedalusMemberRegistrationError {sym, "no parent found"};

			if (parent->_m_registered_to == nullptr) {
				parent->_m_registered_to = type;
			} else if (parent->_m_registered_to != type) {
				throw DaedalusMemberRegistrationError {sym,
				                                       "parent class is already registered with a different type (" +
				                                           std::string {parent->_m_registered_to->name()} + ")"};
			}

			// check type matches
			if constexpr (std::is_same_v<std::string, _member>) {
				if (sym->type() != DaedalusDataType::STRING)
					throw DaedalusInvalidRegistrationDataType {sym, "string"};
			} else if constexpr (std::is_same_v<float, _member>) {
				if (sym->type() != DaedalusDataType::FLOAT)
					throw DaedalusInvalidRegistrationDataType {sym, "float"};
			} else if constexpr (std::is_same_v<int32_t, _member> || std::is_enum_v<_member>) {
				if (sym->type() != DaedalusDataType::INT && sym->type() != DaedalusDataType::FUNCTION)
					throw DaedalusInvalidRegistrationDataType {sym, "int"};
			} else {
				throw DaedalusInvalidRegistrationDataType {sym, "<unknown>"};
			}

			return sym;
		}

		PHOENIX_API DaedalusSymbol* add_temporary_strings_symbol();

	private:
		std::vector<DaedalusSymbol> _m_symbols;
		std::unordered_map<std::string, uint32_t> _m_symbols_by_name;
		std::unordered_map<std::uint32_t, uint32_t> _m_symbols_by_address;

		mutable Buffer _m_text = Buffer::empty();
		std::uint8_t _m_version {0};
	};

	using datatype PHOENIX_DEPRECATED("renamed to DaedalusDataType") = DaedalusDataType;
	namespace symbol_flag = DaedalusSymbolFlag;
	using opcode PHOENIX_DEPRECATED("renamed to DaedalusOpcode") = DaedalusOpcode;
	using symbol PHOENIX_DEPRECATED("renamed to DaedalusSymbol") = DaedalusSymbol;
	using instance PHOENIX_DEPRECATED("renamed to DaedalusInstance") = DaedalusInstance;
	using script_error PHOENIX_DEPRECATED("renamed to DaedalusScriptError") = DaedalusScriptError;
	using symbol_not_found PHOENIX_DEPRECATED("renamed to DaedalusSymbolNotFound") = DaedalusSymbolNotFound;
	using member_registration_error
	    PHOENIX_DEPRECATED("renamed to DaedalusMemberRegistrationError") = DaedalusMemberRegistrationError;
	using invalid_registration_datatype
	    PHOENIX_DEPRECATED("renamed to DaedalusInvalidRegistrationDataType") = DaedalusInvalidRegistrationDataType;
	using illegal_access PHOENIX_DEPRECATED("renamed to DaedalusIllegalAccess") = DaedalusIllegalAccess;
	using illegal_type_access PHOENIX_DEPRECATED("renamed to DaedalusIllegalTypeAccess") = DaedalusIllegalTypeAccess;
	using illegal_index_access PHOENIX_DEPRECATED("renamed to DaedalusIllegalIndexAccess") = DaedalusIllegalIndexAccess;
	using illegal_const_access PHOENIX_DEPRECATED("renamed to DaedalusIllegalConstAccess") = DaedalusIllegalConstAccess;
	using illegal_instance_access
	    PHOENIX_DEPRECATED("renamed to DaedalusIllegalInstanceAccess") = DaedalusIllegalInstanceAccess;
	using unbound_member_access
	    PHOENIX_DEPRECATED("renamed to DaedalusUnboundMemberAccess") = DaedalusUnboundMemberAccess;
	using no_context PHOENIX_DEPRECATED("renamed to DaedalusNoContextError") = DaedalusNoContextError;
	using illegal_context_type PHOENIX_DEPRECATED("renamed to DaedalusIllegalContextType") = DaedalusIllegalContextType;
	using instruction PHOENIX_DEPRECATED("renamed to DaedalusInstruction") = DaedalusInstruction;
	using script PHOENIX_DEPRECATED("renamed to DaedalusScript") = DaedalusScript;
} // namespace phoenix
