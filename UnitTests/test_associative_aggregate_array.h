#ifndef TEST_ASSOCIATIVE_AGGREGATE_ARRAY_H
#define TEST_ASSOCIATIVE_AGGREGATE_ARRAY_H

#include <AssetIO/utility/elysian_associative_aggregate_array.hpp>
#include <QDebug>
#include <QtTest>

using namespace elysian;

namespace tktest {

class AssociativeAggregateArrayTest: public QObject {
    Q_OBJECT

    struct TestType {
        int num     = 0;

        bool operator==(const TestType& rhs) const { return this->num == rhs.num; }
    };

    struct TestData {
        bool    active = false;
        int     bullshit = 3;
        bool operator==(const TestData& rhs) const { return this->active == rhs.active && this->bullshit == rhs.bullshit; }
    };

    using TestAggregate = AssociativeAggregateArray<TestType, TestData>;

public:
    AssociativeAggregateArrayTest(void) = default;


private Q_SLOTS:
    void sizeDefault(void);          //everything else needs count to work!!!
    void constructorEntryFirst(void);
    void getEntryConst(void);
    void getEntry(void);
    void constructorEntryFirstSecond(void);
    void constructorEntriesRValue(void);
    void constructorEntriesLValue(void);
    void findEntryConst(void);
    void findEntry(void);
    void addEntry(void);
    void addEntries(void);
    void addEntriesNew(void);
    void removeEntryExists(void);
    void removeEntryDoesntExist(void);
    void removeEntriesExists(void);
    void removeEntriesNotAllExist(void);
    void clear(void);
    void constIteration(void);
    void iteration(void);
    void subscriptOperatorIntConst(void);
    void subscriptOperatorInt(void);
    void subscriptOperatorFirstConst(void);
    void subscriptOperatorFirst(void);

    //Overloaded operators (internal)
    void plusEqualsOperator(void);
    void minusEqualsOperator(void);
    void assignmentOperatorRValue(void);
    void assignmentOperatorLValue(void);

    //Overloaded operators (external)
    void additionOperator(void);
    void subtractionOperator(void);
    void equalityOperator(void);
    void inequalityOperator(void);

};

inline void AssociativeAggregateArrayTest::sizeDefault(void) {
    TestAggregate agg;
    QVERIFY(!agg.getCount());
}

inline void AssociativeAggregateArrayTest::constructorEntryFirst(void) {
    TestAggregate agg(TestType{});
    QVERIFY(agg.getCount() == 1);
}

inline void AssociativeAggregateArrayTest::getEntryConst(void) {
    const TestAggregate agg(TestType{});
    const auto entry = agg.getEntry(0);
    QVERIFY(entry->first.num == 0);
    QVERIFY(entry->second.active == false);
    QVERIFY(entry->second.bullshit == 3);
}


inline void AssociativeAggregateArrayTest::getEntry(void) {
    TestAggregate agg(TestType{});
    auto entry = agg.getEntry(0);
    QVERIFY(entry->first.num == 0);
    QVERIFY(entry->second.active == false);
    QVERIFY(entry->second.bullshit == 3);
}

inline void AssociativeAggregateArrayTest::constructorEntryFirstSecond(void) {
    TestAggregate agg({TestType{ 2 }, TestData{ true, -1 }});
    auto entry = agg.getEntry(0);
    QVERIFY(entry->first.num == 2);
    QVERIFY(entry->second.active == true);
    QVERIFY(entry->second.bullshit == -1);

}

inline void AssociativeAggregateArrayTest::constructorEntriesRValue(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    });
    QVERIFY(agg.getCount() == 3);
    auto entry = agg.getEntry(0);
    QVERIFY(entry->first.num == 4);
    entry = agg.getEntry(1);
    QVERIFY(entry->second.active == true);
    entry = agg.getEntry(2);
    QVERIFY(entry->first.num == 6);
    QVERIFY(entry->second.active == false);
    QVERIFY(entry->second.bullshit == 18);
}


inline void AssociativeAggregateArrayTest::constructorEntriesLValue(void) {
    std::vector<TestAggregate::Entry> entries = {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    };
    TestAggregate agg(entries);
    QVERIFY(agg.getCount() == 3);
    auto entry = agg.getEntry(0);
    QVERIFY(entry->first.num == 4);
    entry = agg.getEntry(1);
    QVERIFY(entry->second.active == true);
    entry = agg.getEntry(2);
    QVERIFY(entry->first.num == 6);
    QVERIFY(entry->second.active == false);
    QVERIFY(entry->second.bullshit == 18);
}

