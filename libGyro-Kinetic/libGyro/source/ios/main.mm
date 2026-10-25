//
//  main.m
//  SimpleOpenGLInit
//
//  Created by Valentin Radu on 13/01/2013.
//  Copyright (c) 2013 Valentin Radu. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GYAppDelegate.h"
#include <gyro_system_api.h>

extern GYAppInitCb     gyAppInitCb;
extern GYAppUpdateCb   gyAppUpdateCb;
extern GYAppInputCb    gyAppInputCb;
extern GYAppDrawCb     gyAppDrawCb;
extern GYAppQuitCb     gyAppQuitCb;

int gySysAppExec(int argc, char* argv[]) {

    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([GYAppDelegate class]));
    }

}

#if 0
int main(int argc, char *argv[])
{
    _gyLog(GY_DEBUG_VERBOSE, "LOLOL");

    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([GYAppDelegate class]));
    }
}
#endif
