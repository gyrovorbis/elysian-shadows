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





namespace meta {


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
static const bool is_callable_object_v = is_callable_object<T>::value;

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



template <typename T, typename R_T, typename ARG_T>
struct is_stateless_impl: std::false_type {};

template <typename T, typename R_T, typename ...ARG_T>
struct is_stateless_impl<T,typelist<R_T>,typelist<ARG_T...>>:
    public std::is_convertible<T,typename std::add_pointer<R_T(ARG_T...)>::type>
{};

template <typename T>
struct is_stateless
{
    using ft=func_traits<T>;
    static constexpr bool value=is_stateless_impl<T,typename ft::ret_type,typename ft::arg_types>::value;
};


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
constexpr auto forward_tuple_impl(F&& f, TUPLE&& tuple, std::index_sequence<0, I...>, typename std::enable_if<std::is_member_function_pointer<F>::value>::type* = nullptr) noexcept {
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


namespace detail {

    template<typename T, typename F, size_t... I>
    constexpr auto foreach_in_tuple_impl(T&& tuple, F&& f, std::index_sequence<I...>) {
        auto l = { (f(std::get<I>(std::forward<T>(tuple))), 0)... };
        (void)l;

    }

}

template<typename T, typename F>
constexpr void foreach_in_tuple(T&& tuple, F&& f) {
    detail::foreach_in_tuple_impl(std::forward<T>(tuple),
                                  std::forward<F>(f),
                                  std::make_index_sequence<std::tuple_size<std::decay_t<T>>{}>{});
}

template<typename T, typename=std::integer_sequence<typename T::value_type>>
struct reverse_index_sequence;

template<typename T, T... list>
struct reverse_index_sequence<std::integer_sequence<T>, std::integer_sequence<T, list...>> :
    std::integer_sequence<T, list...>
{ };

template<typename T, T first, T... rest, T... done>
struct reverse_index_sequence<std::integer_sequence<T, first, rest...>, std::integer_sequence<T, done...>> :
    reverse_index_sequence<std::integer_sequence<T, rest...>, std::integer_sequence<T, first, done...>>
{ };

template<typename T, typename F>
constexpr void foreach_in_tuple_reverse(T&& tuple, F&& f) {
    detail::foreach_in_tuple_impl(std::forward<T>(tuple),
                                  std::forward<F>(f),
                                 reverse_index_sequence<std::make_index_sequence<std::tuple_size<std::decay_t<T>>{}>>{});

}





#endif // ELYSIAN_FUNCTIONAL_HPP
