/** 
	\file export_func_to_lua.h
	\date Sun Oct 27 00:12:09 2013
	\details 
	Configurable values as set when generating this file
	\li class_functions 60 - Maximum amount of class functions that can be registered for each proxied type	(Default 15)
	\note Warning this file was generated, edits to the file will not persist if it is regenerated.	
*/

#ifndef EXPORT_FUNC_TO_LUA_H_
#	define EXPORT_FUNC_TO_LUA_H_
/** \cond INTERNAL */

#define LUA_MEMBER_FUNC_1(Class,func1) {#func1, &Class::func1},
#define LUA_MEMBER_FUNC_2(Class,func1,func2) LUA_MEMBER_FUNC_1(Class,func1)  LUA_MEMBER_FUNC_1(Class,func2)
#define LUA_MEMBER_FUNC_3(Class,func1,func2,func3) LUA_MEMBER_FUNC_2(Class,func1,func2)  LUA_MEMBER_FUNC_1(Class,func3)
#define LUA_MEMBER_FUNC_4(Class,func1,func2,func3,func4) LUA_MEMBER_FUNC_3(Class,func1,func2,func3)  LUA_MEMBER_FUNC_1(Class,func4)
#define LUA_MEMBER_FUNC_5(Class,func1,func2,func3,func4,func5) LUA_MEMBER_FUNC_4(Class,func1,func2,func3,func4)  LUA_MEMBER_FUNC_1(Class,func5)
#define LUA_MEMBER_FUNC_6(Class,func1,func2,func3,func4,func5,func6) LUA_MEMBER_FUNC_5(Class,func1,func2,func3,func4,func5)  LUA_MEMBER_FUNC_1(Class,func6)
#define LUA_MEMBER_FUNC_7(Class,func1,func2,func3,func4,func5,func6,func7) LUA_MEMBER_FUNC_6(Class,func1,func2,func3,func4,func5,func6)  LUA_MEMBER_FUNC_1(Class,func7)
#define LUA_MEMBER_FUNC_8(Class,func1,func2,func3,func4,func5,func6,func7,func8) LUA_MEMBER_FUNC_7(Class,func1,func2,func3,func4,func5,func6,func7)  LUA_MEMBER_FUNC_1(Class,func8)
#define LUA_MEMBER_FUNC_9(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9) LUA_MEMBER_FUNC_8(Class,func1,func2,func3,func4,func5,func6,func7,func8)  LUA_MEMBER_FUNC_1(Class,func9)
#define LUA_MEMBER_FUNC_10(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10) LUA_MEMBER_FUNC_9(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9)  LUA_MEMBER_FUNC_1(Class,func10)
#define LUA_MEMBER_FUNC_11(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11) LUA_MEMBER_FUNC_10(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10)  LUA_MEMBER_FUNC_1(Class,func11)
#define LUA_MEMBER_FUNC_12(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12) LUA_MEMBER_FUNC_11(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11)  LUA_MEMBER_FUNC_1(Class,func12)
#define LUA_MEMBER_FUNC_13(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13) LUA_MEMBER_FUNC_12(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12)  LUA_MEMBER_FUNC_1(Class,func13)
#define LUA_MEMBER_FUNC_14(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14) LUA_MEMBER_FUNC_13(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13)  LUA_MEMBER_FUNC_1(Class,func14)
#define LUA_MEMBER_FUNC_15(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15) LUA_MEMBER_FUNC_14(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14)  LUA_MEMBER_FUNC_1(Class,func15)
#define LUA_MEMBER_FUNC_16(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16) LUA_MEMBER_FUNC_15(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15)  LUA_MEMBER_FUNC_1(Class,func16)
#define LUA_MEMBER_FUNC_17(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17) LUA_MEMBER_FUNC_16(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16)  LUA_MEMBER_FUNC_1(Class,func17)
#define LUA_MEMBER_FUNC_18(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18) LUA_MEMBER_FUNC_17(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17)  LUA_MEMBER_FUNC_1(Class,func18)
#define LUA_MEMBER_FUNC_19(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19) LUA_MEMBER_FUNC_18(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18)  LUA_MEMBER_FUNC_1(Class,func19)
#define LUA_MEMBER_FUNC_20(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20) LUA_MEMBER_FUNC_19(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19)  LUA_MEMBER_FUNC_1(Class,func20)
#define LUA_MEMBER_FUNC_21(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21) LUA_MEMBER_FUNC_20(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20)  LUA_MEMBER_FUNC_1(Class,func21)
#define LUA_MEMBER_FUNC_22(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22) LUA_MEMBER_FUNC_21(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21)  LUA_MEMBER_FUNC_1(Class,func22)
#define LUA_MEMBER_FUNC_23(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23) LUA_MEMBER_FUNC_22(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22)  LUA_MEMBER_FUNC_1(Class,func23)
#define LUA_MEMBER_FUNC_24(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24) LUA_MEMBER_FUNC_23(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23)  LUA_MEMBER_FUNC_1(Class,func24)
#define LUA_MEMBER_FUNC_25(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25) LUA_MEMBER_FUNC_24(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24)  LUA_MEMBER_FUNC_1(Class,func25)
#define LUA_MEMBER_FUNC_26(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26) LUA_MEMBER_FUNC_25(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25)  LUA_MEMBER_FUNC_1(Class,func26)
#define LUA_MEMBER_FUNC_27(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27) LUA_MEMBER_FUNC_26(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26)  LUA_MEMBER_FUNC_1(Class,func27)
#define LUA_MEMBER_FUNC_28(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28) LUA_MEMBER_FUNC_27(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27)  LUA_MEMBER_FUNC_1(Class,func28)
#define LUA_MEMBER_FUNC_29(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29) LUA_MEMBER_FUNC_28(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28)  LUA_MEMBER_FUNC_1(Class,func29)
#define LUA_MEMBER_FUNC_30(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30) LUA_MEMBER_FUNC_29(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29)  LUA_MEMBER_FUNC_1(Class,func30)
#define LUA_MEMBER_FUNC_31(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31) LUA_MEMBER_FUNC_30(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30)  LUA_MEMBER_FUNC_1(Class,func31)
#define LUA_MEMBER_FUNC_32(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32) LUA_MEMBER_FUNC_31(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31)  LUA_MEMBER_FUNC_1(Class,func32)
#define LUA_MEMBER_FUNC_33(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33) LUA_MEMBER_FUNC_32(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32)  LUA_MEMBER_FUNC_1(Class,func33)
#define LUA_MEMBER_FUNC_34(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34) LUA_MEMBER_FUNC_33(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33)  LUA_MEMBER_FUNC_1(Class,func34)
#define LUA_MEMBER_FUNC_35(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35) LUA_MEMBER_FUNC_34(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34)  LUA_MEMBER_FUNC_1(Class,func35)
#define LUA_MEMBER_FUNC_36(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36) LUA_MEMBER_FUNC_35(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35)  LUA_MEMBER_FUNC_1(Class,func36)
#define LUA_MEMBER_FUNC_37(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37) LUA_MEMBER_FUNC_36(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36)  LUA_MEMBER_FUNC_1(Class,func37)
#define LUA_MEMBER_FUNC_38(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38) LUA_MEMBER_FUNC_37(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37)  LUA_MEMBER_FUNC_1(Class,func38)
#define LUA_MEMBER_FUNC_39(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39) LUA_MEMBER_FUNC_38(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38)  LUA_MEMBER_FUNC_1(Class,func39)
#define LUA_MEMBER_FUNC_40(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40) LUA_MEMBER_FUNC_39(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39)  LUA_MEMBER_FUNC_1(Class,func40)
#define LUA_MEMBER_FUNC_41(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41) LUA_MEMBER_FUNC_40(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40)  LUA_MEMBER_FUNC_1(Class,func41)
#define LUA_MEMBER_FUNC_42(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42) LUA_MEMBER_FUNC_41(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41)  LUA_MEMBER_FUNC_1(Class,func42)
#define LUA_MEMBER_FUNC_43(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43) LUA_MEMBER_FUNC_42(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42)  LUA_MEMBER_FUNC_1(Class,func43)
#define LUA_MEMBER_FUNC_44(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44) LUA_MEMBER_FUNC_43(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43)  LUA_MEMBER_FUNC_1(Class,func44)
#define LUA_MEMBER_FUNC_45(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45) LUA_MEMBER_FUNC_44(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44)  LUA_MEMBER_FUNC_1(Class,func45)
#define LUA_MEMBER_FUNC_46(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46) LUA_MEMBER_FUNC_45(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45)  LUA_MEMBER_FUNC_1(Class,func46)
#define LUA_MEMBER_FUNC_47(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47) LUA_MEMBER_FUNC_46(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46)  LUA_MEMBER_FUNC_1(Class,func47)
#define LUA_MEMBER_FUNC_48(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48) LUA_MEMBER_FUNC_47(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47)  LUA_MEMBER_FUNC_1(Class,func48)
#define LUA_MEMBER_FUNC_49(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49) LUA_MEMBER_FUNC_48(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48)  LUA_MEMBER_FUNC_1(Class,func49)
#define LUA_MEMBER_FUNC_50(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50) LUA_MEMBER_FUNC_49(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49)  LUA_MEMBER_FUNC_1(Class,func50)
#define LUA_MEMBER_FUNC_51(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51) LUA_MEMBER_FUNC_50(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50)  LUA_MEMBER_FUNC_1(Class,func51)
#define LUA_MEMBER_FUNC_52(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52) LUA_MEMBER_FUNC_51(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51)  LUA_MEMBER_FUNC_1(Class,func52)
#define LUA_MEMBER_FUNC_53(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53) LUA_MEMBER_FUNC_52(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52)  LUA_MEMBER_FUNC_1(Class,func53)
#define LUA_MEMBER_FUNC_54(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54) LUA_MEMBER_FUNC_53(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53)  LUA_MEMBER_FUNC_1(Class,func54)
#define LUA_MEMBER_FUNC_55(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55) LUA_MEMBER_FUNC_54(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54)  LUA_MEMBER_FUNC_1(Class,func55)
#define LUA_MEMBER_FUNC_56(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55,func56) LUA_MEMBER_FUNC_55(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55)  LUA_MEMBER_FUNC_1(Class,func56)
#define LUA_MEMBER_FUNC_57(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55,func56,func57) LUA_MEMBER_FUNC_56(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55,func56)  LUA_MEMBER_FUNC_1(Class,func57)
#define LUA_MEMBER_FUNC_58(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55,func56,func57,func58) LUA_MEMBER_FUNC_57(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55,func56,func57)  LUA_MEMBER_FUNC_1(Class,func58)
#define LUA_MEMBER_FUNC_59(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55,func56,func57,func58,func59) LUA_MEMBER_FUNC_58(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55,func56,func57,func58)  LUA_MEMBER_FUNC_1(Class,func59)
#define LUA_MEMBER_FUNC_60(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55,func56,func57,func58,func59,func60) LUA_MEMBER_FUNC_59(Class,func1,func2,func3,func4,func5,func6,func7,func8,func9,func10,func11,func12,func13,func14,func15,func16,func17,func18,func19,func20,func21,func22,func23,func24,func25,func26,func27,func28,func29,func30,func31,func32,func33,func34,func35,func36,func37,func38,func39,func40,func41,func42,func43,func44,func45,func46,func47,func48,func49,func50,func51,func52,func53,func54,func55,func56,func57,func58,func59)  LUA_MEMBER_FUNC_1(Class,func60)

