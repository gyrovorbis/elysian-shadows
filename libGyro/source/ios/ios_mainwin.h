#ifndef LIBGYRO_IOS_MAINWIN_H
#define LIBGYRO_IOS_MAINWIN_H

#import <CoreFoundation/CoreFoundation.h>
#import <UIKit/UIKit.h>
#import "ios_video.h"

@interface MainWindowz : NSObject <UIAccelerometerDelegate, UIApplicationDelegate> {
	UIWindow *window;
	GLViewController *controller;
@private
	NSTimer *updateTimer;
	NSTimeInterval updateInterval;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet GLViewController *controller;
@property NSTimeInterval updateInterval;

-(void)startUpdate;
-(void)stopUpdate;
-(void)applicationDidFinishLaunching: (UIApplication *)app;
-(void)callUpdate;
@end
#endif
