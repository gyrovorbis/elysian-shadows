#ifdef __unix__
#include <thread>
#include <X11/Xlib.h>
#endif
#include <gyro.h>

#include "elysian_engine.hpp"
#include "elysian_debug.hpp"
#include "elysian_behavior.hpp"
#include "elysian_lua_singleton.hpp"

#if GYRO_PLATFORM == GYRO_PLATFORM_iOS //reesty
#	import "ios/bootstrap.h"
#endif

using namespace elysian;

Engine *engine;
Debug *debug;

GYAppCbRet Init(int argc, char **argv) {
	(void)argc;
	(void)argv;

	/* The File subsystem needs to be initialized before anything else, ideally */
	gyFileInit("ESTeam", "ESGamma");

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

	engine = new Engine;

	engine->loadAssets();
	debug->setEngine(*engine);
	debug->sandboxInit();
	debug->initializeEngine();

	return GY_RET_CONTINUE;
}

GYAppCbRet Update(void) {
	engine->processInput();
	debug->update();
	engine->update();
	return GY_RET_CONTINUE;
};

GYAppCbRet Render(void) {
	engine->render();
	return GY_RET_CONTINUE;
}

static void StaticConstructor(void) {
	Behavior::staticConstructor();
}

void gyro_main(void)
{
#ifdef __unix__
	XInitThreads();
#endif
	StaticConstructor();
	gySysRegisterAppInit(&Init);
	gySysRegisterAppTick(&Update);
	gySysRegisterAppDraw(&Render);
}