/// @def end the assigning of functions to the array
#define CLASS_LIST_MEMBERS_END {0,0}};}

/// @def define the constants in the class, which are the the class name and the member function array
#define CLASS_LIST_MEMBERS_START_OOLUA_NON_CONST(Class)\
namespace OOLUA { \
char const OOLUA::Proxy_class< Class >::class_name[] = #Class;\
OOLUA::Proxy_class< Class >::Reg_type OOLUA::Proxy_class< Class >::class_methods[]={

#define CLASS_LIST_MEMBERS_START_OOLUA_CONST(Class)\
namespace OOLUA { \
char const OOLUA::Proxy_class< Class >::class_name_const[] = #Class "_const";\
OOLUA::Proxy_class< Class >::Reg_type_const OOLUA::Proxy_class< Class >::class_methods_const[]={

#define EXPORT_OOLUA_FUNCTIONS_0_(mod,Class)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_1_(mod,Class,p0)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_1(OOLUA::Proxy_class< Class > ,p0)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_2_(mod,Class,p0,p1)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_2(OOLUA::Proxy_class< Class > ,p0,p1)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_3_(mod,Class,p0,p1,p2)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_3(OOLUA::Proxy_class< Class > ,p0,p1,p2)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_4_(mod,Class,p0,p1,p2,p3)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_4(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_5_(mod,Class,p0,p1,p2,p3,p4)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_5(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_6_(mod,Class,p0,p1,p2,p3,p4,p5)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_6(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_7_(mod,Class,p0,p1,p2,p3,p4,p5,p6)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_7(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_8_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_8(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_9_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_9(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_10_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_10(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_11_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_11(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_12_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_12(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_13_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_13(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_14_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_14(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_15_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_15(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_16_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_16(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_17_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_17(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_18_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_18(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_19_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_19(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_20_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_20(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_21_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_21(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_22_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_22(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_23_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_23(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_24_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_24(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_25_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_25(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_26_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_26(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_27_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_27(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_28_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_28(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_29_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_29(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_30_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_30(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_31_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_31(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_32_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_32(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_33_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_33(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_34_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_34(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_35_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_35(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_36_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_36(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_37_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_37(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_38_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_38(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_39_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_39(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_40_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_40(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_41_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_41(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_42_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_42(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_43_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_43(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_44_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_44(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_45_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_45(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_46_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_46(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_47_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_47(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_48_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_48(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_49_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_49(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_50_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_50(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_51_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_51(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_52_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_52(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_53_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_53(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_54_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_54(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_55_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_55(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_56_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_56(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_57_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_57(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_58_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_58(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_59_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_59(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_60_(mod,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58,p59)\
	CLASS_LIST_MEMBERS_START_ ##mod (Class)\
	LUA_MEMBER_FUNC_60(OOLUA::Proxy_class< Class > ,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58,p59)\
	CLASS_LIST_MEMBERS_END

#define EXPORT_OOLUA_FUNCTIONS_0_CONST(Class) \
	EXPORT_OOLUA_FUNCTIONS_0_(OOLUA_CONST,Class)
#define EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Class) \
	EXPORT_OOLUA_FUNCTIONS_0_(OOLUA_NON_CONST,Class)
#define EXPORT_OOLUA_FUNCTIONS_1_CONST(Class,p0) \
	EXPORT_OOLUA_FUNCTIONS_1_(OOLUA_CONST,Class,p0)
#define EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(Class,p0) \
	EXPORT_OOLUA_FUNCTIONS_1_(OOLUA_NON_CONST,Class,p0)
#define EXPORT_OOLUA_FUNCTIONS_2_CONST(Class,p0,p1) \
	EXPORT_OOLUA_FUNCTIONS_2_(OOLUA_CONST,Class,p0,p1)
#define EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Class,p0,p1) \
	EXPORT_OOLUA_FUNCTIONS_2_(OOLUA_NON_CONST,Class,p0,p1)
#define EXPORT_OOLUA_FUNCTIONS_3_CONST(Class,p0,p1,p2) \
	EXPORT_OOLUA_FUNCTIONS_3_(OOLUA_CONST,Class,p0,p1,p2)
#define EXPORT_OOLUA_FUNCTIONS_3_NON_CONST(Class,p0,p1,p2) \
	EXPORT_OOLUA_FUNCTIONS_3_(OOLUA_NON_CONST,Class,p0,p1,p2)
#define EXPORT_OOLUA_FUNCTIONS_4_CONST(Class,p0,p1,p2,p3) \
	EXPORT_OOLUA_FUNCTIONS_4_(OOLUA_CONST,Class,p0,p1,p2,p3)
#define EXPORT_OOLUA_FUNCTIONS_4_NON_CONST(Class,p0,p1,p2,p3) \
	EXPORT_OOLUA_FUNCTIONS_4_(OOLUA_NON_CONST,Class,p0,p1,p2,p3)
#define EXPORT_OOLUA_FUNCTIONS_5_CONST(Class,p0,p1,p2,p3,p4) \
	EXPORT_OOLUA_FUNCTIONS_5_(OOLUA_CONST,Class,p0,p1,p2,p3,p4)
#define EXPORT_OOLUA_FUNCTIONS_5_NON_CONST(Class,p0,p1,p2,p3,p4) \
	EXPORT_OOLUA_FUNCTIONS_5_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4)
#define EXPORT_OOLUA_FUNCTIONS_6_CONST(Class,p0,p1,p2,p3,p4,p5) \
	EXPORT_OOLUA_FUNCTIONS_6_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5)
#define EXPORT_OOLUA_FUNCTIONS_6_NON_CONST(Class,p0,p1,p2,p3,p4,p5) \
	EXPORT_OOLUA_FUNCTIONS_6_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5)
#define EXPORT_OOLUA_FUNCTIONS_7_CONST(Class,p0,p1,p2,p3,p4,p5,p6) \
	EXPORT_OOLUA_FUNCTIONS_7_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6)
