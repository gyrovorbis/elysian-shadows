#ifndef UNITTEST_2
#define UNITTEST_2

#include <QString>
#include <QtTest>

class UnitTestsTest2 : public QObject
{
    Q_OBJECT

public:
    UnitTestsTest2();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
};

inline UnitTestsTest2::UnitTestsTest2()
{
}

inline void UnitTestsTest2::initTestCase()
{
}

inline void UnitTestsTest2::cleanupTestCase()
{
}

inline void UnitTestsTest2::testCase1()
{
    QVERIFY2(true, "Failure");
}


#endif
