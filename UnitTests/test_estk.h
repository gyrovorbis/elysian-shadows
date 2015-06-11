#ifndef TEST_ESTK_H
#define TEST_ESTK_H

#include <QTest>
#include <QObject>
#include <tk_estoolkit.h>
#include <tk_asset_manager.h>
#include <QList>
#include <QStack>
#include <gyro_system_api.h>
#include <QMutex>

class ESToolkit;

namespace tktest {

class ESTkTest: public QObject, public AssetManager {
    friend int              main(int arg, char*argv[]);
protected:
    static ESToolkit*       _estk;
    static QList<QObject*>  _testQueue;
    static QStack<QString>  _subTestStack;
    static QMutex*          _mutex;
    static bool             _initialized;

    static void             setESToolkit(ESToolkit*const estk);
    static ESToolkit*       getESToolkit(void);

    bool                    currentProjectLevelAreaExist(void);

public:
    static void             enqueueNextTest(QObject* object);
    static void             enqueueTest(QObject* object);
    static void             pushSubTest(const QString name);
    static void             popSubTest(const bool success = true);
    static void             flagError(const QString msg);
    static void             flagWarning(const QString msg);

    static void             initialize(void);

};


inline void ESTkTest::flagWarning(const QString msg) {
    if(!_initialized) return;
    QWARN(Q_CSTR(msg));
}


inline void ESTkTest::flagError(const QString msg) {
    if(!_initialized) return;
  //  QVERIFY2(false, Q_CSTR(msg));
    //_mutex->lock();

    //_mutex->unlock();
}

inline void ESTkTest::pushSubTest(const QString name) {
    if(!_initialized) return;
    _mutex->lock();
    _subTestStack.push(name);
    QString out;
    out += "*****BEGINNING SUBTEST: ";
    out += name;
    out += "*****";
  //  gyPrintf("%s\n", Q_CSTR(out));
    _mutex->unlock();
}

inline void ESTkTest::popSubTest(const bool success) {
    if(!_initialized) return;
    _mutex->lock();
    QString out;
    out +=  QString("*****");
    out += _subTestStack.top();
    out += ((success)? QString(" PASS") : QString(" FAIL"));
    out += QString("*****");
   // gyPrintf("%s\n", Q_CSTR(out));
    if(_subTestStack.size()) _subTestStack.pop();
    _mutex->unlock();
}


inline void ESTkTest::setESToolkit(ESToolkit*const estk) { _estk = estk; }
inline ESToolkit* ESTkTest::getESToolkit(void) { return _estk; }

inline void ESTkTest::enqueueNextTest(QObject *object) {
   // for(auto it = _testQueue.begin(); it != _testQueue.end(); ++it) {
     //   if(*it == this) {
       //     ++it;
            _testQueue.push_front(object);
         //   return;
       // }
   // }
}

inline void ESTkTest::enqueueTest(QObject* object) {
    _testQueue.push_back(object);
}

inline void ESTkTest::initialize(void) {
    _mutex = new QMutex();
    _initialized = true;
}

inline bool ESTkTest::currentProjectLevelAreaExist(void) {
    return (getCurrentProject() && getCurrentLevel() && getCurrentArea());
}


int main(int argc, char*argv[]);

}


#endif // TEST_ESTOOLKIT_H