#define EXPORT_OOLUA_FUNCTIONS_7_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6) \
	EXPORT_OOLUA_FUNCTIONS_7_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6)
#define EXPORT_OOLUA_FUNCTIONS_8_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7) \
	EXPORT_OOLUA_FUNCTIONS_8_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7)
#define EXPORT_OOLUA_FUNCTIONS_8_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7) \
	EXPORT_OOLUA_FUNCTIONS_8_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7)
#define EXPORT_OOLUA_FUNCTIONS_9_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8) \
	EXPORT_OOLUA_FUNCTIONS_9_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8)
#define EXPORT_OOLUA_FUNCTIONS_9_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8) \
	EXPORT_OOLUA_FUNCTIONS_9_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8)
#define EXPORT_OOLUA_FUNCTIONS_10_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9) \
	EXPORT_OOLUA_FUNCTIONS_10_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)
#define EXPORT_OOLUA_FUNCTIONS_10_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9) \
	EXPORT_OOLUA_FUNCTIONS_10_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)
#define EXPORT_OOLUA_FUNCTIONS_11_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10) \
	EXPORT_OOLUA_FUNCTIONS_11_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
#define EXPORT_OOLUA_FUNCTIONS_11_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10) \
	EXPORT_OOLUA_FUNCTIONS_11_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10)
