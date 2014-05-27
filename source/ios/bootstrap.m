
#import <CoreFoundation/CoreFoundation.h>
#import <UIKit/UIKit.h>
//#import "ios_mainwin.h"
//#import "gyro_log.h"
#import <dirent.h>
#include "bootstrap.h"
#import "gyro_file_api.h"
#include "gyro_video_glal.h"
#include "libGyroAppDelegate.h"

_RenderFunc RenderFunc;
_UpdateFunc UpdateFunc;
_InitFunc InitFunc;

const char *app_name = "ESGamma";

void IOS_SetInitFunc(_InitFunc func) {
//	LogWrite("set InitFunc to %p\n", func);
	InitFunc = func;
}
void IOS_CallInitFunc(void) {
//	LogWrite("calling InitFunc with argc:1, argv: %s\n", app_name);
	InitFunc(1,(char **)&app_name);
}

void IOS_CallInitRenderFunc(int width, int height) {
	//LogWrite("Calling InitRenderFunc with width:%d, height:%d\n", width, height);
        //InitRenderFunc(width, height);
//    gyVidOglInit(width, height, width, height);
    
}

void IOS_SetRenderFunc(_RenderFunc func) {
//	LogWrite("set RenderFunc to %p\n", func);
	RenderFunc = func;
}

void IOS_CallRenderFunc(void) {
//	LogWrite("Calling RenderFunc");
	RenderFunc();
}

void IOS_SetUpdateFunc(_UpdateFunc func) {
//	LogWrite("set UpdateFunc to %p\n", func);
	UpdateFunc = func;
}

void IOS_CallUpdateFunc(void) {
//	LogWrite("Calling UpdateFunc");
	UpdateFunc();
}

int CallUIApplicationMain(int argc, char *argv[]) {
    
	@autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([libGyroAppDelegate class]));
    }
}
