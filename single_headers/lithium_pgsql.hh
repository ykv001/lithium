// Author: Matthieu Garrigues matthieu.garrigues@gmail.com
//
// Single header version the lithium_pgsql library.
// https://github.com/matt-42/lithium
//
// This file is generated do not edit it.

#pragma once

#include <string>
#include <any>
#include <thread>
#include <iostream>
#include <sstream>
#include <optional>
#include <vector>
#include <deque>
#include <mutex>
#include <map>
#include <cstring>
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <libpq-fe.h>
#include <utility>
#include <atomic>
#include <tuple>
#include <unistd.h>
#include <cassert>
#include <arpa/inet.h>
#include <memory>


#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_PGSQL
#define LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_PGSQL


#include "libpq-fe.h"


#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_CALLABLE_TRAITS_CALLABLE_TRAITS
#define LITHIUM_SINGLE_HEADER_GUARD_LI_CALLABLE_TRAITS_CALLABLE_TRAITS

namespace li {

template <typename... T> struct typelist {};

namespace internal {
template <typename T> struct has_parenthesis_operator {
  template <typename C> static char test(decltype(&C::operator()));
  template <typename C> static int test(...);
  static const bool value = sizeof(test<T>(0)) == 1;
};

} // namespace internal

// Traits on callable (function, functors and lambda functions).

// callable_traits<F>::is_callable = true_type if F is callable.
// callable_traits<F>::arity = N if F takes N arguments.
// callable_traits<F>::arguments_tuple_type = tuple<Arg1, ..., ArgN>

template <typename F, typename X = void> struct callable_traits {
  typedef std::false_type is_callable;
  static const int arity = 0;
  typedef std::tuple<> arguments_tuple;
  typedef typelist<> arguments_list;
  typedef void return_type;
};

template <typename F, typename X> struct callable_traits<F&, X> : public callable_traits<F, X> {};
template <typename F, typename X> struct callable_traits<F&&, X> : public callable_traits<F, X> {};
template <typename F, typename X>
struct callable_traits<const F&, X> : public callable_traits<F, X> {};

template <typename F>
struct callable_traits<F, std::enable_if_t<internal::has_parenthesis_operator<F>::value>> {
  typedef callable_traits<decltype(&F::operator())> super;
  typedef std::true_type is_callable;
  static const int arity = super::arity;
  typedef typename super::arguments_tuple arguments_tuple;
  typedef typename super::arguments_list arguments_list;
  typedef typename super::return_type return_type;
};

template <typename C, typename R, typename... ARGS>
struct callable_traits<R (C::*)(ARGS...) const> {
  typedef std::true_type is_callable;
  static const int arity = sizeof...(ARGS);
  typedef std::tuple<ARGS...> arguments_tuple;
  typedef typelist<ARGS...> arguments_list;
  typedef R return_type;
};

template <typename C, typename R, typename... ARGS> struct callable_traits<R (C::*)(ARGS...)> {
  typedef std::true_type is_callable;
  static const int arity = sizeof...(ARGS);
  typedef std::tuple<ARGS...> arguments_tuple;
  typedef typelist<ARGS...> arguments_list;
  typedef R return_type;
};

template <typename R, typename... ARGS> struct callable_traits<R(ARGS...)> {
  typedef std::true_type is_callable;
  static const int arity = sizeof...(ARGS);
  typedef std::tuple<ARGS...> arguments_tuple;
  typedef typelist<ARGS...> arguments_list;
  typedef R return_type;
};

template <typename R, typename... ARGS> struct callable_traits<R (*)(ARGS...)> {
  typedef std::true_type is_callable;
  static const int arity = sizeof...(ARGS);
  typedef std::tuple<ARGS...> arguments_tuple;
  typedef typelist<ARGS...> arguments_list;
  typedef R return_type;
};

template <typename F>
using callable_arguments_tuple_t = typename callable_traits<F>::arguments_tuple;
template <typename F> using callable_arguments_list_t = typename callable_traits<F>::arguments_list;
template <typename F> using callable_return_type_t = typename callable_traits<F>::return_type;

template <typename F> struct is_callable : public callable_traits<F>::is_callable {};

template <typename F, typename... A> struct callable_with {
  template <typename G, typename... B>
  static char test(int x,
                   std::remove_reference_t<decltype(std::declval<G>()(std::declval<B>()...))>* = 0);
  template <typename G, typename... B> static int test(...);
  static const bool value = sizeof(test<F, A...>(0)) == 1;
};

} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_CALLABLE_TRAITS_CALLABLE_TRAITS

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_METAMAP
#define LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_METAMAP


namespace li {

namespace internal {
struct {
  template <typename A, typename... B> constexpr auto operator()(A&& a, B&&... b) {
    auto result = a;
    using expand_variadic_pack = int[];
    (void)expand_variadic_pack{0, ((result += b), 0)...};
    return result;
  }
} reduce_add;

} // namespace internal

template <typename... Ms> struct metamap;

template <typename F, typename... M> decltype(auto) find_first(metamap<M...>&& map, F fun);

template <typename... Ms> struct metamap;

template <typename M1, typename... Ms> struct metamap<M1, Ms...> : public M1, public Ms... {
  typedef metamap<M1, Ms...> self;
  // Constructors.
  inline metamap() = default;
  inline metamap(self&&) = default;
  inline metamap(const self&) = default;
  self& operator=(const self&) = default;
  self& operator=(self&&) = default;

  // metamap(self& other)
  //  : metamap(const_cast<const self&>(other)) {}

  inline metamap(typename M1::_iod_value_type&& m1, typename Ms::_iod_value_type&&... members) : M1{m1}, Ms{std::forward<typename Ms::_iod_value_type>(members)}... {}
  inline metamap(M1&& m1, Ms&&... members) : M1(m1), Ms(std::forward<Ms>(members))... {}
  inline metamap(const M1& m1, const Ms&... members) : M1(m1), Ms((members))... {}

  // Assignemnt ?

  // Retrive a value.
  template <typename K> decltype(auto) operator[](K k) { return symbol_member_access(*this, k); }

  template <typename K> decltype(auto) operator[](K k) const {
    return symbol_member_access(*this, k);
  }
};

template <> struct metamap<> {
  typedef metamap<> self;
  // Constructors.
  inline metamap() = default;
  // inline metamap(self&&) = default;
  inline metamap(const self&) = default;
  // self& operator=(const self&) = default;

  // metamap(self& other)
  //  : metamap(const_cast<const self&>(other)) {}

  // Assignemnt ?

  // Retrive a value.
  template <typename K> decltype(auto) operator[](K k) { return symbol_member_access(*this, k); }

  template <typename K> decltype(auto) operator[](K k) const {
    return symbol_member_access(*this, k);
  }
};

template <typename... Ms> constexpr auto size(metamap<Ms...>) { return sizeof...(Ms); }

template <typename M> struct metamap_size_t {};
template <typename... Ms> struct metamap_size_t<metamap<Ms...>> {
  enum { value = sizeof...(Ms) };
};
template <typename M> constexpr int metamap_size() {
  return metamap_size_t<std::decay_t<M>>::value;
}

template <typename... Ks> decltype(auto) metamap_values(const metamap<Ks...>& map) {
  return std::forward_as_tuple(map[typename Ks::_iod_symbol_type()]...);
}

template <typename K, typename M> constexpr auto has_key(M&& map, K k) {
  return decltype(has_member(map, k)){};
}

template <typename M, typename K> constexpr auto has_key(K k) {
  return decltype(has_member(std::declval<M>(), std::declval<K>())){};
}

template <typename M, typename K> constexpr auto has_key() {
  return decltype(has_member(std::declval<M>(), std::declval<K>())){};
}

template <typename K, typename M, typename O> constexpr auto get_or(M&& map, K k, O default_) {
  if constexpr (has_key<M, decltype(k)>()) {
    return map[k];
  } else
    return default_;
}

template <typename X> struct is_metamap {
  enum { value = false };
};
template <typename... M> struct is_metamap<metamap<M...>> {
  enum { value = true };
};

} // namespace li

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_CAT
#define LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_CAT

namespace li {

template <typename... T, typename... U>
inline decltype(auto) cat(const metamap<T...>& a, const metamap<U...>& b) {
  return metamap<T..., U...>(*static_cast<const T*>(&a)..., *static_cast<const U*>(&b)...);
}

} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_CAT

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_INTERSECTION
#define LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_INTERSECTION

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_MAKE_METAMAP_SKIP
#define LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_MAKE_METAMAP_SKIP

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_MAKE
#define LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_MAKE

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_SYMBOL_AST
#define LITHIUM_SINGLE_HEADER_GUARD_LI_SYMBOL_AST