#define EXPORT_OOLUA_FUNCTIONS_12_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) \
	EXPORT_OOLUA_FUNCTIONS_12_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)
#define EXPORT_OOLUA_FUNCTIONS_12_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) \
	EXPORT_OOLUA_FUNCTIONS_12_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11)
#define EXPORT_OOLUA_FUNCTIONS_13_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12) \
	EXPORT_OOLUA_FUNCTIONS_13_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)
#define EXPORT_OOLUA_FUNCTIONS_13_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12) \
	EXPORT_OOLUA_FUNCTIONS_13_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12)
#define EXPORT_OOLUA_FUNCTIONS_14_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13) \
	EXPORT_OOLUA_FUNCTIONS_14_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13)
#define EXPORT_OOLUA_FUNCTIONS_14_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13) \
	EXPORT_OOLUA_FUNCTIONS_14_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13)
#define EXPORT_OOLUA_FUNCTIONS_15_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14) \
	EXPORT_OOLUA_FUNCTIONS_15_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14)
#define EXPORT_OOLUA_FUNCTIONS_15_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14) \
	EXPORT_OOLUA_FUNCTIONS_15_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14)
#define EXPORT_OOLUA_FUNCTIONS_16_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15) \
	EXPORT_OOLUA_FUNCTIONS_16_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15)
