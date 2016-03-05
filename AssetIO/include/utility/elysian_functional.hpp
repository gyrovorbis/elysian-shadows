#ifndef ELYSIAN_FUNCTIONAL_HPP
#define ELYSIAN_FUNCTIONAL_HPP


// ======== CONVERT LAMBDA TO STD::FUNCTION SEAMLESSLY =======

template <typename ...T>
struct typelist{};

//function traits for function objects, lambdas, std::functions
template <typename T>
struct func_traits
{
    using ret_type=typename func_traits<decltype(&T::operator())>::ret_type;
    using arg_types=typename func_traits<decltype(&T::operator())>::arg_types;
};

//function traits for C function pointers
template<typename R, typename ...ARGS>
struct func_traits<R(*)(ARGS...)> {
    using ret_type=typelist<R>;
    using arg_types=typelist<ARGS...>;
};

//function traits for const C++ member functions
template <typename R, typename C, typename ...ARGS>
struct func_traits<R (C::*) (ARGS...) const>
{
    using ret_type=typelist<R>;
    using arg_types=typelist<ARGS...>;
};

//function traits for nonconst C++ member functions
template <typename R, typename C, typename ...ARGS>
struct func_traits<R (C::*) (ARGS...)>
{
    using ret_type=typelist<R>;
    using arg_types=typelist<ARGS...>;
};

template <typename R, typename ...ARGS, typename T>
auto make_function(T fun, typelist<R>, typelist <ARGS...>) -> ::std::function<R(ARGS...)>
{
    return ::std::function<R(ARGS...)>(fun);
}

template <typename T>
auto make_function(T fun) -> decltype(make_function(fun, typename func_traits<T>::ret_type{}, typename func_traits<T>::arg_types{}))
{
    return make_function(fun,typename func_traits<T>::ret_type{}, typename func_traits<T>::arg_types{});
}


// ====================



template<unsigned...> struct index_tuple{};

template<unsigned I, typename IndexTuple, typename... Types>
struct make_indices_impl;

template<unsigned I, unsigned... Indices, typename T, typename... Types>
struct make_indices_impl<I, index_tuple<Indices...>, T, Types...>
{
  typedef typename
    make_indices_impl<I + 1,
                      index_tuple<Indices..., I>,
                      Types...>::type type;
};

template<unsigned I, unsigned... Indices>
struct make_indices_impl<I, index_tuple<Indices...> >
{
  typedef index_tuple<Indices...> type;
};

template<typename... Types>
struct make_indices
  : make_indices_impl<0, index_tuple<>, Types...>
{};



//needs to support lambdas, C ptrs, and std::functions with no extra erasure shit

#if 0
template <unsigned... Indices, class... Args, class Ret>
static Ret forward_impl(index_tuple<Indices...>,
                 std::tuple<Args...> tuple,
                 const std::function<Ret(Args...)>& closure)
{
  return closure(std::get<Indices>(tuple)...);
}


template<typename Ret, class... Args>
static Ret forward_tuple_to_func_as_args(const std::tuple<Args...>& tuple, const std::function<Ret(Args...)>& closure)
{
   typedef typename make_indices<Args...>::type Indices;
   return forward_impl(Indices(), tuple, closure);
}

#endif


#include <utility>
#include <tuple>




namespace meta {

#if 0





#endif

// detects callable objects, not functions
// http://stackoverflow.com/questions/15393938/find-out-if-a-c-object-is-callable
template<typename T>
class is_callable_object {
private:
    using yes = char(&)[1];
    using no  = char(&)[2];

    struct Fallback { void operator()(); };
    struct Derived : T, Fallback { };

    template<typename U, U> struct Check;

    template<typename>
    static yes test(...);

