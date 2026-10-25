/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TEST_STRING_H
#define TEST_STRING_H

#include <QtTest>

#define DD_STRING_GROW_SIZE_PADDING 1
#include <AssetIO/utility/elysian_string.hpp>

/*
    If move semantic-ing a string from a type with a heap allocated buffer to a type
    whose stack buffer is large enough to contain it, where allocators match (legit move).
    1. if the #define hasn't been made, do not shrink to stack
    2. copy to stack if the #define is there to shrink
*/

#define TEST(a, b)          inline void a::b(void)
#define EXPECT_TRUE(a)      QVERIFY(a);
#define EXPECT_FALSE(a)     QVERIFY(!(a));

namespace tktest {

using namespace elysian;

struct TestStrAlloc {
    size_t allocCount	= 0;

    void* allocate(size_t size) {
        ++allocCount;
        return malloc(size);
    }

    void* reallocate(void* ptr, size_t size) {
        return realloc(ptr, size);
    }

    void deallocate(void* ptr) {
        if(ptr) --allocCount;
        free(ptr);
    }
};


struct TestStrAlloc2: public TestStrAlloc {};

using TestAlloc 	=	TestStrAlloc;
using TestAlloc2 	= 	TestStrAlloc2;

template<size_t S>
using TestString = String<TestAlloc, S>;

template<size_t S>
using TestString2 = String<TestAlloc2, S>;

class StringTest: public QObject {
    Q_OBJECT


private Q_SLOTS:

    void ConstructEmptyString(void);
    void ConstructCArrayStack(void);
    void ConstructCArrayHeap(void);
    void CopyConstructStack(void);
    void CopyConstructHeap(void);
    void CopyConstructHeapSameAllocator(void);
    void MoveConstructStack(void);
    void MoveConstructHeap(void);
    void SetCStringStack(void);
    void SetCStringHeap(void);
    void Back(void);
    void Front(void);
    void AppendPartialCStringStack(void);
    void AppendFullCStringHeap(void);
    void AppendPartialStringHeap(void);
    void AppendFullStringStack(void);
    void AppendFullStringHeap(void);
    void AppendPartialIteratorStack(void);
    void AppendFullIteratorHeap(void);
    void ResizeEqual(void);
    void ResizeGrowStackToStack(void);
    void ResizeGrowStackToHeap(void);
    void ResizeGrowHeapToHeap(void);
    void ResizeShrinkStackToStack(void);
    void ResizeShrinkHeapToHeap(void);
    void ResizeShrinkHeapToStack(void);
    void ShrinkToFitStackToStack(void);
    void ShrinkToFitHeapToHeap(void);
    void ShrinkToFitHeapToStack(void);
    void At(void);
    void ErasePartialByIndex(void);
    void EraseFullByIndex(void);
    void EraseCharacter(void);
    void EraseIteratorRange(void);
    void SwapSameTypeSharedAllocator(void);
    void SwapSameTypeDifferentAllocator(void);
    void SwapDifferentTypes(void);
    void InsertCStringPartial(void);
    void InsertCStringFull(void);
    void InsertStringPartial(void);
    void InsertStringFull(void);
    void InsertIteratorRange(void);
    void InsertFillCharacter(void);
    void ReplaceCStringPartial(void);
    void ReplaceCStringFull(void);
    void ReplaceStringPartial(void);
    void ReplaceStringFull(void);
    void ReplaceStringAtIteratorRange(void);
    void ReplaceIteratorRangeByIteatorRange(void);
    void FindCStringPartialFound(void);
    void FindCStringFullFound(void);
    void FindCStringPartialNotFound(void);
    void FindCStringFullNotFound(void);
    void FindStringPartialFound(void);
    void FindStringFullFound(void);
    void FindStringPartialNotFound(void);
    void FindStringFullNotFound(void);
    void SubscriptOperator(void);
    void EqualOperatorCString(void);
    void NotEqualOperatorCString(void);
    void PlusEqualsOperatorCString(void);
    void AssignmentOperatorCString(void);
    void EqualOperatorString(void);
    void NotEqualOperatorString(void);
    void PlusEqualsOperatorString(void);
    void AssignmentOperatorString(void);
    void MoveAssignmentOperatorStringStack(void);
    void MoveAssignmentOperatorStringHeap(void);
    void GlobalStringAdditionOperator(void);
    void Format(void);
    void Iterators(void);
    void ConstIterators(void);
    void ReverseIterators(void);
    void ConstReverseIterators(void);
};






TEST(StringTest, ConstructEmptyString) {
    TestString<10> str(nullptr);

    EXPECT_TRUE(str.GetDefaultStackSize() == DD_STRING_STACK_SIZE_DEFAULT);

    EXPECT_TRUE(str.GetLength() == 0)
    EXPECT_TRUE(str.GetAllocator() == nullptr);
    EXPECT_TRUE(str.GetCapacity() == 10);
    EXPECT_TRUE(str.GetStackSize() == 10);
    EXPECT_TRUE(str.GetCString()[0] == '\0');
    EXPECT_FALSE(str.IsHeapAllocated())
}

TEST(StringTest, ConstructCArrayStack) {
    TestString<12> str("12345", nullptr);
    EXPECT_TRUE(str.GetLength() == 5)
    EXPECT_TRUE(str.GetAllocator() == nullptr);
    EXPECT_TRUE(str.GetCapacity() == 12);
    EXPECT_TRUE(str.GetStackSize() == 12);
    EXPECT_TRUE(strcmp(str.GetCString(), "12345") == 0);
    EXPECT_FALSE(str.IsHeapAllocated())
}

TEST(StringTest, ConstructCArrayHeap) {
    TestAlloc alloc;
    EXPECT_TRUE(alloc.allocCount == 0);
    {
        TestString<5> str("1234567890", &alloc);
        EXPECT_TRUE(alloc.allocCount == 1);
        EXPECT_TRUE(str.GetLength() == 10)
        EXPECT_TRUE(str.GetAllocator() == &alloc);
        EXPECT_TRUE(str.GetCapacity() == 12);
        EXPECT_TRUE(str.GetStackSize() == 5);
        EXPECT_TRUE(strcmp(str.GetCString(), "1234567890") == 0);
        EXPECT_TRUE(str.IsHeapAllocated())
    }
    EXPECT_TRUE(alloc.allocCount == 0);
}

TEST(StringTest, CopyConstructStack) {
    TestString<5> str2("1234567", nullptr);
    TestString<11> str = str2;

    EXPECT_TRUE(str.GetLength() == 7)
    EXPECT_TRUE(str.GetAllocator() == nullptr);
    EXPECT_TRUE(str.GetCapacity() == 11);
    EXPECT_TRUE(str.GetStackSize() == 11);
    EXPECT_TRUE(strcmp(str.GetCString(), "1234567") == 0);
    EXPECT_FALSE(str.IsHeapAllocated())
}

TEST(StringTest, CopyConstructHeap) {
    TestAlloc alloc;
    TestAlloc2 alloc2;
    EXPECT_TRUE(alloc.allocCount == 0)
    {
        TestString<3> str2("1234567", &alloc);
        EXPECT_TRUE(alloc.allocCount == 1);
        EXPECT_TRUE(alloc2.allocCount == 0);
        {
            TestString2<3> str(str2, &alloc2);
            EXPECT_TRUE(alloc.allocCount == 1);
            EXPECT_TRUE(alloc2.allocCount == 1);
            EXPECT_TRUE(str.GetLength() == 7)
            EXPECT_TRUE(str.GetAllocator() == &alloc2);
            EXPECT_TRUE(str.GetCapacity() == 9);
            EXPECT_TRUE(str.GetStackSize() == 3);
            EXPECT_TRUE(strcmp(str.GetCString(), "1234567") == 0);
            EXPECT_TRUE(str.IsHeapAllocated())
        }
        EXPECT_TRUE(alloc.allocCount == 1);
        EXPECT_TRUE(alloc2.allocCount == 0);
    }
    EXPECT_TRUE(alloc.allocCount == 0);
}

TEST(StringTest, CopyConstructHeapSameAllocator) {
    TestAlloc alloc;
    TestAlloc2 alloc2;
    EXPECT_TRUE(alloc.allocCount == 0)
    {
        TestString<3> str2("1234567", &alloc);
        EXPECT_TRUE(alloc.allocCount == 1);
        {
            TestString<5> str(str2, &alloc);
            EXPECT_TRUE(alloc.allocCount == 2);
            EXPECT_TRUE(str.GetLength() == 7)
            EXPECT_TRUE(str.GetAllocator() == &alloc);
            EXPECT_TRUE(str.GetCapacity() == 9);
            EXPECT_TRUE(str.GetStackSize() == 5);
            EXPECT_TRUE(strcmp(str.GetCString(), "1234567") == 0);
            EXPECT_TRUE(str.IsHeapAllocated())
        }
        EXPECT_TRUE(alloc.allocCount == 1);
    }
    EXPECT_TRUE(alloc.allocCount == 0);
}

TEST(StringTest, MoveConstructStack) {
    TestAlloc alloc;
    TestString<8> str = TestString<3>("1234567", &alloc);

    EXPECT_TRUE(alloc.allocCount == 0);
    EXPECT_TRUE(str.GetLength() == 7)
    EXPECT_TRUE(str.GetAllocator() == &alloc);
    EXPECT_TRUE(str.GetCapacity() == 8);
    EXPECT_TRUE(str.GetStackSize() == 8);
    EXPECT_TRUE(strcmp(str.GetCString(), "1234567") == 0);
    EXPECT_FALSE(str.IsHeapAllocated())
}


TEST(StringTest, MoveConstructHeap) {
    TestAlloc alloc;
    {
        auto str2 = TestString<3>("1234567", &alloc);
        const char* strPtr = str2.GetCString();
        EXPECT_TRUE(alloc.allocCount == 1);
        {
            TestString<4> str = std::move(str2);
            EXPECT_TRUE(alloc.allocCount == 1);
            EXPECT_TRUE(str.GetCString() == strPtr);

            EXPECT_TRUE(str2.GetCString() == nullptr);
            EXPECT_TRUE(str.GetLength() == 7)
            EXPECT_TRUE(str.GetAllocator() == &alloc);
            EXPECT_TRUE(str.GetCapacity() == 9);
            EXPECT_TRUE(str.GetStackSize() == 4);
            EXPECT_TRUE(strcmp(str.GetCString(), "1234567") == 0);
            EXPECT_TRUE(str.IsHeapAllocated())
        }
    }
    EXPECT_TRUE(alloc.allocCount == 0);
}

TEST(StringTest, SetCStringStack)
{
    TestAlloc alloc;
    {
        TestString<10> str(&alloc);
        str.SetCString("1234567");
        EXPECT_TRUE(alloc.allocCount == 0);
        EXPECT_TRUE(str.GetLength() == 7)
        EXPECT_TRUE(str.GetAllocator() == &alloc);
        EXPECT_TRUE(str.GetCapacity() == 10);
        EXPECT_TRUE(str.GetStackSize() == 10);
        EXPECT_TRUE(strcmp(str.GetCString(), "1234567") == 0);
        EXPECT_FALSE(str.IsHeapAllocated())
    }
    EXPECT_TRUE(alloc.allocCount == 0);
}

TEST(StringTest, SetCStringHeap)
{
    TestAlloc alloc;
    {
        TestString<3> str(&alloc);
        str.SetCString("1234567");
        EXPECT_TRUE(alloc.allocCount == 1);
        EXPECT_TRUE(strcmp(str.GetCString(), "1234567") == 0);
        EXPECT_TRUE(str.GetLength() == 7)
        EXPECT_TRUE(str.GetAllocator() == &alloc);
        EXPECT_TRUE(str.GetCapacity() == 9);
        EXPECT_TRUE(str.GetStackSize() == 3);
        EXPECT_TRUE(strcmp(str.GetCString(), "1234567") == 0);
        EXPECT_TRUE(str.IsHeapAllocated())
    }
    EXPECT_TRUE(alloc.allocCount == 0);
}

TEST(StringTest, Back)
{
    TestString<2> str("ab");
    EXPECT_TRUE(str.Back() == 'b');
}

TEST(StringTest, Front)
{
    TestString<2> str("ab");
    EXPECT_TRUE(str.Front() == 'a');
}

TEST(StringTest, AppendPartialCStringStack)
{
    TestString<20> str("Hell");
    const char* str2 = "go world!";
    str.Append(str2, 1, 1);

    EXPECT_TRUE(strcmp(str.GetCString(), "Hello") == 0);
    EXPECT_TRUE(str.GetLength() == 5);
    EXPECT_FALSE(str.IsHeapAllocated());
}

TEST(StringTest, AppendFullCStringHeap)
{
    TestString<2> str("Hello");
    const char* str2 = " World!";
    str.Append(str2);

    EXPECT_TRUE(strcmp(str.GetCString(), "Hello World!") == 0);
    EXPECT_TRUE(str.GetLength() == 12);
    EXPECT_TRUE(str.IsHeapAllocated());
}


TEST(StringTest, AppendPartialStringHeap)
{
    TestString<2> str("Hell");
    TestString<2> str2 = "go world!";
    str.Append(str2, 1, 1);

    EXPECT_TRUE(strcmp(str.GetCString(), "Hello") == 0);
    EXPECT_TRUE(str.GetLength() == 5);
    EXPECT_TRUE(str.IsHeapAllocated());
}

TEST(StringTest, AppendFullStringStack)
{
    TestString<20> str("Hello");
    TestString<5> str2 = " World!";
    str.Append(str2);

    EXPECT_TRUE(strcmp(str.GetCString(), "Hello World!") == 0);
    EXPECT_TRUE(str.GetLength() == 12);
    EXPECT_FALSE(str.IsHeapAllocated());
}

TEST(StringTest, AppendFullStringHeap)
{
    TestString<2> str("Hello");
    TestString<5> str2 = " World!";
    str.Append(str2);

    EXPECT_TRUE(strcmp(str.GetCString(), "Hello World!") == 0);
    EXPECT_TRUE(str.GetLength() == 12);
    EXPECT_TRUE(str.IsHeapAllocated());
}

TEST(StringTest, AppendPartialIteratorStack)
{
    TestString<10> str("Hell");
    TestString2<2> str2 = "go world!";

    auto beginIt = str2.begin();
    beginIt += 1;
    //auto endIt = beginIt + 1;
  //  str.Append(beginIt, endIt);

    EXPECT_TRUE(strcmp(str.GetCString(), "Hello") == 0);
    EXPECT_TRUE(str.GetLength() == 5);
    EXPECT_FALSE(str.IsHeapAllocated());
}

TEST(StringTest, AppendFullIteratorHeap)
{

}

TEST(StringTest, ResizeEqual) {
    TestString<4> str("Lol");
    EXPECT_FALSE(str.Resize(4));
    EXPECT_TRUE(str.GetCapacity() == 4);
}

TEST(StringTest, ResizeGrowStackToStack)
{
    TestString<20> str("lol");
    EXPECT_FALSE(str.Resize(10));
    EXPECT_TRUE(str.GetCapacity() == 20);
}

TEST(StringTest, ResizeGrowStackToHeap)
{
    TestString<20> str("lol");
    EXPECT_TRUE(str.Resize(30));
    EXPECT_TRUE(str.GetCapacity() == 31);
    EXPECT_TRUE(str.IsHeapAllocated());
}

TEST(StringTest, ResizeGrowHeapToHeap)
{
    TestString<1> str("lol");
    EXPECT_TRUE(str.Resize(10));
    EXPECT_TRUE(str.GetCapacity() == 11);
    EXPECT_TRUE(str.IsHeapAllocated());
}

TEST(StringTest, ResizeShrinkStackToStack)
{
    TestString<20> str("lol");
    EXPECT_FALSE(str.Resize(3));
    EXPECT_TRUE(str.GetCapacity() == 20);
    EXPECT_FALSE(str.IsHeapAllocated());
    EXPECT_TRUE(strcmp(str.GetCString(), "lol") == 0);
}

TEST(StringTest, ResizeShrinkHeapToHeap)
{
    TestString<1> str("lololo");
    EXPECT_FALSE(str.Resize(4));
    EXPECT_TRUE(str.GetCapacity() == 8);
    EXPECT_TRUE(str.IsHeapAllocated());
    qDebug() << str.GetCString();
    EXPECT_TRUE(strcmp(str.GetCString(), "lol") == 0);
}

TEST(StringTest, ResizeShrinkHeapToStack)
{
    TestString<3> str("lololo");
    EXPECT_TRUE(str.Resize(2));
    EXPECT_TRUE(str.GetCapacity() == 3);
    EXPECT_FALSE(str.IsHeapAllocated());
    EXPECT_TRUE(strcmp(str.GetCString(), "lo") == 0);
}

TEST(StringTest, ShrinkToFitStackToStack)
{
    TestString<30> str("lol");
    EXPECT_FALSE(str.ShrinkToFit());
    EXPECT_TRUE(str.GetCapacity() == 30);
}

TEST(StringTest, ShrinkToFitHeapToHeap)
{
    TestString<3> str("lololololo");
    EXPECT_FALSE(str.Resize(4));
    EXPECT_TRUE(str.IsHeapAllocated());
    EXPECT_TRUE(str.GetCapacity() == 12);
    EXPECT_TRUE(str.ShrinkToFit());
    EXPECT_TRUE(str.GetCapacity() == 5);
    EXPECT_TRUE(str == "lol");
}

TEST(StringTest, ShrinkToFitHeapToStack)
{

}

TEST(StringTest, At)
{
    TestString<10> str("LOL");
    EXPECT_TRUE(str.At(0) == 'L');
}

TEST(StringTest, ErasePartialByIndex)
{

}

TEST(StringTest, EraseFullByIndex)
{

}

TEST(StringTest, EraseCharacter)
{

}

TEST(StringTest, EraseIteratorRange)
{

}

TEST(StringTest, SwapSameTypeSharedAllocator)
{

}

TEST(StringTest, SwapSameTypeDifferentAllocator)
{

}

TEST(StringTest, SwapDifferentTypes)
{

}

TEST(StringTest, InsertCStringPartial)
{

}

TEST(StringTest, InsertCStringFull)
{

}

TEST(StringTest, InsertStringPartial)
{

}

TEST(StringTest, InsertStringFull)
{

}

TEST(StringTest, InsertIteratorRange)
{

}

TEST(StringTest, InsertFillCharacter)
{

}

TEST(StringTest, ReplaceCStringPartial)
{

}

TEST(StringTest, ReplaceCStringFull)
{

}

TEST(StringTest, ReplaceStringPartial)
{

}

TEST(StringTest, ReplaceStringFull)
{

}

TEST(StringTest, ReplaceStringAtIteratorRange)
{

}

TEST(StringTest, ReplaceIteratorRangeByIteatorRange)
{

}

TEST(StringTest, FindCStringPartialFound)
{

}

TEST(StringTest, FindCStringFullFound)
{

}

TEST(StringTest, FindCStringPartialNotFound)
{

}

TEST(StringTest, FindCStringFullNotFound)
{

}

TEST(StringTest, FindStringPartialFound)
{

}

TEST(StringTest, FindStringFullFound)
{

}

TEST(StringTest, FindStringPartialNotFound)
{

}

TEST(StringTest, FindStringFullNotFound)
{

}

TEST(StringTest, SubscriptOperator)
{
    TestString<10> str("LOLO");
    EXPECT_TRUE(str[0] == 'L');
    EXPECT_TRUE(str[3] == 'O');
}

TEST(StringTest, EqualOperatorCString)
{
    TestString<10> str("LOLO");
    EXPECT_TRUE(str == "LOLO");
    EXPECT_FALSE(str == "DUDE");
}

TEST(StringTest, NotEqualOperatorCString)
{
    TestString<10> str("LOLO");
    EXPECT_FALSE(str != "LOLO");
    EXPECT_TRUE(str != "DUDE");

}

TEST(StringTest, PlusEqualsOperatorCString)
{
    TestString<10> str("Hell");
    str += "o World!";
    EXPECT_TRUE(str == "Hello World!");
}

TEST(StringTest, AssignmentOperatorCString)
{
    TestString<10> str("Hell");
    str = "Bye";
    EXPECT_TRUE(str == "Bye");
}

TEST(StringTest, EqualOperatorString)
{
    TestString<10> str("Test1");
    TestString<10> str2("Test1");
    EXPECT_TRUE(str == str2);

}

TEST(StringTest, NotEqualOperatorString)
{
    TestString<10> str("Test1");
    TestString<10> str2("Test1");
    EXPECT_FALSE(str != str2);
}

TEST(StringTest, PlusEqualsOperatorString)
{
    TestString<10> str("Hell");
    TestString<3> str2("o World!");
    str += str2;
    EXPECT_TRUE(str == "Hello World!");
}

TEST(StringTest, AssignmentOperatorString)
{
    return;
    TestString<10> str("Test1");
    TestString<10> str2("Test2");
   // str = str2;
    EXPECT_TRUE(str == str2);
}

TEST(StringTest, MoveAssignmentOperatorStringStack)
{

}

TEST(StringTest, MoveAssignmentOperatorStringHeap)
{

}

TEST(StringTest, GlobalStringAdditionOperator)
{

}

TEST(StringTest, Format)
{
    return;
    TestString<10> str("Test1");
    auto str2 = TestString<100>::format(nullptr, "CStr = %s, Str = %s, Num = %d", "CHAR", str, 55);
    EXPECT_TRUE(str2 == "CStr = CHAR, Str = Test1, Num = 55");
}

TEST(StringTest, Iterators)
{

}

TEST(StringTest, ConstIterators)
{

}

TEST(StringTest, ReverseIterators)
{

}

TEST(StringTest, ConstReverseIterators)
{

}

}

#undef TEST

#endif // TEST_STRING_H
