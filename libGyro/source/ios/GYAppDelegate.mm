//
//  SGLAppDelegate.m
//  SimpleOpenGLInit
//
//  Created by Valentin Radu on 13/01/2013.
//  Copyright (c) 2013 Valentin Radu. All rights reserved.
//

#import "GYAppDelegate.h"
#import "GYViewController.h"

@implementation GYAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    
    GYViewController * ctrl = [[GYViewController alloc] init];
    [ctrl setPreferredFramesPerSecond:60];
    
    //these are the only three lines you
    //need to write in order to init your GLKView
    //while there are several options you could set here too,
    //you can actually skip them and go straight ahead to
    //SGLView (which is a subclass of GLKView by the way) drawRect: method
    //and start drawing in it
    
    //we create the context using the latest API
    EAGLContext * context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    [EAGLContext setCurrentContext:context];
    //we init our GLKView subclass with our newly created context
    GYView * view = [[GYView alloc] initWithFrame:self.window.frame context:context];
    
    
    
    ctrl.view = view;
    //[ctrl.view addSubView: view];
    [ctrl initTracking];

    self.window.rootViewController = ctrl;
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
