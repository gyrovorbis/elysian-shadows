
#ifndef TEST_UNITTEST_H
#define TEST_UNITTEST_H
#include <QString>
#include <QtTest>

class UnitTestsTest1 : public QObject
{
    Q_OBJECT

public:
    UnitTestsTest1();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
};

inline UnitTestsTest1::UnitTestsTest1()
{
}

inline void UnitTestsTest1::initTestCase()
{
}

inline void UnitTestsTest1::cleanupTestCase()
{
}

inline void UnitTestsTest1::testCase1()
{
    QVERIFY2(true, "Failure");
}


#endif

