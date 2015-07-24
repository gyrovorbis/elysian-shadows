#ifndef GY_VIEW_CONTROLLER_H
#define GY_VIEW_CONTROLLER_H

#import <GLKit/GLKit.h>
#import <CoreMotion/CoreMotion.h>

@interface GYViewController : GLKViewController {

CMMotionManager *motionManager;
NSOperationQueue *queue;

};


- (void)update;
- (void)viewDidLoad;
- (BOOL)shouldAutorotateToInterfaceOrientation: (UIInterfaceOrientation) interfaceOrientation;
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;

@end

#endif // GYVIEWCONTROLLER_H

