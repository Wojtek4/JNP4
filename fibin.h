#ifndef UNTITLED1_FIBIN_H
#define UNTITLED1_FIBIN_H

#include <iostream>
#include <type_traits>
#include <typeinfo>

struct True{};
struct False{};

struct EmptyContext{};
struct EmptyArglist{};

constexpr uint64_t Var(const char * s) {
	uint64_t result = 0;
	for (auto it = s; *it != '\0'; it++) {
		result *= 256;
		result += *it;
		if (*it <= 'Z' && *it >= 'A')
			result += 'a' - 'A';
	}
	return result;
}

template <uint64_t Name1, typename Value1, typename Tail>
struct ContextType{
	template <typename ValueType, uint64_t name, typename FullContext, typename Arglist>
	static constexpr ValueType eval() {
		if constexpr (name == Name1)
			return Value1::template eval<ValueType, FullContext, Arglist>();
		else
			return Tail::template eval<ValueType, name, FullContext, Arglist>();
	}

	/*template <typename ValueType, uint64_t name, typename Argument, typename FullContext>
	static constexpr ValueType evalFunction() {
		if constexpr (name == Name1)
			return Value1::template evalFunction<ValueType, Argument, FullContext>();
		else
			return Tail::template evalFunction<ValueType, name, Argument, FullContext>();
	}*/
};

template <typename ValueType, bool IsNumeric =
(std::is_integral<ValueType>::value && !std::is_same<ValueType, bool>::value)>
struct Fibin {
	template <typename Expr>
	static constexpr ValueType eval() {
		return Expr::template eval<ValueType, EmptyContext, EmptyArglist>();
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
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr bool eval() {
		//static_assert(std::is_same<ValueType, bool>::value, "Eq returns True/False, not a number");
		return Left::template eval<ValueType, Context, Arglist>() == Right::template eval<ValueType, Context, Arglist>();
	}
};

template <auto n>
struct Fib {
	template <typename ValueType, typename Context = EmptyContext, typename Arglist = EmptyArglist>
	static constexpr ValueType eval() {
		static_assert(!std::is_same<ValueType, bool>::value, "Number is not a True/False value");
		static_assert(n >= 0);
		if constexpr (n < 2)
			return n;
		else
			return Fib<n - 1>::template eval<ValueType>() + Fib<n - 2>::template eval<ValueType>();
	}
};

template <typename T>
struct Lit{
	static_assert(!std::is_same<T,T>::value, "Wrong Literal");
};

template <auto n>
struct Lit<Fib<n>>{
	template <typename ValueType, typename Context = EmptyContext, typename Arglist = EmptyArglist>
	static constexpr ValueType eval() {
		return Fib<n>:: template eval<ValueType>();
	}
};

template <>
struct Lit<False> {
	template <typename ValueType, typename Context = EmptyContext, typename Arglist = EmptyArglist>
	static constexpr ValueType eval() {
		//static_assert(std::is_same<ValueType, bool>::value, "False is not a number");
		return false;
	}
};

template <>
struct Lit<True> {
	template <typename ValueType, typename Context = EmptyContext, typename Arglist = EmptyArglist>
	static constexpr ValueType eval() {
		//static_assert(std::is_same<ValueType, bool>::value, "True is not a number");
		return true;
	}
};

template <typename Condition, typename Then, typename Else>
struct If {
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr ValueType eval() {
		if constexpr (Condition::template eval<ValueType, Context, Arglist>())
			return Then::template eval<ValueType, Context, Arglist>();
		else
			return Else::template eval<ValueType, Context, Arglist>();
	}
	/*template <typename ValueType, typename Argument, typename Context>
	static constexpr ValueType evalFunction() {
		if constexpr (Condition::template eval<bool, Context>())
			return Then::template evalFunction<ValueType, Argument, Context>();
		else
			return Else::template evalFunction<ValueType, Argument, Context>();
	}*/
};

template<typename E1, typename E2, typename... Args>
struct Sum {
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr ValueType eval() {
		return E1::template eval<ValueType, Context, Arglist>() + Sum<E2, Args...>::template eval<ValueType, Context, Arglist>();
	}
};

template<typename E1, typename E2>
struct Sum<E1, E2> {
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr ValueType eval() {
		return E1::template eval<ValueType, Context, Arglist>() + E2::template eval<ValueType, Context, Arglist>();
	}
};

template<typename Arg>
struct Inc10{
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr ValueType eval() {
		return Sum<Lit<Fib<10>>, Arg>:: template eval<ValueType, Context, Arglist>();
	}
};

template<typename Arg>
struct Inc1{
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr ValueType eval() {
		return Sum<Lit<Fib<1>>, Arg>:: template eval<ValueType, Context, Arglist>();
	}
};

template<uint64_t id>
struct Ref {
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr ValueType eval() {
		return Context::template eval<ValueType, id, Context, Arglist>();
	}
	/*template <typename ValueType, typename Argument, typename Context>
	static constexpr ValueType evalFunction() {
		return Context::template evalFunction<ValueType, id, Argument, Context, Arglist>();
	}*/
};

template<uint64_t id, typename Value, typename Expression>
struct Let {
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr ValueType eval() {
		return Expression::template eval<ValueType, ContextType<id, Value, Context>, Arglist>();
	}
	/*template <typename ValueType, typename Argument, typename Context>
	static constexpr ValueType evalFunction() {
		return Expression::template evalFunction<ValueType, Argument, ContextType<id, Value, Context>>();
	}*/
};

template <uint64_t id, typename Body>
struct Lambda {
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr ValueType eval() {
		return Arglist::template eval<ValueType, id, Body, Context>();
	}

	/*template <typename ValueType, typename Argument, typename Context>
	static constexpr ValueType evalFunction() {
		return Body::template eval<ValueType, ContextType<id, Argument, Context>>();
	}*/
};

template <typename Arg1, typename Tail>
struct ArglistType {
	template <typename ValueType, uint64_t variable, typename Function, typename Context>
	static constexpr ValueType eval() {
		return Let<variable, Arg1, Function>::template eval<ValueType, Context, Tail>();
	}
};

template <typename Fun, typename Param>
struct Invoke {
	template <typename ValueType, typename Context, typename Arglist>
	static constexpr ValueType eval() {
		return Fun::template eval<ValueType, Context, ArglistType<Param, Arglist>>();
		//return Fun::template evalFunction<ValueType, Param, Context>();
	}
};

#endif //UNTITLED1_FIBIN_H