#define EXPORT_OOLUA_FUNCTIONS_16_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15) \
	EXPORT_OOLUA_FUNCTIONS_16_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15)
#define EXPORT_OOLUA_FUNCTIONS_17_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16) \
	EXPORT_OOLUA_FUNCTIONS_17_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16)
#define EXPORT_OOLUA_FUNCTIONS_17_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16) \
	EXPORT_OOLUA_FUNCTIONS_17_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16)
#define EXPORT_OOLUA_FUNCTIONS_18_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17) \
	EXPORT_OOLUA_FUNCTIONS_18_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17)
#define EXPORT_OOLUA_FUNCTIONS_18_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17) \
	EXPORT_OOLUA_FUNCTIONS_18_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17)
#define EXPORT_OOLUA_FUNCTIONS_19_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18) \
	EXPORT_OOLUA_FUNCTIONS_19_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18)
#define EXPORT_OOLUA_FUNCTIONS_19_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18) \
	EXPORT_OOLUA_FUNCTIONS_19_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18)
#define EXPORT_OOLUA_FUNCTIONS_20_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19) \
	EXPORT_OOLUA_FUNCTIONS_20_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19)
#define EXPORT_OOLUA_FUNCTIONS_20_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19) \
	EXPORT_OOLUA_FUNCTIONS_20_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19)
