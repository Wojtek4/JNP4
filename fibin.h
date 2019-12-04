#include <iostream>
#include <type_traits>
#include <typeinfo>

template <typename ValueType, bool IsNumeric = 
		(std::is_integral<ValueType>::value && !std::is_same<ValueType, bool>::value)>
class Fibin {
	template <typename Expr>
	static constexpr ValueType eval() {
		return Expr::template eval<ValueType>();
	}
};

template <typename ValueType>
class Fibin <ValueType, false> {
	template <typename Expr>
	static constexpr void eval() {
        std::cout << "Fibin doesn't support: " << typeid(ValueType).name() << std::endl;
	}
};

template <typename Left, typename Right>
class Eq {
	template <typename ValueType>
	static constexpr bool eval() {
		static_assert(std::is_same<ValueType, bool>::value, "Binary method with nonbinary return type");
		return Left::template eval<ValueType>() == Right::template eval<ValueType>();
	}
};

template <int n>
class Lit {
	template <typename ValueType>
	static constexpr ValueType eval() {
		static_assert(!std::is_same<ValueType, bool>::value, "Nonbinary method with binary return type");
		if (n < 2)
			return n;
		return Lit<n - 1>::template eval<ValueType>() + Lit<n - 2>::template eval<ValueType>();
	}
};

template <>
class Lit<false> {
	template <typename ValueType>
	static constexpr ValueType eval() {
		static_assert(std::is_same<ValueType, bool>::value, "Binary method with nonbool return type");
		return false;
	}
};

template <>
class Lit<true> {
	template <typename ValueType>
	static constexpr ValueType eval() {
		static_assert(std::is_same<ValueType, bool>::value, "Binary method with nonbool return type");
		return true;
	}
};

template <typename Condition, typename Then, typename Else>
class If {
	template <typename ValueType>
	static constexpr ValueType eval() {
		if (Condition::template eval<bool>())
			return Then::template eval<ValueType>();
		else
			return Else::template eval<ValueType>();
	}
};

template<typename E1, typename E2, typename... Args>
class Sum {
	template <typename ValueType>
	static constexpr ValueType eval() {
		return E1::template eval<ValueType>() + Sum<E2, Args...>::template eval<ValueType>();
	}
};

template<typename E1, typename E2>
class Sum<E1, E2> {
	template <typename ValueType>
	static constexpr ValueType eval() {
		return E1::template eval<ValueType>() + E2::template eval<ValueType>();
	}
};
