/*
学习C++模板
空明流转的教程，写得很好理解，还没写完：https://github.com/wuye9036/CppTemplateTutorial

*/

#include<iostream>
#include<bitset>
#include<vector>
#include <tuple>
using namespace std;

#define WRONG_CODE_ENABLED 0

// ---------------
// 可变数量的模板参数
// ---------------


// print任意数量任意类型，替代FG_TRACE
namespace _1_ {
// 特化
void print() {}

template<class T, class... Ts>
void print(const T& args1st, const Ts&... args)
{
	cout << args1st << endl;
	cout << (sizeof...(args)) << " params left:" << endl;
	print(args...);
}

// 泛化
template<class... Ts>
void print(const Ts&... args)
{
	cout << "!" << endl;
}

void test_print()
{
	print(1, 2, "helo", bitset<16>(0101), 42, 9.30f);
}

}

// ------------------Tuple------------------
// .	 .	  .	   .	.	 Head
// .	 .	  .	   .	Head  
// .	 .	  .	   Head	  
// .	 .	  Head	
// .	 Head
// Head

// 任意数量类型的容器，应用场景？，性能？
namespace _2_ 
{
using namespace _1_;

template<class... Ts> class Tuple;

template<> class Tuple<> {};

template<class Head, class... Ts>
class Tuple<Head, Ts...> : private Tuple<Ts...>
{
	typedef Tuple<Ts...> inherited;
public:
	Tuple() {}
	Tuple(const Head& h, const Ts&... ts)
		: head_(h), inherited(ts...) {}

	const Head& head() const { return head_; }
	const inherited& tail() const { return *this; }

	/*
	怎么做类内部get？auto推导
	template<size_t I>
	const auto& get() const { return tail().get<I - 1>;	}

	template<0>
	const auto& get() const { return head(); }
	*/

	template<size_t I>
	const auto& get() const;

protected:
	Head head_;
};

template<size_t N, typename Head> class TupleElem;

template<size_t N, typename Head, typename... Ts>
class TupleElem<N, Tuple<Head, Ts...>> : public TupleElem <N - 1, Tuple<Ts...>>
{
public:
	typedef typename TupleElem<N, Tuple<Head, Ts...>>::Type type;
	static const type& Get(const Tuple<Head, Ts...>& tuple)
	{
		return TupleElem<N - 1, Tuple<Ts...>>::Get(tuple.tail());
	}
};

template<typename Head, typename... Ts>
class TupleElem<0, Tuple<Head, Ts...>>
{
public:
	typedef Head Type;
	static const Type& Get(const Tuple<Head, Ts...>& tuple)
	{
		return tuple.head();
	}
};

template<size_t I, typename Head, typename... Ts>
typename const TupleElem<I, Tuple<Head, Ts...>>::Type& Get(const Tuple<Head, Ts...>& t)
{
	return TupleElem<I, Tuple<Head, Ts...>>::Get(t);
}

template<class Head, class... Ts>
template<size_t I>
const auto& Tuple<Head, Ts...>::get() const
{
	return Get<I>(*this);
}

// -------------- std tuple --------------
void test_tuple()
{
	Tuple<int, float, string> t1(1, 0.1f, "h1");
	tuple<int, float, string> t2(2, 0.2f, "h2");

	bool b = is_same<decltype(t1), decltype(t2)>();

	cout << Get<0>(t1) << endl;
	cout << Get<1>(t1) << endl;
	cout << Get<2>(t1) << endl;

	cout << t1.get<0>() << endl;
	cout << t1.get<1>() << endl;
	cout << t1.get<2>() << endl;

	print(t1.get<0>(), t1.get<1>(), t1.get<2>());

	cout << get<0>(t2) << endl;
	cout << get<1>(t2) << endl;
	cout << get<2>(t2) << endl;
}

}

// SFINAE 替换失败不是错误，会继续尝试替换其他
namespace _3_ {

template <typename A>
struct B { using type = typename A::type; };

template <
	class T,
	class U = typename T::type,      // 若 T 无成员 type 则为 SFINAE 失败
	class V = typename B<T>::type    // 若 T 无成员 type 则为硬错误（ C++14 起保证不出现，因为到 U 的默认模板实参中的替换会首先失败）
> void foo(int) {};

struct C { using type = int; };
struct D { using type2 = int; };

#define explicit_instantiate 0
#if explicit_instantiate

template<> void foo<B<C>>(int) {} // 函数模板显式实例化

#if WRONG_CODE_ENABLED
	template<> void foo<B<D>>(int) {}
#endif

#else

void test_sfinae1()
{
	foo<B<C>>(0);
#if WRONG_CODE_ENABLED
	foo<B<D>>(0); // 函数模板隐式实例化
#endif
}

#endif // explicit_instantiate

}

namespace _4_{

template <int I> struct X { };
template <template <class T> class> struct Z { };
template <class T> void f(typename T::Y*) {}
template <class T> void g(X<T::N>*) {}
template <class T> void h(Z<T::template TT>*) {}
struct A {};
struct B { int Y; };
struct C { typedef int N; };
struct D { typedef int TT; };
struct B1 { typedef int Y; };
struct C1 { static const int N = 0; };
struct D1 {
	template <typename T>
	struct TT
	{
	};
};

void test_sfinae() {

#if WRONG_CODE_ENABLED
	// 下列各个情况推导失败：
	f<A>(0); // 不含成员 Y
	f<B>(0); // B 的 Y 成员不是类型
	g<C>(0); // C 的 N 成员不是非类型
	h<D>(0); // D 的 TT 成员不是模板
#endif

	// 下列各个情况推导成功：
	f<B1>(0);
	g<C1>(0);
	h<D1>(0);
}

}

// std::is_enum
namespace _5_{

/*
https://zh.cppreference.com/w/cpp/types/is_enum
https://cloud.tencent.com/developer/ask/120661
https://stackoverflow.com/questions/13082635/boostis-enum-how-it-works
https://stackoverflow.com/questions/11316912/is-enum-implementation
*/

#include <type_traits>

class AA {};

enum E {};

enum class Ec : int {};

template<class T>
struct my_is_enum { static const bool value = true; };

template<>
struct my_is_enum<enum T> { static const bool value = false; };


void test_is_enum()
{
	cout << my_is_enum<AA>::value << '\n';
	cout << my_is_enum<E>::value << '\n';
	cout << my_is_enum<Ec>::value << '\n';
	cout << my_is_enum<int>::value << '\n';

	is_enum<AA>;

	system( "pause" );
}

// is int
template<class T>
struct is_int { static const bool value = false; };

template<>
struct is_int<int> { static const bool value = true; };

#define MAKETRAIT(is_t, t, v) \
template<> \
struct is_t<t> { static const bool value = v; };

MAKETRAIT(is_int, unsigned, true)

#if WRONG_CODE_ENABLED
MAKETRAIT(is_float, float, true)
#endif

template<class T>
struct is_float { static const bool value = false; };

MAKETRAIT(is_float, float, true)
MAKETRAIT(is_float, double, true)

#define MAKETRAIT0(is_t) \
template<class T> \
struct is_t { static const bool value = false; };

MAKETRAIT0(is_ptr)
MAKETRAIT(is_ptr, nullptr_t, true)
MAKETRAIT(is_ptr, void*, true)

}