#define EXPORT_OOLUA_FUNCTIONS_21_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20) \
	EXPORT_OOLUA_FUNCTIONS_21_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20)
#define EXPORT_OOLUA_FUNCTIONS_21_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20) \
	EXPORT_OOLUA_FUNCTIONS_21_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20)
#define EXPORT_OOLUA_FUNCTIONS_22_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21) \
	EXPORT_OOLUA_FUNCTIONS_22_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21)
#define EXPORT_OOLUA_FUNCTIONS_22_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21) \
	EXPORT_OOLUA_FUNCTIONS_22_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21)
#define EXPORT_OOLUA_FUNCTIONS_23_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22) \
	EXPORT_OOLUA_FUNCTIONS_23_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22)
#define EXPORT_OOLUA_FUNCTIONS_23_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22) \
	EXPORT_OOLUA_FUNCTIONS_23_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22)
#define EXPORT_OOLUA_FUNCTIONS_24_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23) \
	EXPORT_OOLUA_FUNCTIONS_24_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23)
#define EXPORT_OOLUA_FUNCTIONS_24_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23) \
	EXPORT_OOLUA_FUNCTIONS_24_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23)
#define EXPORT_OOLUA_FUNCTIONS_25_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24) \
	EXPORT_OOLUA_FUNCTIONS_25_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24)
#define EXPORT_OOLUA_FUNCTIONS_25_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24) \
	EXPORT_OOLUA_FUNCTIONS_25_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24)
#define EXPORT_OOLUA_FUNCTIONS_26_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25) \
	EXPORT_OOLUA_FUNCTIONS_26_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25)
#define EXPORT_OOLUA_FUNCTIONS_26_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25) \
	EXPORT_OOLUA_FUNCTIONS_26_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25)
#define EXPORT_OOLUA_FUNCTIONS_27_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26) \
	EXPORT_OOLUA_FUNCTIONS_27_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26)
#define EXPORT_OOLUA_FUNCTIONS_27_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26) \
	EXPORT_OOLUA_FUNCTIONS_27_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26)
#define EXPORT_OOLUA_FUNCTIONS_28_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27) \
	EXPORT_OOLUA_FUNCTIONS_28_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27)
#define EXPORT_OOLUA_FUNCTIONS_28_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27) \
	EXPORT_OOLUA_FUNCTIONS_28_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27)
#define EXPORT_OOLUA_FUNCTIONS_29_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28) \
	EXPORT_OOLUA_FUNCTIONS_29_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28)
#define EXPORT_OOLUA_FUNCTIONS_29_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28) \
	EXPORT_OOLUA_FUNCTIONS_29_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28)
#define EXPORT_OOLUA_FUNCTIONS_30_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29) \
	EXPORT_OOLUA_FUNCTIONS_30_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29)
#define EXPORT_OOLUA_FUNCTIONS_30_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29) \
	EXPORT_OOLUA_FUNCTIONS_30_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29)
#define EXPORT_OOLUA_FUNCTIONS_31_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30) \
	EXPORT_OOLUA_FUNCTIONS_31_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30)
#define EXPORT_OOLUA_FUNCTIONS_31_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30) \
	EXPORT_OOLUA_FUNCTIONS_31_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30)
#define EXPORT_OOLUA_FUNCTIONS_32_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31) \
	EXPORT_OOLUA_FUNCTIONS_32_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31)
#define EXPORT_OOLUA_FUNCTIONS_32_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31) \
	EXPORT_OOLUA_FUNCTIONS_32_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31)
#define EXPORT_OOLUA_FUNCTIONS_33_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32) \
	EXPORT_OOLUA_FUNCTIONS_33_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32)
#define EXPORT_OOLUA_FUNCTIONS_33_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32) \
	EXPORT_OOLUA_FUNCTIONS_33_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32)