    template<typename C>
    static no test(Check<void (Fallback::*)(), &C::operator()>*);

public:
    static const bool value = sizeof(test<Derived>(0)) == sizeof(yes);
};

template <typename T>
static constexpr auto is_callable_object_v = is_callable_object<T>::value;

// function traits

inline namespace detail {

template <typename ReturnType, typename ClassType, typename ... Args>
struct mem_function_traits {
    static constexpr auto arity = sizeof...(Args);

    using result_type = ReturnType;
    using class_type = ClassType;
    static constexpr auto valid = true;

    template <size_t i>
    struct arg {
        using type = std::tuple_element_t<i, std::tuple<Args...>>;
    };

    template <size_t i>
    using arg_t = typename arg<i>::type;
};

template <typename ReturnType, typename ... Args>
struct reg_function_traits {
    static constexpr auto arity = sizeof...(Args);

    using result_type = ReturnType;
    static constexpr auto valid = true;

    template <size_t i>
    struct arg {
        using type = decltype(std::get<i>(std::tuple<Args...>()));
    };

    template <size_t i>
    using arg_t = typename arg<i>::type;
};

} // namespace detail

// no types defined
template <typename T, typename = void, typename = void>
struct function_traits {
    static constexpr auto valid = false;
};

// generic functions
template <typename T>
struct function_traits<T,
    std::enable_if_t<std::is_class<T>::value>,
    std::enable_if_t<is_callable_object_v<T>>>
    : public function_traits<decltype(&T::operator())>
{ };

template <typename T>
struct function_traits<T,
    std::enable_if_t<std::is_reference<T>::value>>
    : public function_traits<typename std::remove_reference_t<T>>
{ };

// const member functions
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const,
    std::enable_if_t<std::is_member_function_pointer<ReturnType(ClassType::*)(Args...)>::value>>
    : public mem_function_traits<ReturnType, ClassType, Args...>
{ };

// member functions
template <typename ReturnType, typename ClassType, typename ... Args>
struct function_traits<ReturnType(ClassType::*)(Args...),
    std::enable_if_t<std::is_member_function_pointer<ReturnType(ClassType::*)(Args...)>::value>>
    : public mem_function_traits<ReturnType, ClassType, Args...>
{ };

// regular functions
template <typename ReturnType, typename ... Args>
struct function_traits<ReturnType(Args...),
    std::enable_if_t<std::is_function<ReturnType(Args...)>::value>>
    : public reg_function_traits<ReturnType, Args...>
{ };

// is_callable

template <typename T>
struct is_callable : public std::integral_constant<bool, function_traits<T>::valid> {};

// result_of

template <typename T>
struct result_of {
    using type = std::result_of_t<T>;
};

template <typename R, typename... Args>
struct result_of<R(*)(Args...)> {
    using type = R;
};

template <typename R, typename... Args>
struct result_of<R(&)(Args...)> {
    using type = R;
};

template <typename R, typename C, typename ... Args>
struct result_of<R(C::*)(Args...)> {
    using type = R;
};

template <typename R, typename C, typename ... Args>
struct result_of<R(C::*)(Args...) const> {
    using type = R;
};

} // namespace meta




#if 0

template <typename F, typename = void>
struct forward_tuple_impl {
    template <typename TUPLE, size_t... I, typename = void>
    auto operator()(F&& f, TUPLE&& tuple, std::index_sequence<I...>) {
        return f(std::get<I>(std::forward<TUPLE>(tuple))...);
    }
};


template <typename F>
struct forward_tuple_impl<F, std::enable_if_t<std::is_member_function_pointer<F>::value>> {
    template <typename ClassPtrType, typename... TupleArgs, size_t... I>
    auto operator()(F&& f, std::tuple<ClassPtrType, TupleArgs...>&& tuple, std::index_sequence<0, I...>) {
        static_assert(
            std::is_same<std::remove_cv_t<typename meta::function_traits<F>::class_type*>, std::remove_cv_t<ClassPtrType>>::value,
            "First argument in parameter pack must be a pointer to an class with member function F");

        return (std::get<0>(tuple)->*f)(std::get<I>(std::forward<std::tuple<ClassPtrType, TupleArgs...>>(tuple))...);
    }
};


template <typename F>
struct forward_tuple_impl<F, std::enable_if_t<std::is_member_function_pointer<F>::value>> {
    template <typename ClassPtrType, typename TUPLE, size_t... I>
    auto operator()(F&& f, TUPLE&& tuple, std::index_sequence<0, I...>) {
        static_assert(
            std::is_same<std::remove_cv_t<typename meta::function_traits<F>::class_type*>, std::remove_cv_t<decltype(std::get<0>(tuple))>>::value,
            "First argument in parameter pack must be a pointer to an class with member function F");

        return (std::get<0>(tuple)->*f)(std::get<I>(std::forward<TUPLE>(tuple))...);
    }
};

#endif

template <typename F,
          typename  = std::enable_if_t<!std::is_member_function_pointer<F>::value>,
          typename TUPLE,
          size_t... I>
constexpr auto forward_tuple_impl(F&& f, TUPLE&& tuple, std::index_sequence<I...>) {
    return f(std::get<I>(std::forward<TUPLE>(tuple))...);
}

template <typename    F,
          typename    TUPLE,
          size_t...   I>
constexpr auto forward_tuple_impl(F&& f, TUPLE&& tuple, std::index_sequence<0, I...>, typename std::enable_if<std::is_member_function_pointer<F>::value>::type* = nullptr) {
   static_assert(
        std::is_same<std::remove_cv_t<typename meta::function_traits<F>::class_type*>,
               std::remove_cv_t<std::remove_pointer_t<typename std::tuple_element<0, std::remove_reference_t<decltype(tuple)> >::type >>* >::value,
               "First argument in parameter pack must be a pointer to the class containing the member function F!");

    return (std::get<0>(std::forward<TUPLE>(tuple))->*f)(std::get<I>(std::forward<TUPLE>(tuple))...);
}


template<typename F, typename TUPLE>
constexpr auto forward_tuple(F&& f, TUPLE&& args) {
    return forward_tuple_impl(
        std::forward<F>(f),
        std::forward<TUPLE>(args),
        std::make_index_sequence<std::tuple_size<std::decay_t<TUPLE>>{}>{}
    );
}



