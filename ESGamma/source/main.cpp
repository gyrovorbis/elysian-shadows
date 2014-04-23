#ifdef __unix__
#include <thread>
#include <X11/Xlib.h>
#endif
#include <gyro.h>

#include "elysian_engine.hpp"
#include "elysian_debug.hpp"
#include "elysian_behavior.hpp"
#include "elysian_lua_singleton.hpp"

#include "elysian_subprocess_manager.hpp"

#if GYRO_PLATFORM == GYRO_PLATFORM_iOS //reesty
#	import "ios/bootstrap.h"
#endif

using namespace elysian;

Engine *engine;
Debug *debug;


class SystemProcessManager;

class RandomProcess: public elysian::SubProcess<RandomProcess, SystemProcessManager> {
public:
	void update(SystemProcessManager *m) {printf("UPDATE"); }
	void start(SystemProcessManager * const m) {printf("START"); }
	void stop(SystemProcessManager * const m) { printf("STOP");}

};

class RandomProcess2: public elysian::MonitorableSubProcess<RandomProcess, SystemProcessManager> {
public:
	void update(SystemProcessManager *m) {printf("MONUPDATE"); }
	void start(SystemProcessManager * const m) {printf("MONSTART"); }
	void stop(SystemProcessManager * const m) { printf("MONSTOP");}

};



class SystemProcessManager: public elysian::SubProcessManager<SystemProcessManager, RandomProcess, RandomProcess, RandomProcess2>
{

public:
	void update(void) { updateSubProcesses(); }
};

static void Init(int argc, char *argv[]) {

	SystemProcessManager proc;
	//get<0>(proc).update(&proc);
	//get<1>(proc).start(&proc);
	//get<2>(proc).stop(&proc);
	proc.update();

	(void)argc;
	(void)argv;
	debug = &Debug::getInstance();
	char verString[50];

	debug->parseArguments();
	strcpy(verString, "ESGamma v");
	strcat(verString, GYRO_LIB_VERSION);

	GYVidInitParams vidParams =
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
	{ (GY_VID_RESOLUTION)debug->getCmdLineArgs().resolution, GY_WIN_RESIZABLE, 480, 270, 1280, 720, verString, "icon.bmp"};
#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
	{ 0 };
#elif GYRO_PLATFORM == GYRO_PLATFORM_DC
	{ 0 };
#elif GYRO_PLATFORM == GYRO_PLATFORM_PSP

#endif

	gySysInit();
	gyVidInit(&vidParams);
	gyAudInit();
	gyInputInit();
	gyAudInit();
	gyTimerInit();
	gyFileInit();

	engine = new Engine;

	engine->loadAssets();
	debug->setEngine(*engine);
	debug->sandboxInit();
	debug->initializeEngine();

}

int Update(void) {
	debug->update();
	engine->update();
	return 0;
};

void Render(void) {
	engine->render();
}

static void ProcessInput(void) {
	engine->processInput();
}

static void StaticConstructor(void) {
	Behavior::staticConstructor();
}

/* Everything is in a global function, so that switching LibGyro to be event-driven for
   Droid and iOS reest will be less painful.
 */
int main(int argc, char *argv[]) {

#ifdef __unix__
	XInitThreads();
#endif
	StaticConstructor();

#if GYRO_PLATFORM == GYRO_PLATFORM_iOS
	IOS_SetInitFunc(Init);
	IOS_SetRenderFunc(Render);
	IOS_SetUpdateFunc(Update);
	CallUIApplicationMain(argc, argv);

#else

	Init(argc, argv);

	for(;;) {
		if(Update() == -1) break;
		Render();
		ProcessInput();
	}

#endif
	return 0;
}