#define EXPORT_OOLUA_FUNCTIONS_34_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33) \
	EXPORT_OOLUA_FUNCTIONS_34_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33)
#define EXPORT_OOLUA_FUNCTIONS_34_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33) \
	EXPORT_OOLUA_FUNCTIONS_34_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33)
#define EXPORT_OOLUA_FUNCTIONS_35_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34) \
	EXPORT_OOLUA_FUNCTIONS_35_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34)
#define EXPORT_OOLUA_FUNCTIONS_35_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34) \
	EXPORT_OOLUA_FUNCTIONS_35_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34)
#define EXPORT_OOLUA_FUNCTIONS_36_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35) \
	EXPORT_OOLUA_FUNCTIONS_36_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35)
#define EXPORT_OOLUA_FUNCTIONS_36_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35) \
	EXPORT_OOLUA_FUNCTIONS_36_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35)
#define EXPORT_OOLUA_FUNCTIONS_37_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36) \
	EXPORT_OOLUA_FUNCTIONS_37_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36)
#define EXPORT_OOLUA_FUNCTIONS_37_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36) \
	EXPORT_OOLUA_FUNCTIONS_37_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36)
#define EXPORT_OOLUA_FUNCTIONS_38_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37) \
	EXPORT_OOLUA_FUNCTIONS_38_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37)
#define EXPORT_OOLUA_FUNCTIONS_38_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37) \
	EXPORT_OOLUA_FUNCTIONS_38_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37)
#define EXPORT_OOLUA_FUNCTIONS_39_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38) \
	EXPORT_OOLUA_FUNCTIONS_39_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38)
#define EXPORT_OOLUA_FUNCTIONS_39_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38) \
	EXPORT_OOLUA_FUNCTIONS_39_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38)
#define EXPORT_OOLUA_FUNCTIONS_40_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39) \
	EXPORT_OOLUA_FUNCTIONS_40_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39)
#define EXPORT_OOLUA_FUNCTIONS_40_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39) \
	EXPORT_OOLUA_FUNCTIONS_40_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39)
#define EXPORT_OOLUA_FUNCTIONS_41_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40) \
	EXPORT_OOLUA_FUNCTIONS_41_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40)
#define EXPORT_OOLUA_FUNCTIONS_41_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40) \
	EXPORT_OOLUA_FUNCTIONS_41_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40)
#define EXPORT_OOLUA_FUNCTIONS_42_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41) \
	EXPORT_OOLUA_FUNCTIONS_42_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41)
#define EXPORT_OOLUA_FUNCTIONS_42_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41) \
	EXPORT_OOLUA_FUNCTIONS_42_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41)
#define EXPORT_OOLUA_FUNCTIONS_43_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42) \
	EXPORT_OOLUA_FUNCTIONS_43_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42)
#define EXPORT_OOLUA_FUNCTIONS_43_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42) \
	EXPORT_OOLUA_FUNCTIONS_43_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42)
#define EXPORT_OOLUA_FUNCTIONS_44_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43) \
	EXPORT_OOLUA_FUNCTIONS_44_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43)
#define EXPORT_OOLUA_FUNCTIONS_44_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43) \
	EXPORT_OOLUA_FUNCTIONS_44_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43)
#define EXPORT_OOLUA_FUNCTIONS_45_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44) \
	EXPORT_OOLUA_FUNCTIONS_45_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44)
#define EXPORT_OOLUA_FUNCTIONS_45_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44) \
	EXPORT_OOLUA_FUNCTIONS_45_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44)
#define EXPORT_OOLUA_FUNCTIONS_46_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45) \
	EXPORT_OOLUA_FUNCTIONS_46_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45)
#define EXPORT_OOLUA_FUNCTIONS_46_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45) \
	EXPORT_OOLUA_FUNCTIONS_46_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45)
#define EXPORT_OOLUA_FUNCTIONS_47_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46) \
	EXPORT_OOLUA_FUNCTIONS_47_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46)
#define EXPORT_OOLUA_FUNCTIONS_47_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46) \
	EXPORT_OOLUA_FUNCTIONS_47_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46)
#define EXPORT_OOLUA_FUNCTIONS_48_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47) \
	EXPORT_OOLUA_FUNCTIONS_48_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47)