#if 0
template<typename F, typename... Args>
constexpr auto forward_tuple(F&& f, std::tuple<Args...>&& args) {
    return forward_tuple_impl<F>()(
        std::forward<F>(f),
        std::forward<std::tuple<Args...>>(args),
        std::index_sequence_for<Args...>{}
    );
}

template <typename F, typename... Args>
class bound_function {
public:
    bound_function(F f, Args... args)
        : _f(std::move(f))
        , _args(std::forward<Args>(args)...)
    {}

    auto operator()(){
        return forward_tuple(std::move(_f), std::move(_args));
    }

private:
    F _f;
    std::tuple<Args...> _args;
};

template <typename... Args>
auto bind(Args&&... args) {
    return bound_function<Args...>(std::forward<Args>(args)...);
}
#endif

#if 0
template <typename F, typename... TupleArgs, size_t... I>
constexpr auto forward_tuple_impl(F&& f, std::tuple<TupleArgs...>&& tuple, std::index_sequence<I...>) {
    return f(std::get<I>(std::forward<std::tuple<TupleArgs...>>(tuple))...);
}

template<typename F, typename... Args>
constexpr auto forward_tuple(F&& f, std::tuple<Args...>&& args) {
    return forward_tuple_impl(
        std::forward<F>(f),
        std::forward<std::tuple<Args...>>(args),
        std::index_sequence_for<Args...>{}
    );
}

#endif

//GET RID OF ALL INDICES SHIT AND START USING
//std::index_sequences
//Make sure to use PERFECT FORWARDING EVERYWHERE WITH STD::FORWARD!
//c++14 auto return types, so shit isn't fucking hideous with -> decltypes
//need a FOREACH lambda iteration scheme for performing someting on every std::tuple element (pulling Lua arguments)


//forward std::functions, C functions, Lambdas
template <unsigned... Indices, class... Args, class Ret, class T>
static Ret forward_impl(index_tuple<Indices...>,
                 std::tuple<Args...> tuple,
                 const T& function,
                 typelist<Ret>)
{
  return function(std::get<Indices>(tuple)...);
}


template<typename T, typename ...ARGS>
auto forward_tuple_to_func_as_args(const T& func, const std::tuple<ARGS...>& argTuple)
    -> decltype(forward_impl(typename make_indices<ARGS...>::type{}, argTuple, func, typename func_traits<T>::ret_type{})) {
    typedef typename make_indices<ARGS...>::type Indices;
    return forward_impl(Indices(), argTuple, func, typename func_traits<T>::ret_type{});
}

//Forward NONCONST C++ member functions
template <unsigned... Indices, class... Args, class Ret, class C>
static Ret forward_impl(index_tuple<Indices...>,
                 std::tuple<C*, Args...> args,
                 Ret (C::*&mfuncptr) (Args...),
                 C*const obj,
                 typelist<Ret>)
{
  return (obj->*mfuncptr)(std::get<Indices>(args)...);
}


template<typename RET, typename C, typename ...ARGS>
auto forward_tuple_to_func_as_args(RET (C::*mfuncptr)(ARGS...), std::tuple<C*, ARGS...>& argTuple)
    -> decltype(forward_impl(typename make_indices_impl<1, index_tuple<>, ARGS...>::type{}, argTuple, mfuncptr, std::get<0>(argTuple), typename func_traits<decltype(mfuncptr)>::ret_type{}))
{
    //indices are created at an offset of 1, since the first argument is actually used as the this ptr and isn't passed to the function
    typedef typename make_indices_impl<1, index_tuple<>, ARGS...>::type Indices;
    return forward_impl(Indices(), argTuple, mfuncptr, std::get<0>(argTuple), typename func_traits<decltype(mfuncptr)>::ret_type{});
}

//forward CONST C++ member functions
template <unsigned... Indices, class... Args, class Ret, class C>
static Ret forward_impl(index_tuple<Indices...>,
                 std::tuple<const C*, Args...> args,
                 Ret (C::*&mfuncptr) (Args...) const,
                 const C*const obj,
                 typelist<Ret>)
{
  return (obj->*mfuncptr)(std::get<Indices>(args)...);
}


template<typename RET, typename C, typename ...ARGS>
auto forward_tuple_to_func_as_args(RET (C::*mfuncptr)(ARGS...) const, std::tuple<const C*, ARGS...>& argTuple)
    -> decltype(forward_impl(typename make_indices_impl<1, index_tuple<>, ARGS...>::type{}, argTuple, mfuncptr, std::get<0>(argTuple), typename func_traits<decltype(mfuncptr)>::ret_type{}))
{
    typedef typename make_indices_impl<1, index_tuple<>, ARGS...>::type Indices;
    return forward_impl(Indices(), argTuple, mfuncptr, std::get<0>(argTuple), typename func_traits<decltype(mfuncptr)>::ret_type{});
}

template<std::size_t> struct int_{};








#endif // ELYSIAN_FUNCTIONAL_HPP
