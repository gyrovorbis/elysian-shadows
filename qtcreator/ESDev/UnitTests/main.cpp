
#include <QtTest>

#include "test_unittest.h"
#include "tst_unitteststest.h"


int main(int argc, char** argv)
{
   int status = 0;
   auto ASSERT_TEST = [&status, argc, argv](QObject* obj) {
     status |= QTest::qExec(obj, argc, argv);
     delete obj;
   };

   ASSERT_TEST(new UnitTestsTest1());
   ASSERT_TEST(new UnitTestsTest2());

   return status;
}

