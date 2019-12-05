#ifndef UNTITLED1_FIBIN_H
#define UNTITLED1_FIBIN_H

#include <iostream>
#include <type_traits>
#include <typeinfo>

struct True{};
struct False{};

struct EmptyContext{};

constexpr uint64_t Var(const char * s) {
	uint64_t result = 0;
	for (auto it = s; *it != '\0'; it++) {
		result *= 256;
		result += *it;
	}
	return result;
}

template <uint64_t Name1, typename Value1, typename Tail>
struct ContextType{
	template <typename ValueType, uint64_t name, typename FullContext>
	static constexpr ValueType eval() {
		if (Name1 == name)
			return Value1::template eval<ValueType, FullContext>();
		return Tail::template eval<ValueType, name, FullContext>();
	}
	template <typename ValueType, uint64_t name, typename Argument, typename FullContext>
	static constexpr ValueType evalFunction() {
		if (Name1 == name)
			return Value1::template evalFunction<ValueType, Argument, FullContext>();
		return Tail::template evalFunction<ValueType, name, Argument, FullContext>();
	}
};

template <typename ValueType, bool IsNumeric =
(std::is_integral<ValueType>::value && !std::is_same<ValueType, bool>::value)>
struct Fibin {
	template <typename Expr>
	static constexpr ValueType eval() {
		return Expr::template eval<ValueType, EmptyContext>();
	}
};

template <typename ValueType>
struct Fibin <ValueType, false> {
	template <typename Expr>
	static constexpr void eval() {
		std::cout << "Fibin doesn't support: " << typeid(ValueType).name() << std::endl;
	}
};

template <typename Left, typename Right>
struct Eq {
	template <typename ValueType, typename Context>
	static constexpr bool eval() {
		static_assert(std::is_same<ValueType, bool>::value, "Binary method with nonbinary return type");
		return Left::template eval<ValueType, Context>() == Right::template eval<ValueType, Context>();
	}
};

template <auto n>
struct Fib {
	template <typename ValueType, typename Context = EmptyContext>
	static constexpr ValueType eval() {
		static_assert(!std::is_same<ValueType, bool>::value, "Nonbinary method with binary return type");
		static_assert(n >= 0);
		if (n < 2)
			return n;
		return Fib<n - 1>::template eval<ValueType>() + Fib<n - 2>::template eval<ValueType>();
	}
};

template <typename T>
struct Lit{
	static_assert(!std::is_same<T,T>::value);
};

template <auto n>
struct Lit<Fib<n>>{
	template <typename ValueType, typename Context = EmptyContext>
	static constexpr ValueType eval() {
		return Fib<n>:: template eval<ValueType>();
	}
};

template <>
struct Lit<False> {
	template <typename ValueType>
	static constexpr ValueType eval() {
		static_assert(std::is_same<ValueType, bool>::value, "Binary method with nonbool return type");
		return false;
	}
};

template <>
struct Lit<True> {
	template <typename ValueType>
	static constexpr ValueType eval() {
		static_assert(std::is_same<ValueType, bool>::value, "Binary method with nonbool return type");
		return true;
	}
};

template <typename Condition, typename Then, typename Else>
struct If {
	template <typename ValueType, typename Context>
	static constexpr ValueType eval() {
		if (Condition::template eval<bool, Context>())
			return Then::template eval<ValueType, Context>();
		else
			return Else::template eval<ValueType, Context>();
	}
	template <typename ValueType, typename Argument, typename Context>
	static constexpr ValueType evalFunction() {
		if (Condition::template eval<bool, Context>())
			return Then::template evalFunction<ValueType, Argument, Context>();
		else
			return Else::template evalFunction<ValueType, Argument, Context>();
	}
};

template<typename E1, typename E2, typename... Args>
struct Sum {
	template <typename ValueType, typename Context>
	static constexpr ValueType eval() {
		return E1::template eval<ValueType, Context>() + Sum<E2, Args...>::template eval<ValueType, Context>();
	}
};

template<typename E1, typename E2>
struct Sum<E1, E2> {
	template <typename ValueType, typename Context>
	static constexpr ValueType eval() {
		return E1::template eval<ValueType, Context>() + E2::template eval<ValueType, Context>();
	}
};

template<uint64_t id>
struct Ref {
	template <typename ValueType, typename Context>
	static constexpr ValueType eval() {
		return Context::template eval<ValueType, id, Context>();
	}
	template <typename ValueType, typename Argument, typename Context>
	static constexpr ValueType evalFunction() {
		return Context::template evalFunction<ValueType, id, Argument, Context>();
	}
};

template<uint64_t id, typename Value, typename Expression>
struct Let {
	template <typename ValueType, typename Context>
	static constexpr ValueType eval() {
		return Expression::template eval<ValueType, ContextType<id, Value, Context>>();
	}
	template <typename ValueType, typename Argument, typename Context>
	static constexpr ValueType evalFunction() {
		return Expression::template evalFunction<ValueType, Argument, ContextType<id, Value, Context>>();
	}
};

template <uint64_t id, typename Body>
struct Lambda {
	template <typename ValueType, typename Argument, typename Context>
	static constexpr ValueType evalFunction() {
		return Body::template eval<ValueType, ContextType<id, Argument, Context>>();
	}
};

template <typename Fun, typename Param>
struct Invoke {
	template <typename ValueType, typename Context>
	static constexpr ValueType eval() {
		return Fun::template evalFunction<ValueType, Param, Context>();
	}
};

#endif //UNTITLED1_FIBIN_H