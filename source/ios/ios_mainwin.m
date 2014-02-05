#import "ios_mainwin.h"
#import "ios_video.h"
#import "ios_gl_controller.h"
#import "bootstrap.h"

#define RenderFreq 30.0

@implementation MainWindowz
@synthesize window;
@synthesize controller;
@synthesize updateInterval;
-(void)applicationDidFinishLaunching:(UIApplication *)app {
	CGRect rect = [[UIScreen mainScreen] bounds];
	rect.origin.x = rect.origin.y = 0.0;
	window = [[UIWindow alloc] initWithFrame:rect];
	controller = [[GLViewController alloc] initWithFrame:rect];
	updateInterval = 1.0f/30.0f;
	IOS_CallInitFunc();
	//[window addSubview:controller.view];
    [window setRootViewController:controller];
	[window makeKeyAndVisible];
	[self startUpdate];
}

-(void)callUpdate {
	IOS_CallUpdateFunc();
}

-(void)startUpdate {
	updateTimer = [NSTimer scheduledTimerWithTimeInterval:updateInterval target:self selector:@selector(callUpdate) userInfo:nil repeats:YES];
}

-(void)stopUpdate {
	[updateTimer invalidate];
	updateTimer = nil;
}

-(void)dealloc {
	[window dealloc];
	[controller dealloc];
	[super dealloc];
}

@end
