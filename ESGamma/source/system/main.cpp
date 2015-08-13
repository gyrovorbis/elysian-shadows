#include <gyro_system_api.h>
#include <gyro_video_api.h>
#include <gyro_input_api.h>
#include <gyro_audio_api.h>
#include <gyro_file_api.h>
#include <system/elysian_engine.hpp>
#include <system/crtp/elysian_camera_system.hpp>
#include <crtp/elysian_debug.hpp>
#include "elysian_behavior.hpp"
#include "elysian_lua_singleton.hpp"

static elysian::Engine* engine  = NULL;
static elysian::Debug*  debug   = NULL;

int main(int argc, char* argv[]) {
    debug = &elysian::Debug::getInstance();

    gySysRegisterAppInit([](int argc, char* argv[]) {
        (void)argc;
        (void)argv;

        /* The File subsystem needs to be initialized before anything else, ideally */
        gyFileInit(nullptr);

        char verString[50];

        debug->parseArguments();
        strcpy(verString, "ESGamma v");
        strcat(verString, GYRO_LIB_VERSION);

        GYVidInitParams vidParams =
    #if GYRO_PLATFORM == GYRO_PLATFORM_x86
        { (GY_VID_RESOLUTION)debug->getCmdLineArgs().resolution, GY_WIN_RESIZABLE, 480, 270, 1280, 720, verString, "icon.bmp"};
    #elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID
        { GY_VID_1280x720, GY_WIN_FULLSCREEN, 1280, 720, 1280, 720 };
    #elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
        { };
    #elif GYRO_PLATFORM == GYRO_PLATFORM_DC
        { 0 };
    #elif GYRO_PLATFORM == GYRO_PLATFORM_PSP

    #endif

        GYSysInitParams sysParams =
    #if GYRO_PLATFORM == GYRO_PLATFORM_x86
        { (GY_VID_RESOLUTION)debug->getCmdLineArgs().resolution, GY_WIN_RESIZABLE, 480, 270, 1280, 720, verString, "icon.bmp"};
    #elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID
        { GY_VID_1280x720, GY_WIN_FULLSCREEN, 1280, 720, 1280, 720 };
    #elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
        { };
    #elif GYRO_PLATFORM == GYRO_PLATFORM_DC
        { 0 };
    #elif GYRO_PLATFORM == GYRO_PLATFORM_PSP

    #endif


        GYInputInitParams inputParams =
    #if GYRO_PLATFORM == GYRO_PLATFORM_x86
        { (GY_VID_RESOLUTION)debug->getCmdLineArgs().resolution, GY_WIN_RESIZABLE, 480, 270, 1280, 720, verString, "icon.bmp"};
    #elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID
        { GY_VID_1280x720, GY_WIN_FULLSCREEN, 1280, 720, 1280, 720 };
    #elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
        { };
    #elif GYRO_PLATFORM == GYRO_PLATFORM_DC
        { 0 };
    #elif GYRO_PLATFORM == GYRO_PLATFORM_PSP

    #endif

        gySysInit(&sysParams);
        gyVidInit(&vidParams);
        gyInputInit(&inputParams);
        gyAudInit();
        gyTimerInit();

        engine = new elysian::Engine;

        engine->loadAssets();
        debug->setEngine(*engine);
        debug->parseArguments();
        debug->sandboxInit();
        debug->initializeEngine();

        return GY_RET_CONTINUE;


    });

    gySysRegisterAppUpdate([](void) {
        if(engine->getCurrentArea()) {
            engine->processInput();
            debug->update();
            engine->update();
        }
        return GY_RET_CONTINUE;


    });

    gySysRegisterAppInput([](void) {
        return GY_RET_CONTINUE;


    });

    gySysRegisterAppDraw([](void) {
        if(engine->getCurrentArea()) {
            engine->getCameraSys()->initializeProjMatrix();
            engine->render();
        }
        return GY_RET_CONTINUE;

    });


    return gySysAppExec(argc, argv);
}