namespace li {

template <typename E> struct Exp {};

template <typename E> struct array_subscriptable;

template <typename E> struct callable;

template <typename E> struct assignable;

template <typename E> struct array_subscriptable;

template <typename M, typename... A>
struct function_call_exp : public array_subscriptable<function_call_exp<M, A...>>,
                           public callable<function_call_exp<M, A...>>,
                           public assignable<function_call_exp<M, A...>>,
                           public Exp<function_call_exp<M, A...>> {
  using assignable<function_call_exp<M, A...>>::operator=;

  function_call_exp(const M& m, A&&... a) : method(m), args(std::forward<A>(a)...) {}

  M method;
  std::tuple<A...> args;
};

template <typename O, typename M>
struct array_subscript_exp : public array_subscriptable<array_subscript_exp<O, M>>,
                             public callable<array_subscript_exp<O, M>>,
                             public assignable<array_subscript_exp<O, M>>,
                             public Exp<array_subscript_exp<O, M>> {
  using assignable<array_subscript_exp<O, M>>::operator=;

  array_subscript_exp(const O& o, const M& m) : object(o), member(m) {}

  O object;
  M member;
};

template <typename L, typename R> struct assign_exp : public Exp<assign_exp<L, R>> {
  typedef L left_t;
  typedef R right_t;

  // template <typename V>
  // assign_exp(L l, V&& r) : left(l), right(std::forward<V>(r)) {}
  // template <typename V>
  inline assign_exp(L l, R r) : left(l), right(r) {}
  // template <typename V>
  // inline assign_exp(L l, const V& r) : left(l), right(r) {}

  L left;
  R right;
};

template <typename E> struct array_subscriptable {
public:
  // Member accessor
  template <typename S> constexpr auto operator[](S&& s) const {
    return array_subscript_exp<E, S>(*static_cast<const E*>(this), std::forward<S>(s));
  }
};

template <typename E> struct callable {
public:
  // Direct call.
  template <typename... A> constexpr auto operator()(A&&... args) const {
    return function_call_exp<E, A...>(*static_cast<const E*>(this), std::forward<A>(args)...);
  }
};

template <typename E> struct assignable {
public:
  template <typename L> auto operator=(L&& l) const {
    return assign_exp<E, L>(static_cast<const E&>(*this), std::forward<L>(l));
  }

  template <typename L> auto operator=(L&& l) {
    return assign_exp<E, L>(static_cast<E&>(*this), std::forward<L>(l));
  }

  template <typename T> auto operator=(const std::initializer_list<T>& l) const {
    return assign_exp<E, std::vector<T>>(static_cast<const E&>(*this), std::vector<T>(l));
  }
};

#define iod_query_declare_binary_op(OP, NAME)                                                      \
  template <typename A, typename B>                                                                \
  struct NAME##_exp : public assignable<NAME##_exp<A, B>>, public Exp<NAME##_exp<A, B>> {          \
    using assignable<NAME##_exp<A, B>>::operator=;                                                 \
    NAME##_exp() {}                                                                                \
    NAME##_exp(A&& a, B&& b) : lhs(std::forward<A>(a)), rhs(std::forward<B>(b)) {}                 \
    typedef A lhs_type;                                                                            \
    typedef B rhs_type;                                                                            \
    lhs_type lhs;                                                                                  \
    rhs_type rhs;                                                                                  \
  };                                                                                               \
  template <typename A, typename B>                                                                \
  inline std::enable_if_t<std::is_base_of<Exp<A>, A>::value || std::is_base_of<Exp<B>, B>::value,  \
                          NAME##_exp<A, B>>                                                        \
  operator OP(const A& b, const B& a) {                                                            \
    return NAME##_exp<std::decay_t<A>, std::decay_t<B>>{b, a};                                     \
  }

iod_query_declare_binary_op(+, plus);
iod_query_declare_binary_op(-, minus);
iod_query_declare_binary_op(*, mult);
iod_query_declare_binary_op(/, div);
iod_query_declare_binary_op(<<, shiftl);
iod_query_declare_binary_op(>>, shiftr);
iod_query_declare_binary_op(<, inf);
iod_query_declare_binary_op(<=, inf_eq);
iod_query_declare_binary_op(>, sup);
iod_query_declare_binary_op(>=, sup_eq);
iod_query_declare_binary_op(==, eq);
iod_query_declare_binary_op(!=, neq);
iod_query_declare_binary_op(&, logical_and);
iod_query_declare_binary_op (^, logical_xor);
iod_query_declare_binary_op(|, logical_or);
iod_query_declare_binary_op(&&, and);
iod_query_declare_binary_op(||, or);

#undef iod_query_declare_binary_op

} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_SYMBOL_AST

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_SYMBOL_SYMBOL
#define LITHIUM_SINGLE_HEADER_GUARD_LI_SYMBOL_SYMBOL


namespace li {

template <typename S>
class symbol : public assignable<S>,
               public array_subscriptable<S>,
               public callable<S>,
               public Exp<S> {};
} // namespace li

#ifdef LI_SYMBOL
#undef LI_SYMBOL
#endif