#define EXPORT_OOLUA_FUNCTIONS_48_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47) \
	EXPORT_OOLUA_FUNCTIONS_48_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47)
#define EXPORT_OOLUA_FUNCTIONS_49_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48) \
	EXPORT_OOLUA_FUNCTIONS_49_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48)
#define EXPORT_OOLUA_FUNCTIONS_49_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48) \
	EXPORT_OOLUA_FUNCTIONS_49_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48)
#define EXPORT_OOLUA_FUNCTIONS_50_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49) \
	EXPORT_OOLUA_FUNCTIONS_50_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49)
#define EXPORT_OOLUA_FUNCTIONS_50_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49) \
	EXPORT_OOLUA_FUNCTIONS_50_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49)
#define EXPORT_OOLUA_FUNCTIONS_51_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50) \
	EXPORT_OOLUA_FUNCTIONS_51_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50)
#define EXPORT_OOLUA_FUNCTIONS_51_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50) \
	EXPORT_OOLUA_FUNCTIONS_51_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50)
#define EXPORT_OOLUA_FUNCTIONS_52_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51) \
	EXPORT_OOLUA_FUNCTIONS_52_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51)
#define EXPORT_OOLUA_FUNCTIONS_52_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51) \
	EXPORT_OOLUA_FUNCTIONS_52_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51)
#define EXPORT_OOLUA_FUNCTIONS_53_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52) \
	EXPORT_OOLUA_FUNCTIONS_53_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52)
#define EXPORT_OOLUA_FUNCTIONS_53_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52) \
	EXPORT_OOLUA_FUNCTIONS_53_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52)
#define EXPORT_OOLUA_FUNCTIONS_54_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53) \
	EXPORT_OOLUA_FUNCTIONS_54_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53)
#define EXPORT_OOLUA_FUNCTIONS_54_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53) \
	EXPORT_OOLUA_FUNCTIONS_54_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53)
#define EXPORT_OOLUA_FUNCTIONS_55_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54) \
	EXPORT_OOLUA_FUNCTIONS_55_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54)
#define EXPORT_OOLUA_FUNCTIONS_55_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54) \
	EXPORT_OOLUA_FUNCTIONS_55_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54)
#define EXPORT_OOLUA_FUNCTIONS_56_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55) \
	EXPORT_OOLUA_FUNCTIONS_56_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55)
#define EXPORT_OOLUA_FUNCTIONS_56_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55) \
	EXPORT_OOLUA_FUNCTIONS_56_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55)
#define EXPORT_OOLUA_FUNCTIONS_57_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56) \
	EXPORT_OOLUA_FUNCTIONS_57_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56)
#define EXPORT_OOLUA_FUNCTIONS_57_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56) \
	EXPORT_OOLUA_FUNCTIONS_57_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56)
#define EXPORT_OOLUA_FUNCTIONS_58_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57) \
	EXPORT_OOLUA_FUNCTIONS_58_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57)
#define EXPORT_OOLUA_FUNCTIONS_58_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57) \
	EXPORT_OOLUA_FUNCTIONS_58_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57)
#define EXPORT_OOLUA_FUNCTIONS_59_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58) \
	EXPORT_OOLUA_FUNCTIONS_59_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58)
#define EXPORT_OOLUA_FUNCTIONS_59_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58) \
	EXPORT_OOLUA_FUNCTIONS_59_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58)
#define EXPORT_OOLUA_FUNCTIONS_60_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58,p59) \
	EXPORT_OOLUA_FUNCTIONS_60_(OOLUA_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58,p59)
#define EXPORT_OOLUA_FUNCTIONS_60_NON_CONST(Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58,p59) \
	EXPORT_OOLUA_FUNCTIONS_60_(OOLUA_NON_CONST,Class,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58,p59)
/** \endcond */

/** \addtogroup OOLuaExporting  
@{
	\def OOLUA_EXPORT_NO_FUNCTIONS
	\hideinitializer
	\brief Inform that there are no functions of interest
	\param Class 
*/
#define OOLUA_EXPORT_NO_FUNCTIONS(Class)\
	EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Class)\
	EXPORT_OOLUA_FUNCTIONS_0_CONST(Class)

/**@}*/

#endif