inline void AssociativeAggregateArrayTest::findEntryConst(void) {
    const TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    });
    QVERIFY(agg.findEntry(TestType{ 4 }));
    QVERIFY(!agg.findEntry(TestType{ 12 }));
}

inline void AssociativeAggregateArrayTest::findEntry(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    });
    QVERIFY(agg.findEntry(TestType{ 4 }));
    QVERIFY(!agg.findEntry(TestType{ 12 }));

}

inline void AssociativeAggregateArrayTest::addEntry(void) {
    TestAggregate agg;
    agg.addEntry({TestType{ 2 }, TestData{ true, -1 }});
    QVERIFY(agg.getCount() == 1);
    auto entry = agg.getEntry(0);
    QVERIFY(entry->first.num == 2);
    QVERIFY(entry->second.active == true);
    QVERIFY(entry->second.bullshit == -1);

}

inline void AssociativeAggregateArrayTest::addEntries(void) {
    TestAggregate agg;
    agg.addEntries(std::vector<TestAggregate::Entry> {
                       TestType{ 4 },
                       { TestType{}, TestData{ true} },
                       { TestType{ 6 }, TestData{ false, 18 }}
                   });
    QVERIFY(agg.getCount() == 3);
}

inline void AssociativeAggregateArrayTest::addEntriesNew(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    });

    QVERIFY(agg.addNewEntries(std::vector<TestAggregate::Entry> {
                       TestType{ 4 },
                       { TestType{7}, TestData{ true} },
                       { TestType{ 6 }, TestData{ false, 18 }}
                   }) == 1);

    QVERIFY(agg.getCount() == 4);
}

inline void AssociativeAggregateArrayTest::removeEntryExists(void) {
    TestAggregate agg({TestType{ 2 }, TestData{ true, -1 }});
    QVERIFY(agg.removeEntry(TestType{ 2 }));
    QVERIFY(!agg.getCount());
}

inline void AssociativeAggregateArrayTest::removeEntryDoesntExist(void) {
    TestAggregate agg({TestType{ 2 }, TestData{ true, -1 }});
    QVERIFY(!agg.removeEntry(TestType{ 4 }));
    QVERIFY(agg.getCount() == 1);

}

inline void AssociativeAggregateArrayTest::removeEntriesExists(void) {
    std::vector<TestAggregate::Entry> entries = {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    };
    std::vector<TestType> entriesRemove = {
        TestType{ 4 },
        TestType{},
        TestType{ 6 }
    };
    TestAggregate agg(entries);
    QVERIFY(agg.removeEntries(entriesRemove));
    QVERIFY(!agg.getCount());

}

inline void AssociativeAggregateArrayTest::removeEntriesNotAllExist(void) {
    std::vector<TestAggregate::Entry> entries = {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    };
    std::vector<TestType> entriesRemove = {
        TestType{ 4 },
        TestType{ 8 },
        TestType{ 6 }
    };
    TestAggregate agg(entries);
    QVERIFY(agg.removeEntries(entriesRemove) == 2);
    QVERIFY(agg.getCount() == 1);
}


inline void AssociativeAggregateArrayTest::clear(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    });
    agg.clear();
    QVERIFY(agg.getCount() == 0);
}

inline void AssociativeAggregateArrayTest::constIteration(void) {
    std::vector<TestAggregate::Entry> entries = {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    };

    TestAggregate agg(entries);

    auto it = entries.cbegin();

    for(const auto& i : agg) {
        QVERIFY((*it) == i);
        ++it;
    }
}

inline void AssociativeAggregateArrayTest::iteration(void) {
    std::vector<TestAggregate::Entry> entries = {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    };

    TestAggregate agg(entries);

    auto it = entries.begin();
    for(auto& i : agg) {
        QVERIFY(i == (*it));
        ++it;
    }
}

inline void AssociativeAggregateArrayTest::subscriptOperatorIntConst(void) {
    const TestAggregate agg(TestType{});
    const auto entry = agg[0];
    QVERIFY(entry->first.num == 0);
    QVERIFY(entry->second.active == false);
    QVERIFY(entry->second.bullshit == 3);

}

inline void AssociativeAggregateArrayTest::subscriptOperatorInt(void) {
    TestAggregate agg(TestType{});
    auto entry = agg[0];
    QVERIFY(entry->first.num == 0);
    QVERIFY(entry->second.active == false);
    QVERIFY(entry->second.bullshit == 3);
}

