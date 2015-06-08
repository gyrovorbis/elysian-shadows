#ifndef TEST_ESTOOLKIT_H
#define TEST_ESTOOLKIT_H

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

class ESTkTestCommon {
protected:
    static ESToolkit*       _estk;
};


class ESToolkitTest: public QObject, public ESTkTestCommon, public AssetManager {


    Q_OBJECT
    friend int main(int arg, char*argv[]);
protected:

    static QList<QObject*>  _testQueue;
    static QStack<QString>  _subTestStack;
    static QMutex*          _mutex;
    static bool             _initialized;



    //test reload shaders
    //switch area via tabs
    //switch area via asset view
    //layer test (switch layers, lay tiles)


public:

    static void enqueueNextText(QObject* object);
    static void enqueueTest(QObject* object);
    static void pushSubTest(const QString name);
    static void popSubTest(const bool success = true);
    static void flagError(const QString msg);
    static void flagWarning(const QString msg);

    static void initialize(void);


    static void         setESToolkit(ESToolkit*const estk);
    static ESToolkit*   getESToolkit(void);


private Q_SLOTS:

    void createESToolkitMainWindow(void) {
        _estk = new ESToolkit;
        QVERIFY(_estk != nullptr);
    }
};

inline void ESToolkitTest::flagWarning(const QString msg) {
    if(!_initialized) return;
    QWARN(Q_CSTR(msg));
}


inline void ESToolkitTest::flagError(const QString msg) {
    if(!_initialized) return;
    QVERIFY2(false, Q_CSTR(msg));
    //_mutex->lock();

    //_mutex->unlock();
}

inline void ESToolkitTest::pushSubTest(const QString name) {
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

inline void ESToolkitTest::popSubTest(const bool success) {
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


inline void ESToolkitTest::setESToolkit(ESToolkit*const estk) { _estk = estk; }
inline ESToolkit* ESToolkitTest::getESToolkit(void) { return _estk; }

inline void ESToolkitTest::enqueueNextText(QObject *object) {
   // for(auto it = _testQueue.begin(); it != _testQueue.end(); ++it) {
     //   if(*it == this) {
       //     ++it;
            _testQueue.push_front(object);
         //   return;
       // }
   // }
}

inline void ESToolkitTest::enqueueTest(QObject* object) {
    _testQueue.push_back(object);
}

inline void ESToolkitTest::initialize(void) {
    _mutex = new QMutex();
    _initialized = true;
}


int main(int argc, char*argv[]);

}


#endif // TEST_ESTOOLKIT_H

