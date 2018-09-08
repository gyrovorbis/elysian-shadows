#ifndef ELYSIAN_OOLUA_STRING_TRAITS_HPP
#define ELYSIAN_OOLUA_STRING_TRAITS_HPP

#include <oolua_traits.h>

namespace OOLUA {
#if 1
template<>
    struct maybe_null<const char*>
    {
        typedef INTERNAL::function_return<const char*> return_trait;
        typedef const char* type;
        typedef typename LVD::raw_type<const char*>::type raw;
        typedef typename INTERNAL::Pull_type_<raw, const char*
                                                , LVD::is_integral_type<raw>::value
                                                || INTERNAL::is_shared_type<type>::value
                                            >::type pull_type;
        enum { in = 0};
        enum { out = 1};
        enum { owner = No_change};
        enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
        enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant
                                        || INTERNAL::is_shared_const<type>::value };
        enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral
                                        || INTERNAL::is_shared_type<type>::value };
       // typedef char type_can_not_be_normal_integral [INTERNAL::Type_enum_defaults<type>::is_integral ? -1 : 1 ];
        typedef char type_can_not_be_normal_integral [1];
        typedef char type_has_to_be_by_reference_if_not_shared [!INTERNAL::is_shared_type<type>::value && is_by_value ? -1 : 1 ];
        typedef char type_can_not_be_just_a_reference_to_type_if_not_shared [!INTERNAL::is_shared_type<type>::value && LVD::is_same<raw&, type>::value ? -1 : 1];
        typedef char type_can_not_be_just_a_const_reference_to_type_if_not_shared [!INTERNAL::is_shared_type<type>::value && LVD::is_same<raw const&, type>::value ? -1 : 1];
        /*Reference to pointer:
        this could be valid in some situations, until such a time as it is required
        or requested disable it*/
        typedef char type_can_not_be_a_reference_to_ptr [ LVD::is_same<raw *&, type>::value ? -1 : 1];
        typedef char type_can_not_be_a_reference_to_const_ptr [ LVD::is_same<raw *const&, type>::value ? -1 : 1];
        typedef char type_can_not_be_a_reference_to_const_ptr_const [ LVD::is_same<raw const*const&, type>::value ? -1 : 1];
        typedef char type_can_not_be_a_reference_to_ptr_const [ LVD::is_same<raw const*&, type>::value ? -1 : 1];
    };
#else
    template<>
    struct maybe_null<const char*> {
        typedef const char* type;
        typedef typename LVD::raw_type<const char*>::type raw;
        typedef typename INTERNAL::Pull_type_<raw, const char*, LVD::is_integral_type<raw>::value >::type pull_type;
        enum { in = 0};
        enum { out = 1};
        enum { owner = No_change};
        enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
        enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
        enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral  };
        typedef char type_can_not_be_integral [ 1 ];
        typedef char type_has_to_be_by_reference [is_by_value ? -1 : 1 ];
        typedef char type_can_not_be_just_a_reference_to_type [	LVD::is_same<raw&, type>::value ? -1 : 1];
        typedef char type_can_not_be_just_a_const_reference_to_type [ LVD::is_same<raw const&, type>::value ? -1 : 1];
        /*Reference to pointer:
        this could be valid in some situations, until such a time as it is required
        or requested disable it*/
        typedef char type_can_not_be_a_reference_to_ptr [ LVD::is_same<raw *&, type>::value ? -1 : 1];
        typedef char type_can_not_be_a_reference_to_const_ptr [ LVD::is_same<raw *const&, type>::value ? -1 : 1];
        typedef char type_can_not_be_a_reference_to_const_ptr_const [ LVD::is_same<raw const*const&, type>::value ? -1 : 1];
        typedef char type_can_not_be_a_reference_to_ptr_const [ LVD::is_same<raw const*&, type>::value ? -1 : 1];
    };
#endif

}

#endif // ELYSIAN_OOLUA_STRING_TRAITS_HPP