inline void AssociativeAggregateArrayTest::subscriptOperatorFirstConst(void) {
    const TestAggregate agg(TestType{12});
    const auto entry = agg[TestType{12}];
    QVERIFY(entry->first.num == 12);
    QVERIFY(entry->second.active == false);
    QVERIFY(entry->second.bullshit == 3);
}

inline void AssociativeAggregateArrayTest::subscriptOperatorFirst(void) {
    TestAggregate agg(TestType{12});
    auto entry = agg[TestType{12}];
    QVERIFY(entry->first.num == 12);
    QVERIFY(entry->second.active == false);
    QVERIFY(entry->second.bullshit == 3);
}

inline void AssociativeAggregateArrayTest::plusEqualsOperator(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    });

    agg += std::vector<TestAggregate::Entry> {
            TestType{ 8 },
            { TestType{19}, TestData{ true} },
            { TestType{ 20 }, TestData{ false, 18 }}
        };

    QVERIFY(agg.getCount() == 6);

}

inline void AssociativeAggregateArrayTest::minusEqualsOperator(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 4 },
        { TestType{}, TestData{ true} },
        { TestType{ 6 }, TestData{ false, 18 }}
    });

    agg -= std::vector<TestAggregate::Entry> {
            TestType{ 4 },
            { TestType{}, TestData{ true} },
            { TestType{ 20 }, TestData{ false, 18 }}
        };

    QVERIFY(agg.getCount() == 1);

}

inline void AssociativeAggregateArrayTest::assignmentOperatorRValue(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 19 },
        { TestType{}, TestData{ true} }
    });

    agg = std::vector<TestAggregate::Entry> {
            TestType{ 4 },
            { TestType{}, TestData{ true} },
            { TestType{ 20 }, TestData{ false, 18 }}
        };

    QVERIFY(agg.getCount() == 3);
    QVERIFY(!agg.findEntry({19}));
    QVERIFY(agg.findEntry({20}));

}

inline void AssociativeAggregateArrayTest::assignmentOperatorLValue(void) {

    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 19 },
        { TestType{}, TestData{ true} }
    });

    auto agg2 = std::vector<TestAggregate::Entry> {
            TestType{ 4 },
            { TestType{}, TestData{ true} },
            { TestType{ 20 }, TestData{ false, 18 }}
        };

    agg = agg2;

    QVERIFY(agg.getCount() == 3);
    QVERIFY(!agg.findEntry({19}));
    QVERIFY(agg.findEntry({20}));
}

inline void AssociativeAggregateArrayTest::additionOperator(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 19 },
        { TestType{}, TestData{ true} }
    });

    auto agg2 = TestAggregate(std::vector<TestAggregate::Entry> {
            TestType{ 4 },
            { TestType{18}, TestData{ true} },
            { TestType{ 20 }, TestData{ false, 18 }}
        });

    auto agg3 = agg + agg2;

    QVERIFY(agg3.getCount() == 5);
    QVERIFY(agg3.findEntry({19}));
    QVERIFY(agg3.findEntry(TestType{18}));

}

inline void AssociativeAggregateArrayTest::subtractionOperator(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 19 },
        { TestType{}, TestData{ true} }
    });

    auto agg2 = TestAggregate(std::vector<TestAggregate::Entry> {
            TestType{19}
        });

    auto agg3 = agg - agg2;

    QVERIFY(agg3.getCount() == 1);
    QVERIFY(!agg3.findEntry({19}));
    QVERIFY(agg3.findEntry({0}));
}

inline void AssociativeAggregateArrayTest::equalityOperator(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 19 },
        { TestType{}, TestData{ true} }
    });

    auto agg2 = TestAggregate(std::vector<TestAggregate::Entry> {
            TestType{ 19 },
            { TestType{}, TestData{ true} }
        });

    QVERIFY(agg == agg2);
}

inline void AssociativeAggregateArrayTest::inequalityOperator(void) {
    TestAggregate agg(std::vector<TestAggregate::Entry> {
        TestType{ 19 },
        { TestType{}, TestData{ true} }
    });

    auto agg2 = TestAggregate(std::vector<TestAggregate::Entry> {
            TestType{ 4 },
            { TestType{}, TestData{ true} },
            { TestType{ 20 }, TestData{ false, 18 }}
        });

    QVERIFY(agg != agg2);
}


}



#endif // TEST_ASSOCIATIVE_AGGREGATE_ARRAY_H
