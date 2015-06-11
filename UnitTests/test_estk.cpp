
#include <QtTest>

#include <tk_estoolkit.h>
#include <test_estoolkit.h>
#include <test_project.h>
#include <test_map_view.h>
#include <test_libgyro_file_api.h>

/*
 *
 * *SEPARATE ENGINE/TOOLKIT/CORE tests from functional tests*

UNIT TESTS TO WRITE
1) Instantiate ESTk
2) Initialize all of libGyro
3) Load shaders
4) Switch areas via tabs (currently broken)
5) authenticate to Gitlab server
6) LuaTableProxy? (serialize, deserialize, register, access complex types, etc)
7) Setting up entities, adding and removing components, serialize, deserialize, etc.
8) area/level load and save (fuck with attributes via lua first, basic map modification)
9) mem pool shit
10) LuaManager push/pop tests, test table iterations (generic iterate), check stack balancing
11) Lua Behavior basic tests
11.5) Lua Behavior Script configurations (inheritance, parent scripts, overriding methods, scoping, etc)
12) collisions
13) Adding/removing lights
14) allocating/deleting textures
15) allocating/playing audio files
16) spine files
17) Check GL errors after render call(s)


CREATE QUEUE WITH STATES OF INSERTION
- auto insert tests upon static initialization?
- have enums for state: pre project, post project, post engine, post area, blah?


 *
 *
 */



namespace tktest {

ESToolkit* ESTkTestCommon::_estk             = nullptr;
QList<QObject*> ESToolkitTest::_testQueue;
QStack<QString> ESToolkitTest::_subTestStack;
QMutex* ESToolkitTest::_mutex = nullptr;
bool ESToolkitTest::_initialized = false;

int main(int argc, char** argv)
{
    Debug::setLevel(Debug::OFF);
    ESToolkitTest::initialize();


    QCommandLineParser parser;
    parser.setApplicationDescription("ESToolkit Command-Line Mode");
    parser.addHelpOption();
    parser.addVersionOption();

    qDebug() << "Current working directory:" << QDir::currentPath();
    QCommandLineOption testOption("project", "Set project working directory", "directory");
    parser.addOption(testOption);

    QStringList args = QApplication::arguments();
    //parser.process(args);

    QString project;

    for(auto it = args.begin(); it != args.end();  ++it) {
        if(*it == "--project") {
            project = (*++it);
            args.erase(it);
            break;
        }
    }

    for(auto it = args.begin(); it != args.end();  ++it) {
        if(*it == "--project") {
            args.erase(it);
            break;
        }
    }

    args.push_back(QString("-o"));
    args.push_back(QString("ESTkTestResults0000.xml,xml"));

    qDebug() << "Using project directory supplied as command-line argument:" << project;

    ESToolkitTest::enqueueTest(new LibGyroFileApiTest());
    ESToolkitTest::enqueueTest(new tktest::ESToolkitTest());
    ESToolkitTest::enqueueTest(new tktest::TestSmokeProject(project));

   int failures = 0;
   int testCases = 0;
   auto ASSERT_TEST = [&failures, &args](QObject* obj) {
        failures += QTest::qExec(obj, args);
        delete obj;
    };//testestestests

   //Clear old test data
    char strBuff[200];
    int i = -1;
    do {
        sprintf(strBuff, "ESTkTestResults%04d.xml", ++i);
    } while(QFile::exists(strBuff) && QFile::remove(strBuff));



   while(ESToolkitTest::_testQueue.size()) {
       sprintf(strBuff, "./ESTkTestResults%04d.xml", ++testCases);

       QObject *current =  ESToolkitTest::_testQueue.takeFirst();

       ASSERT_TEST(current);

       if(!QFile::exists("ESTkTestResults0000.xml")) {
            qDebug() << "DOESN't EXIST";
       }

       QFile lolz("ESTkTestResults0000.xml");
       if(!lolz.isReadable()) {
        qDebug() << "NOT READABLE";
       }
       //if(lolz.is
        //qDebug() << "IS OPEN!";
       //}

       if(!QFile::copy("ESTkTestResults0000.xml", strBuff)) {
        qDebug() << "FAILED COPY!!";
       }

   }

   char buff[7];
   sprintf(buff, "%d", failures);

    qDebug() << "FUCKING FUCK!!!!!" << QString(buff) << "errors!";
    qDebug() << ((failures)? "GO FIX ASSETS RIGHT FUCKING NOW!": "WEEEEEEELL! GOOD FUCKING JOB, NIGGAS!");

   return failures;
}

}

