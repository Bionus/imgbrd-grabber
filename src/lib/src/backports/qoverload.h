#ifndef BACKPORTS_QOVERLOAD_H
#define BACKPORTS_QOVERLOAD_H

#include <QtGlobal>


template <typename... Args>
struct QNonConstOverload
{
	template <typename R, typename T>
	Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...)) const Q_DECL_NOTHROW -> decltype(ptr)
	{ return ptr; }

	template <typename R, typename T>
	static Q_DECL_CONSTEXPR auto of(R (T::*ptr)(Args...)) Q_DECL_NOTHROW -> decltype(ptr)
	{ return ptr; }
};

template <typename... Args>
struct QConstOverload
{
	template <typename R, typename T>
	Q_DECL_CONSTEXPR auto operator()(R (T::*ptr)(Args...) const) const Q_DECL_NOTHROW -> decltype(ptr)
	{ return ptr; }

	template <typename R, typename T>
	static Q_DECL_CONSTEXPR auto of(R (T::*ptr)(Args...) const) Q_DECL_NOTHROW -> decltype(ptr)
	{ return ptr; }
};

template <typename... Args>
struct QOverload : QConstOverload<Args...>, QNonConstOverload<Args...>
{
	using QConstOverload<Args...>::of;
	using QConstOverload<Args...>::operator();
	using QNonConstOverload<Args...>::of;
	using QNonConstOverload<Args...>::operator();

	template <typename R>
	Q_DECL_CONSTEXPR auto operator()(R (*ptr)(Args...)) const Q_DECL_NOTHROW -> decltype(ptr)
	{ return ptr; }

	template <typename R>
	static Q_DECL_CONSTEXPR auto of(R (*ptr)(Args...)) Q_DECL_NOTHROW -> decltype(ptr)
	{ return ptr; }
};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304 // C++14
	template <typename... Args> Q_DECL_CONSTEXPR QOverload<Args...> qOverload Q_DECL_UNUSED = {};
	template <typename... Args> Q_DECL_CONSTEXPR QConstOverload<Args...> qConstOverload Q_DECL_UNUSED = {};
	template <typename... Args> Q_DECL_CONSTEXPR QNonConstOverload<Args...> qNonConstOverload Q_DECL_UNUSED = {};
#endif

#endif // BACKPORTS_QOVERLOAD_H