#define LI_SYMBOL(NAME)                                                                            \
  namespace s {                                                                                    \
  struct NAME##_t : li::symbol<NAME##_t> {                                                         \
                                                                                                   \
    using assignable<NAME##_t>::operator=;                                                         \
                                                                                                   \
    inline constexpr bool operator==(NAME##_t) { return true; }                                    \
    template <typename T> inline constexpr bool operator==(T) { return false; }                    \
                                                                                                   \
    template <typename V> struct variable_t {                                                      \
      typedef NAME##_t _iod_symbol_type;                                                           \
      typedef V _iod_value_type;                                                                   \
      V NAME;                                                                                      \
    };                                                                                             \
                                                                                                   \
    template <typename T, typename... A>                                                           \
    static inline decltype(auto) symbol_method_call(T&& o, A... args) {                            \
      return o.NAME(args...);                                                                      \
    }                                                                                              \
    template <typename T, typename... A> static inline auto& symbol_member_access(T&& o) {         \
      return o.NAME;                                                                               \
    }                                                                                              \
    template <typename T>                                                                          \
    static constexpr auto has_getter(int)                                                          \
        -> decltype(std::declval<T>().NAME(), std::true_type{}) {                                  \
      return {};                                                                                   \
    }                                                                                              \
    template <typename T> static constexpr auto has_getter(long) { return std::false_type{}; }     \
    template <typename T>                                                                          \
    static constexpr auto has_member(int) -> decltype(std::declval<T>().NAME, std::true_type{}) {  \
      return {};                                                                                   \
    }                                                                                              \
    template <typename T> static constexpr auto has_member(long) { return std::false_type{}; }     \
                                                                                                   \
    static inline auto symbol_string() { return #NAME; }                                           \
  };                                                                                               \
  static constexpr NAME##_t NAME;                                                                  \
  }

namespace li {

template <typename S> inline decltype(auto) make_variable(S s, char const v[]) {
  typedef typename S::template variable_t<const char*> ret;
  return ret{v};
}

template <typename V, typename S> inline decltype(auto) make_variable(S s, V v) {
  typedef typename S::template variable_t<std::remove_const_t<std::remove_reference_t<V>>> ret;
  return ret{v};
}

template <typename K, typename V> inline decltype(auto) make_variable_reference(K s, V&& v) {
  typedef typename K::template variable_t<V> ret;
  return ret{v};
}

template <typename T, typename S, typename... A>
static inline decltype(auto) symbol_method_call(T&& o, S, A... args) {
  return S::symbol_method_call(o, std::forward<A>(args)...);
}

template <typename T, typename S> static inline decltype(auto) symbol_member_access(T&& o, S) {
  return S::symbol_member_access(o);
}

template <typename T, typename S> constexpr auto has_member(T&& o, S) {
  return S::template has_member<T>(0);
}
template <typename T, typename S> constexpr auto has_member() {
  return S::template has_member<T>(0);
}

template <typename T, typename S> constexpr auto has_getter(T&& o, S) {
  return decltype(S::template has_getter<T>(0)){};
}
template <typename T, typename S> constexpr auto has_getter() {
  return decltype(S::template has_getter<T>(0)){};
}

template <typename S, typename T> struct CANNOT_FIND_REQUESTED_MEMBER_IN_TYPE {};

template <typename T, typename S> decltype(auto) symbol_member_or_getter_access(T&& o, S) {
  if constexpr (has_getter<T, S>()) {
    return symbol_method_call(o, S{});
  } else if constexpr (has_member<T, S>()) {
    return symbol_member_access(o, S{});
  } else {
    return CANNOT_FIND_REQUESTED_MEMBER_IN_TYPE<S, T>::error;
  }
}

template <typename S> auto symbol_string(symbol<S> v) { return S::symbol_string(); }

template <typename V> auto symbol_string(V v, typename V::_iod_symbol_type* = 0) {
  return V::_iod_symbol_type::symbol_string();
}
} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_SYMBOL_SYMBOL


namespace li {

template <typename... Ms> struct metamap;

namespace internal {

template <typename S, typename V> decltype(auto) exp_to_variable_ref(const assign_exp<S, V>& e) {
  return make_variable_reference(S{}, e.right);
}

template <typename S, typename V> decltype(auto) exp_to_variable(const assign_exp<S, V>& e) {
  typedef std::remove_const_t<std::remove_reference_t<V>> vtype;
  return make_variable(S{}, e.right);
}

template <typename S> decltype(auto) exp_to_variable(const symbol<S>& e) {
  return exp_to_variable(S() = int());
}

template <typename... T> inline decltype(auto) make_metamap_helper(T&&... args) {
  return metamap<T...>(std::forward<T>(args)...);
}

} // namespace internal

// Store copies of values in the map
static struct {
  template <typename... T> inline decltype(auto) operator()(T&&... args) const {
    // Copy values.
    return internal::make_metamap_helper(internal::exp_to_variable(std::forward<T>(args))...);
  }
} mmm;

// Store references of values in the map
template <typename... T> inline decltype(auto) make_metamap_reference(T&&... args) {
  // Keep references.
  return internal::make_metamap_helper(internal::exp_to_variable_ref(std::forward<T>(args))...);
}

} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_MAKE


namespace li {

struct skip {};
static struct {

  template <typename... M, typename... T>
  inline decltype(auto) run(metamap<M...> map, skip, T&&... args) const {
    return run(map, std::forward<T>(args)...);
  }

  template <typename T1, typename... M, typename... T>
  inline decltype(auto) run(metamap<M...> map, T1&& a, T&&... args) const {
    return run(
        cat(map, internal::make_metamap_helper(internal::exp_to_variable(std::forward<T1>(a)))),
        std::forward<T>(args)...);
  }

  template <typename... M> inline decltype(auto) run(metamap<M...> map) const { return map; }

  template <typename... T> inline decltype(auto) operator()(T&&... args) const {
    // Copy values.
    return run(metamap<>{}, std::forward<T>(args)...);
  }

} make_metamap_skip;

} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_MAKE_METAMAP_SKIP

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_MAP_REDUCE
#define LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_MAP_REDUCE

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_TUPLE_UTILS
#define LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_TUPLE_UTILS


namespace li {

template <typename... E, typename F> void apply_each(F&& f, E&&... e) {
  (void)std::initializer_list<int>{((void)f(std::forward<E>(e)), 0)...};
}

template <typename... E, typename F, typename R>
auto tuple_map_reduce_impl(F&& f, R&& reduce, E&&... e) {
  return reduce(f(std::forward<E>(e))...);
}

template <typename T, typename F> void tuple_map(T&& t, F&& f) {
  return std::apply([&](auto&&... e) { apply_each(f, std::forward<decltype(e)>(e)...); },
                    std::forward<T>(t));
}

template <typename T, typename F> auto tuple_reduce(T&& t, F&& f) {
  return std::apply(std::forward<F>(f), std::forward<T>(t));
}

template <typename T, typename F, typename R>
decltype(auto) tuple_map_reduce(T&& m, F map, R reduce) {
  auto fun = [&](auto... e) { return tuple_map_reduce_impl(map, reduce, e...); };
  return std::apply(fun, m);
}

template <typename F> inline std::tuple<> tuple_filter_impl() { return std::make_tuple(); }

template <typename F, typename... M, typename M1> auto tuple_filter_impl(M1 m1, M... m) {
  if constexpr (std::is_same<M1, F>::value)
    return tuple_filter_impl<F>(m...);
  else
    return std::tuple_cat(std::make_tuple(m1), tuple_filter_impl<F>(m...));
}

template <typename F, typename... M> auto tuple_filter(const std::tuple<M...>& m) {

  auto fun = [](auto... e) { return tuple_filter_impl<F>(e...); };
  return std::apply(fun, m);
}

} // namespace li
#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_TUPLE_UTILS


namespace li {

// Map a function(key, value) on all kv pair
template <typename... M, typename F> void map(const metamap<M...>& m, F fun) {
  auto apply = [&](auto key) -> decltype(auto) { return fun(key, m[key]); };

  apply_each(apply, typename M::_iod_symbol_type{}...);
}

// Map a function(key, value) on all kv pair. Ensure that the calling order
// is kept.
// template <typename O, typename F>
// void map_sequential2(F fun, O& obj)
// {}
// template <typename O, typename M1, typename... M, typename F>
// void map_sequential2(F fun, O& obj, M1 m1, M... ms)
// {
//   auto apply = [&] (auto key) -> decltype(auto)
//     {
//       return fun(key, obj[key]);
//     };

//   apply(m1);
//   map_sequential2(fun, obj, ms...);
// }
// template <typename... M, typename F>
// void map_sequential(const metamap<M...>& m, F fun)
// {
//   auto apply = [&] (auto key) -> decltype(auto)
//     {
//       return fun(key, m[key]);
//     };

//   map_sequential2(fun, m, typename M::_iod_symbol_type{}...);
// }

// Map a function(key, value) on all kv pair (non const).
template <typename... M, typename F> void map(metamap<M...>& m, F fun) {
  auto apply = [&](auto key) -> decltype(auto) { return fun(key, m[key]); };

  apply_each(apply, typename M::_iod_symbol_type{}...);
}

template <typename... E, typename F, typename R> auto apply_each2(F&& f, R&& r, E&&... e) {
  return r(f(std::forward<E>(e))...);
  //(void)std::initializer_list<int>{
  //  ((void)f(std::forward<E>(e)), 0)...};
}

// Map a function(key, value) on all kv pair an reduce
// all the results value with the reduce(r1, r2, ...) function.
template <typename... M, typename F, typename R>
decltype(auto) map_reduce(const metamap<M...>& m, F map, R reduce) {
  auto apply = [&](auto key) -> decltype(auto) {
    // return map(key, std::forward<decltype(m[key])>(m[key]));
    return map(key, m[key]);
  };

  return apply_each2(apply, reduce, typename M::_iod_symbol_type{}...);
  // return reduce(apply(typename M::_iod_symbol_type{})...);
}

// Map a function(key, value) on all kv pair an reduce
// all the results value with the reduce(r1, r2, ...) function.
template <typename... M, typename R> decltype(auto) reduce(const metamap<M...>& m, R reduce) {
  return reduce(m[typename M::_iod_symbol_type{}]...);
}

} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_MAP_REDUCE



namespace li {

template <typename... T, typename... U>
inline decltype(auto) intersection(const metamap<T...>& a, const metamap<U...>& b) {
  return map_reduce(a,
                    [&](auto k, auto&& v) -> decltype(auto) {
                      if constexpr (has_key<metamap<U...>, decltype(k)>()) {
                        return k = std::forward<decltype(v)>(v);
                      } else
                        return skip{};
                    },
                    make_metamap_skip);
}

} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_INTERSECTION

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_SUBSTRACT
#define LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_SUBSTRACT


namespace li {

template <typename... T, typename... U>
inline auto substract(const metamap<T...>& a, const metamap<U...>& b) {
  return map_reduce(a,
                    [&](auto k, auto&& v) {
                      if constexpr (!has_key<metamap<U...>, decltype(k)>()) {
                        return k = std::forward<decltype(v)>(v);
                      } else
                        return skip{};
                    },
                    make_metamap_skip);
}

} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_ALGORITHMS_SUBSTRACT


#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_METAMAP_METAMAP

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_SQL_COMMON
#define LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_SQL_COMMON


namespace li {
struct sql_blob : public std::string {
  using std::string::string;
  using std::string::operator=;

  sql_blob() : std::string() {}
};

struct sql_null_t {};
static sql_null_t null;

template <unsigned SIZE> struct sql_varchar : public std::string {
  using std::string::string;
  using std::string::operator=;

  sql_varchar() : std::string() {}
};
} // namespace li
#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_SQL_COMMON

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_SYMBOLS
#define LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_SYMBOLS

#ifndef LI_SYMBOL_ATTR
#define LI_SYMBOL_ATTR
    LI_SYMBOL(ATTR)
#endif

#ifndef LI_SYMBOL_after_insert
#define LI_SYMBOL_after_insert
    LI_SYMBOL(after_insert)
#endif

#ifndef LI_SYMBOL_after_remove
#define LI_SYMBOL_after_remove
    LI_SYMBOL(after_remove)
#endif

#ifndef LI_SYMBOL_after_update
#define LI_SYMBOL_after_update
    LI_SYMBOL(after_update)
#endif

#ifndef LI_SYMBOL_auto_increment
#define LI_SYMBOL_auto_increment
    LI_SYMBOL(auto_increment)
#endif

#ifndef LI_SYMBOL_before_insert
#define LI_SYMBOL_before_insert
    LI_SYMBOL(before_insert)
#endif

#ifndef LI_SYMBOL_before_remove
#define LI_SYMBOL_before_remove
    LI_SYMBOL(before_remove)
#endif

#ifndef LI_SYMBOL_before_update
#define LI_SYMBOL_before_update
    LI_SYMBOL(before_update)
#endif

#ifndef LI_SYMBOL_charset
#define LI_SYMBOL_charset
    LI_SYMBOL(charset)
#endif

#ifndef LI_SYMBOL_computed
#define LI_SYMBOL_computed
    LI_SYMBOL(computed)
#endif

#ifndef LI_SYMBOL_database
#define LI_SYMBOL_database
    LI_SYMBOL(database)
#endif

#ifndef LI_SYMBOL_host
#define LI_SYMBOL_host
    LI_SYMBOL(host)
#endif

#ifndef LI_SYMBOL_id
#define LI_SYMBOL_id
    LI_SYMBOL(id)
#endif

#ifndef LI_SYMBOL_password
#define LI_SYMBOL_password
    LI_SYMBOL(password)
#endif

#ifndef LI_SYMBOL_port
#define LI_SYMBOL_port
    LI_SYMBOL(port)
#endif

#ifndef LI_SYMBOL_primary_key
#define LI_SYMBOL_primary_key
    LI_SYMBOL(primary_key)
#endif

#ifndef LI_SYMBOL_read_access
#define LI_SYMBOL_read_access
    LI_SYMBOL(read_access)
#endif

#ifndef LI_SYMBOL_read_only
#define LI_SYMBOL_read_only
    LI_SYMBOL(read_only)
#endif

#ifndef LI_SYMBOL_synchronous
#define LI_SYMBOL_synchronous
    LI_SYMBOL(synchronous)
#endif

#ifndef LI_SYMBOL_user
#define LI_SYMBOL_user
    LI_SYMBOL(user)
#endif

#ifndef LI_SYMBOL_validate
#define LI_SYMBOL_validate
    LI_SYMBOL(validate)
#endif

#ifndef LI_SYMBOL_write_access
#define LI_SYMBOL_write_access
    LI_SYMBOL(write_access)
#endif


#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_SYMBOLS

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_PGSQL_STATEMENT
#define LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_PGSQL_STATEMENT



namespace li {


struct pgsql_statement_data : std::enable_shared_from_this<pgsql_statement_data> {
  pgsql_statement_data(const std::string& s) : stmt_name(s) {}
  std::string stmt_name;
};

template <typename Y>
struct pgsql_statement {

  // Wait for the next result.
  PGresult* wait_for_next_result() {
    //std::cout << "WAIT ======================" << std::endl;
    while (true)
    {
      if (PQconsumeInput(connection_) == 0)
        throw std::runtime_error(std::string("PQconsumeInput() failed: ") + PQerrorMessage(connection_));

      if (PQisBusy(connection_))
      {
        //std::cout << "isbusy" << std::endl;
        try {
          yield_();
        } catch (typename Y::exception_type& e) {
          // Yield thrown a exception (probably because a closed connection).
          // Mark the connection as broken because it is left in a undefined state.
          *connection_status_ = 1;
          throw std::move(e);
        }
      }
      else 
      {
        //std::cout << "notbusy" << std::endl;
        PGresult* res =  PQgetResult(connection_);
        if (PQresultStatus(res) == PGRES_FATAL_ERROR and PQerrorMessage(connection_)[0] != 0)
          throw std::runtime_error(std::string("Postresql fatal error:") + PQerrorMessage(connection_));
        else if (PQresultStatus(res) == PGRES_NONFATAL_ERROR)
          std::cerr << "Postgresql non fatal error: " << PQerrorMessage(connection_) << std::endl;
        return res;
      }
    }
  }

  void flush_results()
  {
    while (PGresult* res = wait_for_next_result())
      PQclear(res);
  }
  
  // Execute a simple request takes no arguments and return no rows.
  auto& operator()() {

    //std::cout << "sending " << data_.stmt_name.c_str() << std::endl;
    flush_results();
    if (!PQsendQueryPrepared(connection_, data_.stmt_name.c_str(), 0, nullptr, nullptr, nullptr, 1))
      throw std::runtime_error(std::string("Postresql error:") + PQerrorMessage(connection_));
    
    return *this;
  }


  // Execute a request with placeholders.
  template <unsigned N>
  void bind_param(sql_varchar<N>&& m, const char** values, int* lengths, int* binary)
  {
    //std::cout << "send param varchar " << m << std::endl;
    *values = m.c_str(); *lengths = m.size(); *binary = 0;
  }
  template <unsigned N>
  void bind_param(const sql_varchar<N>& m, const char** values, int* lengths, int* binary)
  {
    //std::cout << "send param const varchar " << m << std::endl;
    *values = m.c_str(); *lengths = m.size(); *binary = 0;
  }
  void bind_param(const char* m, const char** values, int* lengths, int* binary)
  {
    //std::cout << "send param const char*[N] " << m << std::endl;
    *values = m; *lengths = strlen(m); *binary = 0;
  }

  template <typename T>
  void bind_param(const std::vector<T>& m, const char** values, int* lengths, int* binary)
  {
    int tsize = [&] {
      if constexpr (is_metamap<T>::value) return metamap_size<T>();
      else return 1; }();

    int i = 0;
    for (int i = 0; i < m.size(); i++)
      bind_param(m[i], values + i * tsize, lengths + i * tsize, binary + i * tsize);
  }

  template <typename T>
  void bind_param(const T& m, const char** values, int* lengths, int* binary)
  { 
    if constexpr(is_metamap<std::decay_t<decltype(m)>>::value)
    {
      int i = 0;
      li::map(m, [&] (auto k, const auto& m) {
        bind_param(m, values + i, lengths + i, binary + i);
        i++;
      });
    }
    else
     if constexpr(std::is_same<std::decay_t<decltype(m)>, std::string>::value or
                  std::is_same<std::decay_t<decltype(m)>, std::string_view>::value)
    {
      //std::cout << "send param string: " << m << std::endl;
      *values = m.c_str();
      *lengths = m.size();
      *binary = 0;
    }
    else if constexpr(std::is_same<std::remove_reference_t<decltype(m)>, const char*>::value)
    {
      //std::cout << "send param const char* " << m << std::endl;
      *values = m;
      *lengths = strlen(m);
      *binary = 0;
    }
    else if constexpr(std::is_same<std::decay_t<decltype(m)>, int>::value)
    {
      *values = (char*)new int(htonl(m));
      *lengths = sizeof(m);
      *binary = 1;
    }
    else if constexpr(std::is_same<std::decay_t<decltype(m)>, long long int>::value)
    {
      // FIXME send 64bit values.
      //std::cout << "long long int param: " << m << std::endl;
      *values = (char*)new int(htonl(uint32_t(m)));
      *lengths = sizeof(uint32_t);
      // does not work:
      //values = (char*)new uint64_t(htobe64((uint64_t) m));
      //lengths = sizeof(uint64_t);
      *binary = 1;
    }
  }

  template <typename T>
  unsigned int bind_compute_nparam(const T& arg) { return 1; }
  template <typename... T>
  unsigned int bind_compute_nparam(const metamap<T...>& arg) { return sizeof...(T); }
  template <typename T>
  unsigned int bind_compute_nparam(const std::vector<T>& arg){ 
    return arg.size() * bind_compute_nparam(arg[0]);
  }

  // Bind parameter to the prepared statement and execute it.
  template <typename... T> auto& operator()(T&&... args) {

    unsigned int nparams = (bind_compute_nparam(std::forward<T>(args))+...); 
    const char* values_[nparams];
    int lengths_[nparams];
    int binary_[nparams];

    const char** values = values_;
    int* lengths = lengths_;
    int* binary = binary_;
    
    int i = 0;
    tuple_map(std::forward_as_tuple(args...), [&] (const auto& a) {
      bind_param(a, values + i, lengths + i, binary + i);
      i++;
    });

    flush_results();
    // std::cout << "sending " << data_.stmt_name.c_str() << " with " << nparams << " params" << std::endl;
    if (!PQsendQueryPrepared(connection_, data_.stmt_name.c_str(), nparams, values, lengths, binary, 1))
      throw std::runtime_error(std::string("Postresql error:") + PQerrorMessage(connection_));
    
    return *this;
  }


  //FIXME long long int affected_rows() { return pgsql_stmt_affected_rows(data_.stmt_); }

  // Fetch a string from a result field.
  template <typename... A> void fetch_impl(std::string& out, char* val, int length, bool is_binary) {
    //assert(!is_binary);
    //std::cout << "fetch string: " << length << " '"<< val <<"'" << std::endl;
    out = std::move(std::string(val, strlen(val)));
  }

  // Fetch a blob from a result field.
  template <typename... A> void fetch_impl(sql_blob& out, char* val, int length, bool is_binary) {
    //assert(is_binary);
    out = std::move(std::string(val, length));
  }

  // Fetch an int from a result field.
  void fetch_impl(int& out, char* val, int length, bool is_binary) {
    assert(is_binary);
    //std::cout << "fetch integer " << length << " " << is_binary << std::endl;
    // std::cout << "fetch integer " << be64toh(*((uint64_t *) val)) << std::endl;
    if (length == 8) 
    {
      // std::cout << "fetch 64b integer " << std::hex << int(32) << std::endl;
      // std::cout << "fetch 64b integer " << std::hex << uint64_t(*((uint64_t *) val)) << std::endl;
      // std::cout << "fetch 64b integer " << std::hex << (*((uint64_t *) val)) << std::endl;
      // std::cout << "fetch 64b integer " << std::hex << be64toh(*((uint64_t *) val)) << std::endl;
      out = be64toh(*((uint64_t *) val));
    }
    else if (length == 4) out = (uint32_t) ntohl(*((uint32_t *) val));
    else if (length == 2) out = (uint16_t) ntohs(*((uint16_t *) val));
    else assert(0);
  }

  // Fetch an unsigned int from a result field.
  void fetch_impl(unsigned int& out, char* val, int length, bool is_binary) {
    assert(is_binary);
    if (length == 8) out = be64toh(*((uint64_t *) val));
    else if (length == 4) out = ntohl(*((uint32_t *) val));
    else if (length == 2) out = ntohs(*((uint16_t *) val));
    else assert(0);
  }

  // Fetch a complete row in a metamap.
  template <typename... A> void fetch(PGresult* res, int row_i, metamap<A...>& o) {
    li::map(o, [&] (auto k, auto& m) {
      int field_i = PQfnumber(res, symbol_string(k));
      if (field_i == -1)
        throw std::runtime_error(std::string("postgresql errror : Field ") + symbol_string(k) + " not fount in result."); 

      fetch_impl(m, PQgetvalue(res, row_i, field_i),
                 PQgetlength(res, row_i, field_i), 
                 PQfformat(res, field_i));
    });
  }

  // Fetch a complete row in a tuple.
  template <typename... A> void fetch(PGresult* res, int row_i, std::tuple<A...>& o) {
    int field_i = 0;

    int nfields = PQnfields(res);
    if (nfields != sizeof...(A))
      throw std::runtime_error("postgresql error: in fetch: Mismatch between the request number of field and the outputs.");

    tuple_map(o, [&] (auto& m) {
      fetch_impl(m, PQgetvalue(res, row_i, field_i),
                 PQgetlength(res, row_i, field_i), 
                 PQfformat(res, field_i));
      field_i++;
    });
  }
  
  template <typename T> void fetch(PGresult* res, int row_i, T& o) {
    int field_i = 0;

    int nfields = PQnfields(res);
    if (nfields != 1)
      throw std::runtime_error("postgresql error: in fetch: Mismatch between the request number of field and the outputs.");
    fetch_impl(o, PQgetvalue(res, row_i, 0),
                 PQgetlength(res, row_i, 0), 
                 PQfformat(res, 0));
  }

  // Fetch one object (expect a resultset of maximum 1 row).
  // Return 0 if no rows, 1 if the fetch is ok.
  template <typename T> int fetch_one(T& o) {
    PGresult* res = wait_for_next_result();


    int nrows = PQntuples(res);
    if (nrows == 0)
      return 0;
    if (nrows > 1)
      throw std::runtime_error("postgresql error: in fetch_one: The request returned more than one row.");

    fetch(res, 0, o);
    PQclear(res);
    return 1;
  }

  // Read a single optional value.
  template <typename T> void read(std::optional<T>& o) {
    T t;
    if (fetch_one(t))
      o = std::optional<T>(t);
    else
      o = std::optional<T>();
  }

  // Read a single value.
  template <typename T> T read() {
    T t;
    if (!fetch_one(t))
      throw std::runtime_error("Request did not return any data.");
    return t;
  }

  // Read a single optional value.
  template <typename T> std::optional<T> read_optional() {
    T t;
    if (fetch_one(t))
      return std::optional<T>(t);
    else
      return std::optional<T>();
  }

  template <typename T>
  struct unconstref_tuple_elements {};
  template <typename... T>
  struct unconstref_tuple_elements<std::tuple<T...>> {
    typedef std::tuple<std::remove_const_t<std::remove_reference_t<T>>...> ret;
  };
  
  // Map a function to multiple rows.
  template <typename F> void map(F f) {
    typedef typename unconstref_tuple_elements<callable_arguments_tuple_t<F>>::ret tp;
    typedef std::remove_const_t<std::remove_reference_t<std::tuple_element_t<0, tp>>> T;

    while(PGresult* res = wait_for_next_result())
    {
      int nrows = PQntuples(res);
      for (int row_i = 0; row_i < nrows; row_i++)
      {
        if constexpr (li::is_metamap<T>::value) {
          T o;
          fetch(res, row_i, o);
          f(o);
        } else { // tuple version.
          tp o;
          fetch(res, row_i, o);
          std::apply(f, o);
        }
      }
      PQclear(res);
    }

  }

  // Get the last id of the row inserted by the last command.
  long long int last_insert_id() { 
    //while (PGresult* res = wait_for_next_result())
    //  PQclear(res);
    //PQsendQuery(connection_, "LASTVAL()");
    return this->read<int>();
    // PGresult *PQexec(connection_, const char *command);
    // this->operator()
    //   last_insert_id_ = PQoidValue(res);
    //   std::cout << "id " << last_insert_id_ << std::endl;
    //   PQclear(res);
    // }
    // return last_insert_id_; 
  }

  // Return true if the request returns an empty set.  
  bool empty() {
    PGresult* res = wait_for_next_result();
    int nrows = PQntuples(res);
    PQclear(res);
    return nrows = 0;
  }

  void wait () {
    while (PGresult* res = wait_for_next_result())
      PQclear(res);
  }

  PGconn* connection_;
  Y& yield_;
  pgsql_statement_data& data_;
  std::shared_ptr<int> connection_status_;
  int last_insert_id_ = -1;
};

}

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_PGSQL_STATEMENT

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_TYPE_HASHMAP
#define LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_TYPE_HASHMAP


namespace li {

template <typename V>
struct type_hashmap {

  template <typename E, typename F> E& get_cache_entry(int& hash, F)
  {
    // Init hash if needed.
    if (hash == -1)
    {
      std::lock_guard lock(mutex_);
      if (hash == -1)
        hash = counter_++;
    }
    // Init cache if miss.
    if (hash >= values_.size() or !values_[hash].has_value())
    {
      if (values_.size() < hash + 1)
        values_.resize(hash+1);
      values_[hash] = E();
    }

    // Return existing cache entry.
    return std::any_cast<E&>(values_[hash]);
  }
  template <typename K, typename F> V& operator()(F f, K key)
  {
    static int hash = -1;
    return this->template get_cache_entry<std::unordered_map<K, V>>(hash, f)[key];
  }

  template <typename F> V& operator()(F f)
  {
    static int hash = -1;
    return this->template get_cache_entry<V>(hash, f);
  }

private:
  static std::mutex mutex_;
  static int counter_;
  std::vector<std::any> values_;
};

template <typename V>
std::mutex type_hashmap<V>::mutex_;
template <typename V>
int type_hashmap<V>::counter_ = 0;

}

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_TYPE_HASHMAP



namespace li {

int max_pgsql_connections_per_thread = 200;
thread_local int total_number_of_pgsql_connections = 0;

struct pgsql_tag {};

struct pgsql_database;


struct pgsql_connection_data {

  ~pgsql_connection_data() {
    if (connection)
    {
      cancel();
      PQfinish(connection);
      // std::cerr << " DISCONNECT " << fd << std::endl;
      total_number_of_pgsql_connections--;
    }
  }
  void cancel() {
    if (connection)
    {
      // Cancel any pending request.
      PGcancel* cancel = PQgetCancel(connection);
      char x[256];
      if (cancel)
      {
        PQcancel(cancel, x, 256);
        PQfreeCancel(cancel);
      }
    }
  } 

  PGconn* connection = nullptr;
  int fd = -1;
  std::unordered_map<std::string, std::shared_ptr<pgsql_statement_data>> statements;
  type_hashmap<std::shared_ptr<pgsql_statement_data>> statements_hashmap;
};

thread_local std::deque<std::shared_ptr<pgsql_connection_data>> pgsql_connection_pool;

template <typename Y>
void pq_wait(Y& yield, PGconn* con)
{
  while (PQisBusy(con)) yield();
}


template <typename Y>
struct pgsql_connection {

  typedef pgsql_tag db_tag;

  inline pgsql_connection(const pgsql_connection&) = delete;
  inline pgsql_connection& operator=(const pgsql_connection&) = delete;
  inline pgsql_connection(pgsql_connection&&) = default;
  
  inline pgsql_connection(Y yield, std::shared_ptr<li::pgsql_connection_data> data)
      : yield_(yield), data_(data), stm_cache_(data->statements),
        connection_(data->connection){

    connection_status_ = std::shared_ptr<int>(new int(0), [data, yield](int* p) mutable {
      if (*p) 
      {
        assert(total_number_of_pgsql_connections >= 1);
        //yield.unsubscribe(data->fd);
        //std::cerr << "Discarding broken pgsql connection." << std::endl;
      }
      else
      {
         pgsql_connection_pool.push_back(data);
      }
    });
  }

  //FIXME long long int last_insert_rowid() { return pgsql_insert_id(connection_); }

  //pgsql_statement<Y> operator()(const std::string& rq) { return prepare(rq)(); }

  // Read request returning 1 scalar. Use prepared statement for more advanced read functions.
  template <typename T>
  T read()
  {
    PGresult* res = wait_for_next_result();
    return boost::lexical_cast<T>(PQgetvalue(res, 0, 0));
  }

  auto& operator()(const std::string& rq) {
    wait();
    if (!PQsendQuery(connection_, rq.c_str()))
      throw std::runtime_error(std::string("Postresql error:") + PQerrorMessage(connection_));
    return *this;
  }

  void wait () {
    while (PGresult* res = wait_for_next_result())
      PQclear(res);
  }

  PGresult* wait_for_next_result() {
    while (true)
    {
      if (PQconsumeInput(connection_) == 0)
        throw std::runtime_error(std::string("PQconsumeInput() failed: ") + PQerrorMessage(connection_));

      if (PQisBusy(connection_))
      {
        try {
          yield_();
        } catch (typename Y::exception_type& e) {
          // Yield thrown a exception (probably because a closed connection).
          // Mark the connection as broken because it is left in a undefined state.
          *connection_status_ = 1;
          throw std::move(e);
        }
      }
      else 
        return PQgetResult(connection_);
    }
  }

  template <typename F, typename... K>
  pgsql_statement<Y> cached_statement(F f, K... keys) {
    if (data_->statements_hashmap(f, keys...).get() == nullptr)
    {
      pgsql_statement<Y> res = prepare(f());
      data_->statements_hashmap(f, keys...) = res.data_.shared_from_this();
      return res;
    }
    else
      return pgsql_statement<Y>{connection_, yield_, 
                               *data_->statements_hashmap(f, keys...),
                               connection_status_};
  }

  pgsql_statement<Y> prepare(const std::string& rq) {
    auto it = stm_cache_.find(rq);
    if (it != stm_cache_.end())
    {
      //pgsql_wrapper_.pgsql_stmt_free_result(it->second->stmt_);
      //pgsql_wrapper_.pgsql_stmt_reset(it->second->stmt_);
      return pgsql_statement<Y>{connection_, yield_, *it->second, connection_status_};
    }
    std::stringstream stmt_name;
    stmt_name << (void*)connection_ << stm_cache_.size();
    //std::cout << "prepare " << rq << " NAME: " << stmt_name.str() << std::endl;

    while (PGresult* res = wait_for_next_result())
      PQclear(res);

    if (!PQsendPrepare(connection_, stmt_name.str().c_str(), rq.c_str(), 0, nullptr)) { 
      throw std::runtime_error(std::string("PQsendPrepare error") + PQerrorMessage(connection_)); 
    }
    // flush results.
    while(PGresult* ret = PQgetResult(connection_))
    {
      if (PQresultStatus(ret) == PGRES_FATAL_ERROR)
        throw std::runtime_error(std::string("Postresql fatal error:") + PQerrorMessage(connection_));
      if (PQresultStatus(ret) == PGRES_NONFATAL_ERROR)
        std::cerr << "Postgresql non fatal error: " << PQerrorMessage(connection_) << std::endl;
      PQclear(ret);
    }
    //pq_wait(yield_, connection_);

    auto pair = stm_cache_.emplace(rq, std::make_shared<pgsql_statement_data>(stmt_name.str()));
    return pgsql_statement<Y>{connection_, yield_, *pair.first->second, connection_status_};
  }

  template <typename T>
  inline std::string type_to_string(const T&, std::enable_if_t<std::is_integral<T>::value>* = 0) {
    return "INT";
  }
  template <typename T>
  inline std::string type_to_string(const T&,
                                    std::enable_if_t<std::is_floating_point<T>::value>* = 0) {
    return "DOUBLE";
  }
  inline std::string type_to_string(const std::string&) { return "TEXT"; }
  inline std::string type_to_string(const sql_blob&) { return "BLOB"; }
  template <unsigned S> inline std::string type_to_string(const sql_varchar<S>) {
    std::ostringstream ss;
    ss << "VARCHAR(" << S << ')';
    return ss.str();
  }

  Y yield_;
  std::shared_ptr<pgsql_connection_data> data_;
  std::unordered_map<std::string, std::shared_ptr<pgsql_statement_data>>& stm_cache_;
  PGconn* connection_;
  std::shared_ptr<int> connection_status_;
};


struct pgsql_database : std::enable_shared_from_this<pgsql_database> {

  template <typename... O> inline pgsql_database(O... opts) {

    auto options = mmm(opts...);
    static_assert(has_key(options, s::host), "open_pgsql_connection requires the s::host argument");
    static_assert(has_key(options, s::database),
                  "open_pgsql_connection requires the s::databaser argument");
    static_assert(has_key(options, s::user), "open_pgsql_connection requires the s::user argument");
    static_assert(has_key(options, s::password),
                  "open_pgsql_connection requires the s::password argument");

    host_ = options.host;
    database_ = options.database;
    user_ = options.user;
    passwd_ = options.password;
    port_ = get_or(options, s::port, 0);
    character_set_ = get_or(options, s::charset, "utf8");

    if (!PQisthreadsafe())
      throw std::runtime_error("LibPQ is not threadsafe.");
  }

  template <typename Y>
  inline pgsql_connection<Y> connect(Y yield) {

    //std::cout << "nconnection " << total_number_of_pgsql_connections << std::endl;
    std::shared_ptr<pgsql_connection_data> data = nullptr;
    while (!data)
    {

      if (!pgsql_connection_pool.empty()) {
        data = pgsql_connection_pool.back();
        pgsql_connection_pool.pop_back();
        yield.listen_to_fd(data->fd);
      }
      else
      {
        // std::cout << total_number_of_pgsql_connections << " connections. "<< std::endl;
        if (total_number_of_pgsql_connections >= max_pgsql_connections_per_thread)
        {
          //std::cout << "Waiting for a free pgsql connection..." << std::endl;
          yield();
          continue;
        }
        total_number_of_pgsql_connections++;
        PGconn* connection = nullptr;
        int pgsql_fd = -1;
        std::stringstream coninfo;
        coninfo << "postgresql://" << user_ << ":" << passwd_ << "@" << host_ << ":" << port_ << "/" << database_;
        // connection = PQconnectdb(coninfo.str().c_str());
        connection = PQconnectStart(coninfo.str().c_str());
        if (!connection)
        {
          std::cerr << "Warning: PQconnectStart returned null." << std::endl;
          total_number_of_pgsql_connections--;
          yield();
          continue;
        }
        if (PQsetnonblocking(connection, 1) == -1)
        {
          std::cerr << "Warning: PQsetnonblocking returned -1: " << PQerrorMessage(connection) << std::endl;
          PQfinish(connection);
          total_number_of_pgsql_connections--;
          yield();
          continue;
        }

        pgsql_fd = PQsocket(connection);
        if (pgsql_fd == -1)
        {
          std::cerr << "Warning: PQsocket returned -1: " << PQerrorMessage(connection) << std::endl;
          // If PQsocket return -1, retry later.
          PQfinish(connection);
          total_number_of_pgsql_connections--;
          yield();
          continue;
        }
        yield.listen_to_fd(pgsql_fd);

        int status = PQconnectPoll(connection);

        try
        {
          while (status != PGRES_POLLING_FAILED and status != PGRES_POLLING_OK)
          {
            yield();
            status = PQconnectPoll(connection);
          }
        } catch (typename Y::exception_type& e) {
          // Yield thrown a exception (probably because a closed connection).
          total_number_of_pgsql_connections--;
          PQfinish(connection);
          throw std::move(e);
        }
        //std::cout << "CONNECT " << total_number_of_pgsql_connections << std::endl;
        if (status != PGRES_POLLING_OK)
        {
          std::cerr << "Warning: cannot connect to the postgresql server " << host_  << ": " << PQerrorMessage(connection) << std::endl;
          std::cerr<< "thread allocated connection == " << total_number_of_pgsql_connections <<  std::endl;
          std::cerr<< "Maximum is " << max_pgsql_connections_per_thread <<  std::endl;
          total_number_of_pgsql_connections--;
          PQfinish(connection);
          yield();
          continue;
        }


        //pgsql_set_character_set(pgsql, character_set_.c_str());
        data = std::shared_ptr<pgsql_connection_data>(new pgsql_connection_data{connection, pgsql_fd});
      }
    }
    assert(data);
    return pgsql_connection(yield, data);
  }
  struct active_yield {
    typedef std::runtime_error exception_type;
    void operator()() {}
    void listen_to_fd(int) {}
    void unsubscribe(int) {}
  };

  inline pgsql_connection<active_yield> connect() {
    return connect(active_yield{});
  }

  std::mutex mutex_;
  std::string host_, user_, passwd_, database_;
  unsigned int port_;
  std::string character_set_;
};


} // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_PGSQL

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_SQL_ORM
#define LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_SQL_ORM

#ifndef LITHIUM_SINGLE_HEADER_GUARD_LI_HTTP_BACKEND_SYMBOLS
#define LITHIUM_SINGLE_HEADER_GUARD_LI_HTTP_BACKEND_SYMBOLS

#ifndef LI_SYMBOL_blocking
#define LI_SYMBOL_blocking
    LI_SYMBOL(blocking)
#endif

#ifndef LI_SYMBOL_create_secret_key
#define LI_SYMBOL_create_secret_key
    LI_SYMBOL(create_secret_key)
#endif

#ifndef LI_SYMBOL_date_thread
#define LI_SYMBOL_date_thread
    LI_SYMBOL(date_thread)
#endif

#ifndef LI_SYMBOL_hash_password
#define LI_SYMBOL_hash_password
    LI_SYMBOL(hash_password)
#endif

#ifndef LI_SYMBOL_https_cert
#define LI_SYMBOL_https_cert
    LI_SYMBOL(https_cert)
#endif

#ifndef LI_SYMBOL_https_key
#define LI_SYMBOL_https_key
    LI_SYMBOL(https_key)
#endif

#ifndef LI_SYMBOL_id
#define LI_SYMBOL_id
    LI_SYMBOL(id)
#endif

#ifndef LI_SYMBOL_linux_epoll
#define LI_SYMBOL_linux_epoll
    LI_SYMBOL(linux_epoll)
#endif

#ifndef LI_SYMBOL_name
#define LI_SYMBOL_name
    LI_SYMBOL(name)
#endif

#ifndef LI_SYMBOL_non_blocking
#define LI_SYMBOL_non_blocking
    LI_SYMBOL(non_blocking)
#endif

#ifndef LI_SYMBOL_nthreads
#define LI_SYMBOL_nthreads
    LI_SYMBOL(nthreads)
#endif

#ifndef LI_SYMBOL_one_thread_per_connection
#define LI_SYMBOL_one_thread_per_connection
    LI_SYMBOL(one_thread_per_connection)
#endif

#ifndef LI_SYMBOL_path
#define LI_SYMBOL_path
    LI_SYMBOL(path)
#endif

#ifndef LI_SYMBOL_primary_key
#define LI_SYMBOL_primary_key
    LI_SYMBOL(primary_key)
#endif

#ifndef LI_SYMBOL_read_only
#define LI_SYMBOL_read_only
    LI_SYMBOL(read_only)
#endif

#ifndef LI_SYMBOL_select
#define LI_SYMBOL_select
    LI_SYMBOL(select)
#endif

#ifndef LI_SYMBOL_server_thread
#define LI_SYMBOL_server_thread
    LI_SYMBOL(server_thread)
#endif

#ifndef LI_SYMBOL_session_id
#define LI_SYMBOL_session_id
    LI_SYMBOL(session_id)
#endif

#ifndef LI_SYMBOL_update_secret_key
#define LI_SYMBOL_update_secret_key
    LI_SYMBOL(update_secret_key)
#endif

#ifndef LI_SYMBOL_user_id
#define LI_SYMBOL_user_id
    LI_SYMBOL(user_id)
#endif


#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_HTTP_BACKEND_SYMBOLS



namespace li {

struct sqlite_tag;
struct mysql_tag;
struct pgsql_tag;

using s::auto_increment;
using s::primary_key;
using s::read_only;

template <typename SCHEMA, typename C> struct sql_orm {

  typedef decltype(std::declval<SCHEMA>().all_fields()) O;
  typedef O object_type;

  ~sql_orm() {

    //assert(0);
  }
  sql_orm(sql_orm&&) = default;
  sql_orm(const sql_orm&) = delete;
  sql_orm& operator=(const sql_orm&) = delete;

  sql_orm(SCHEMA& schema, C&& con) : schema_(schema), con_(std::forward<C>(con)) {}

  template <typename S, typename... A> void call_callback(S s, A&&... args) {
    if constexpr (has_key<decltype(schema_.get_callbacks())>(S{}))
      return schema_.get_callbacks()[s](args...);
  }

  inline auto& drop_table_if_exists() {
    con_(std::string("DROP TABLE IF EXISTS ") + schema_.table_name()).wait();
    return *this;
  }

  inline auto& create_table_if_not_exists() {
    std::ostringstream ss;
    ss << "CREATE TABLE if not exists " << schema_.table_name() << " (";

    bool first = true;
    li::tuple_map(schema_.all_info(), [&](auto f) {
      auto f2 = schema_.get_field(f);
      typedef decltype(f) F;
      typedef decltype(f2) F2;
      typedef typename F2::left_t K;
      typedef typename F2::right_t V;

      bool auto_increment = SCHEMA::template is_auto_increment<F>::value;
      bool primary_key = SCHEMA::template is_primary_key<F>::value;
      K k{};
      V v{};

      if (!first)
        ss << ", ";
      ss << li::symbol_string(k) << " ";
      
      if (!std::is_same<typename C::db_tag, pgsql_tag>::value or !auto_increment)
        ss << con_.type_to_string(v);

      if (std::is_same<typename C::db_tag, sqlite_tag>::value) {
        if (auto_increment || primary_key)
          ss << " PRIMARY KEY ";
      }

      if (std::is_same<typename C::db_tag, mysql_tag>::value) {
        if (auto_increment)
          ss << " AUTO_INCREMENT NOT NULL";
        if (primary_key)
          ss << " PRIMARY KEY ";
      }

      if (std::is_same<typename C::db_tag, pgsql_tag>::value) {
        if (auto_increment)
          ss << " SERIAL PRIMARY KEY ";
      }

      first = false;
    });
    ss << ");";
    try {
      con_(ss.str()).wait();
    } catch (std::runtime_error e) {
      std::cerr << "Warning: Lithium::sql could not create the " << schema_.table_name() << " sql table."
                << std::endl
                << "You can ignore this message if the table already exists."
                << "The sql error is: " << e.what() << std::endl;
    }
    return *this;
  }

  std::string placeholder_string() {

    if (std::is_same<typename C::db_tag, pgsql_tag>::value) {
      placeholder_pos_++;
      std::stringstream ss;
      ss << '$' << placeholder_pos_;
      return ss.str();
    }
    else return "?";
  }

  template <typename W> void where_clause(W&& cond, std::ostringstream& ss) {
    ss << " WHERE ";
    bool first = true;
    map(cond, [&](auto k, auto v) {
      if (!first)
        ss << " and ";
      first = false;
      ss << li::symbol_string(k) << " = " << placeholder_string();
    });
    ss << " ";
  }

  template <typename... W, typename... A> auto find_one(metamap<W...> where, A&&... cb_args) {

    auto stmt = con_.cached_statement([&] (){ 
        std::ostringstream ss;
        placeholder_pos_ = 0;
        ss << "SELECT ";
        bool first = true;
        O o;
        li::map(o, [&](auto k, auto v) {
          if (!first)
            ss << ",";
          first = false;
          ss << li::symbol_string(k);
        });

        ss << " FROM " << schema_.table_name();
        where_clause(where, ss);
        ss << "LIMIT 1";
        return ss.str();
    });

    auto res = li::tuple_reduce(metamap_values(where), stmt).template read_optional<O>();
    if (res)
      call_callback(s::read_access, *res, cb_args...);
    return res;
  }

  template <typename A, typename B, typename... O, typename... W>
  auto find_one(metamap<O...>&& o, assign_exp<A, B> w1, W... ws) {
    return find_one(cat(o, mmm(w1)), ws...);
  }
  template <typename A, typename B, typename... W> auto find_one(assign_exp<A, B> w1, W... ws) {
    return find_one(mmm(w1), ws...);
  }

  template <typename W> bool exists(W&& cond) {

    O o;
    auto stmt = con_.cached_statement([&] { 
        std::ostringstream ss;
        placeholder_pos_ = 0;
        ss << "SELECT count(*) FROM " << schema_.table_name();
        where_clause(cond, ss);
        ss << "LIMIT 1";
        return ss.str();
    });

    return li::tuple_reduce(metamap_values(cond), stmt).template read<int>();
  }

  template <typename A, typename B, typename... W> auto exists(assign_exp<A, B> w1, W... ws) {
    return exists(mmm(w1, ws...));
  }
  // Save a ll fields except auto increment.
  // The db will automatically fill auto increment keys.
  template <typename N, typename... A> auto insert(N&& o, A&&... cb_args) {

    auto values = schema_.without_auto_increment();
    map(o, [&](auto k, auto& v) { values[k] = o[k]; });

    call_callback(s::validate, values, cb_args...);
    call_callback(s::before_insert, values, cb_args...);


    auto stmt = con_.cached_statement([&] { 
        std::ostringstream ss;
        std::ostringstream vs;

        placeholder_pos_ = 0;
        ss << "INSERT into " << schema_.table_name() << "(";

        bool first = true;
        li::map(values, [&](auto k, auto v) {
          if (!first) {
            ss << ",";
            vs << ",";
          }
          first = false;
          ss << li::symbol_string(k);
          vs << placeholder_string();
        });

        ss << ") VALUES (" << vs.str() << ")";

        if (std::is_same<typename C::db_tag, pgsql_tag>::value &&
            has_key(schema_.all_fields(), s::id))
          ss << " returning id;";
        return ss.str();
    });

    auto request_res = li::reduce(values, stmt);

    call_callback(s::after_insert, o, cb_args...);

    if constexpr(has_key<decltype(schema_.all_fields())>(s::id))
      return request_res.last_insert_id();
    else return request_res.wait();
  };

  template <typename A, typename B, typename... O, typename... W>
  auto insert(metamap<O...>&& o, assign_exp<A, B> w1, W... ws) {
    return insert(cat(o, mmm(w1)), ws...);
  }
  template <typename A, typename B, typename... W>
  auto insert(assign_exp<A, B> w1, W... ws) {
    return insert(mmm(w1), ws...);
  }

  // template <typename S, typename V, typename... A>
  // long long int insert(const assign_exp<S, V>& a, A&&... tail) {
  //   auto m = mmm(a, tail...);
  //   return insert(m);
  // }

  // Iterate on all the rows of the table.
  template <typename F> void forall(F f) {

    typedef decltype(schema_.all_fields()) O;

    auto stmt = con_.cached_statement([&] { 
        std::ostringstream ss;
        placeholder_pos_ = 0;
        ss << "SELECT * from " << schema_.table_name();
        return ss.str();
    });
    stmt().map([&](const O& o) { f(o); });
  }

  // Update N's members except auto increment members.
  // N must have at least one primary key.
  // template <typename N, typename... CB> void bulk_update(const N& o, CB&&... args) {
  //   auto stmt = con_.cached_statement([&] { 

  //     // Select pk
  //     std::ostringstream ss;
  //     ss << "UPDATE World SET ";
      
  //     map(vector[0], [&](auto k, auto v) {
  //       if (!first)
  //         ss << ",";
  //       first = false;
  //       ss << li::symbol_string(k) << " = tmp." << li::symbol_string(k);
  //     });

  //     // randomNumber=tmp.randomNumber FROM (VALUES ";
  //     ss << " FROM (VALUES ";
  //     for (int i = 0; i < N; i++)
  //       ss << "($" << i*2+1 << "::integer, $" << i*2+2 << "::integer) "<< (i == N-1 ? "": ",");
  //     ss << ") AS tmp(id, randomNumber) WHERE tmp.id = World.id";
  //     return ss.str();
  //   });

  // }

  // Update N's members except auto increment members.
  // N must have at least one primary key.
  template <typename N, typename... CB> void update(const N& o, CB&&... args) {
    // check if N has at least one member of PKS.

    call_callback(s::validate, o, args...);
    call_callback(s::write_access, o, args...);
    call_callback(s::before_update, o, args...);

    // static_assert(metamap_size<decltype(intersect(o, schema_.read_only()))>(),
    //"You cannot give read only fields to the orm update method.");

    auto to_update = substract(o, schema_.read_only());
    auto pk = intersection(o, schema_.primary_key());

    auto stmt = con_.cached_statement([&] { 
        static_assert(metamap_size<decltype(pk)>() > 0,
                      "You must provide at least one primary key to update an object.");
        std::ostringstream ss;
        placeholder_pos_ = 0;
        ss << "UPDATE " << schema_.table_name() << " SET ";

        bool first = true;

        map(to_update, [&](auto k, auto v) {
          if (!first)
            ss << ",";
          first = false;
          ss << li::symbol_string(k) << " = " << placeholder_string();
        });

        where_clause(pk, ss);
        return ss.str();
    });

    li::tuple_reduce(std::tuple_cat(metamap_values(to_update), metamap_values(pk)), stmt);

    call_callback(s::after_update, o, args...);
  }

  template <typename A, typename B, typename... O, typename... W>
  void update(metamap<O...>&& o, assign_exp<A, B> w1, W... ws) {
    return update(cat(o, mmm(w1)), ws...);
  }
  template <typename A, typename B, typename... W> void update(assign_exp<A, B> w1, W... ws) {
    return update(mmm(w1), ws...);
  }

  inline int count() {
    return con_.prepare(std::string("SELECT count(*) from ") + schema_.table_name())().template read<int>();
  }

  template <typename N, typename... CB> void remove(const N& o, CB&&... args) {

    call_callback(s::before_remove, o, args...);

    auto stmt = con_.cached_statement([&] { 
      std::ostringstream ss;
      placeholder_pos_ = 0;
      ss << "DELETE from " << schema_.table_name() << " WHERE ";

      bool first = true;
      map(schema_.primary_key(), [&](auto k, auto v) {
        if (!first)
          ss << " and ";
        first = false;
        ss << li::symbol_string(k) << " = " << placeholder_string();
      });
      return ss.str();
    });

    auto pks = intersection(o, schema_.primary_key());
    li::reduce(pks, stmt);

    call_callback(s::after_remove, o, args...);
  }
  template <typename A, typename B, typename... O, typename... W>
  void remove(metamap<O...>&& o, assign_exp<A, B> w1, W... ws) {
    return remove(cat(o, mmm(w1)), ws...);
  }
  template <typename A, typename B, typename... W> void remove(assign_exp<A, B> w1, W... ws) {
    return remove(mmm(w1), ws...);
  }

  auto& schema() { return schema_; }

  C& backend_connection() { return con_; }

  SCHEMA schema_;
  C con_;
  int placeholder_pos_ = 0;
};

template <typename... F> struct orm_fields {

  orm_fields(F... fields) : fields_(fields...) {
    static_assert(sizeof...(F) == 0 || metamap_size<decltype(this->primary_key())>() != 0,
                  "You must give at least one primary key to the ORM. Use "
                  "s::your_field_name(s::primary_key) to add a primary_key");
  }

  // Field extractor.
  template <typename M> auto get_field(M m) { return m; }
  template <typename M, typename T> auto get_field(assign_exp<M, T> e) { return e; }
  template <typename M, typename T, typename... A>
  auto get_field(assign_exp<function_call_exp<M, A...>, T> e) {
    return assign_exp<M, T>{M{}, e.right};
  }

  // template <typename M> struct get_field { typedef M ret; };
  // template <typename M, typename T> struct get_field<assign_exp<M, T>> {
  //   typedef assign_exp<M, T> ret;
  //   static auto ctor() { return assign_exp<M, T>{M{}, T()}; }
  // };
  // template <typename M, typename T, typename... A>
  // struct get_field<assign_exp<function_call_exp<M, A...>, T>> : public get_field<assign_exp<M,
  // T>> {
  // };

// get_field<E>::ctor();
// field attributes checks.
#define CHECK_FIELD_ATTR(ATTR)                                                                     \
  template <typename M> struct is_##ATTR : std::false_type {};                                     \
  template <typename M, typename T, typename... A>                                                 \
  struct is_##ATTR<assign_exp<function_call_exp<M, A...>, T>>                                      \
      : std::disjunction<std::is_same<std::decay_t<A>, s::ATTR##_t>...> {};                        \
                                                                                                   \
  auto ATTR() {                                                                                    \
    return tuple_map_reduce(fields_,                                                               \
                            [this](auto e) {                                                       \
                              typedef std::remove_reference_t<decltype(e)> E;                      \
                              if constexpr (is_##ATTR<E>::value)                                   \
                                return get_field(e);                                               \
                              else                                                                 \
                                return skip{};                                                     \
                            },                                                                     \
                            make_metamap_skip);                                                    \
  }

  CHECK_FIELD_ATTR(primary_key);
  CHECK_FIELD_ATTR(read_only);
  CHECK_FIELD_ATTR(auto_increment);
  CHECK_FIELD_ATTR(computed);
#undef CHECK_FIELD_ATTR

  // Do not remove this comment, this is used by the symbol generation.
  // s::primary_key s::read_only s::auto_increment s::computed

  auto all_info() { return fields_; }

  auto all_fields() {

    return tuple_map_reduce(fields_,
                            [this](auto e) {
                              // typedef std::remove_reference_t<decltype(e)> E;
                              return get_field(e);
                            },
                            [](auto... e) { return mmm(e...); });
  }

  auto without_auto_increment() { return substract(all_fields(), auto_increment()); }
  auto all_fields_except_computed() {
    return substract(substract(all_fields(), computed()), auto_increment());
  }

  std::tuple<F...> fields_;
};

template <typename DB, typename MD = orm_fields<>, typename CB = decltype(mmm())>
struct sql_orm_schema : public MD {

  sql_orm_schema(DB& db, const std::string& table_name, CB cb = CB(), MD md = MD())
      : MD(md), database_(db), table_name_(table_name), callbacks_(cb) {}

  inline auto connect() { return sql_orm{*this, database_.connect()}; }
  template <typename Y>
  inline auto connect(Y& y) { return sql_orm{*this, database_.connect(y)}; }

  const std::string& table_name() const { return table_name_; }
  auto get_callbacks() const { return callbacks_; }

  template <typename... P> auto callbacks(P... params_list) const {
    auto cbs = mmm(params_list...);
    auto allowed_callbacks = mmm(s::before_insert, s::before_remove, s::before_update,
                                 s::after_insert, s::after_remove, s::after_update, s::validate);

    static_assert(
        metamap_size<decltype(substract(cbs, allowed_callbacks))>() == 0,
        "The only supported callbacks are: s::before_insert, s::before_remove, s::before_update,"
        " s::after_insert, s::after_remove, s::after_update, s::validate");
    return sql_orm_schema<DB, MD, decltype(cbs)>(database_, table_name_, cbs,
                                                 *static_cast<const MD*>(this));
  }

  template <typename... P> auto fields(P... p) const {
    return sql_orm_schema<DB, orm_fields<P...>, CB>(database_, table_name_, callbacks_,
                                                    orm_fields<P...>(p...));
  }

  DB& database_;
  std::string table_name_;
  CB callbacks_;
};

}; // namespace li

#endif // LITHIUM_SINGLE_HEADER_GUARD_LI_SQL_SQL_ORM

