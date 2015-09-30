#import "GYViewController.h"
#include <gyro_system_api.h>
#include <gyro_input_api.h>
#include <iostream>

using namespace std;

extern GYAppUpdateCb   gyAppUpdateCb;

static GYInput _input = {
    0, 0, 0, 0, 0, 1.0f, 1.0f, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }
};

static GYMouse _mouse;

// ==== PUBLIC C INPUT API =====



int gyKeyboardPoll(GYKeyboard**const keybd) {
    *keybd = NULL;
    return 1;
}

int gyMousePoll(GYMouse**const mouse) {
    *mouse = NULL;
    return 1;
}

int gyControllerPoll(GYController**const controller) {
    *controller = NULL;
    return 1;
}



int gyInputInit(const GYInputInitParams * const params) {
    return 1;
}

// ==== VIEW CONTROLLER OBJ-C IMPLEMENTATION =====

@interface GYViewController ()
- (void)handlePinchGesture:(UIPinchGestureRecognizer *)recognizer;

@end


@implementation GYViewController




- (void)initTracking {
 //   [super viewDidLoad];


    motionManager = [[CMMotionManager alloc] init];


    //accelerometer shiz
   /*if motionManager.accelerometerAvailable{
        queue = new NSOperationQueue();
        motionManager.startAccelerometerUpdatesToQueue(queue, withHandler: {


    }*/

    if (motionManager.accelerometerAvailable){
         queue = [[NSOperationQueue alloc] init];

         [motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue]
                                               withHandler:^(CMAccelerometerData  *accelerometerData, NSError *error) {
          [self accelerometerHandler:accelerometerData.acceleration];
            if(error){

             NSLog(@"%@", error);
            }
          }];
    }



    pinchGesture = [[UIPinchGestureRecognizer alloc]
                          initWithTarget:self
                          action:@selector(handlePinchGesture:)];
  //  [twoFingerPinch setDelegate:self];
     [self.view addGestureRecognizer:pinchGesture];

}

-(void)accelerometerHandler:(CMAcceleration)acceleration
{

    _input.accelerometer.x = acceleration.x;
    _input.accelerometer.y = acceleration.y;
    _input.accelerometer.z = acceleration.z;

}

- (void)update {
    if(gyAppUpdateCb) gyAppUpdateCb();
    //_input.down = 0;
   // _input.up = 0;
   // _input.left = 0;
   // _input.right = 0;
    _input.shake = 0;
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    if(interfaceOrientation == UIInterfaceOrientationLandscapeRight) {
        return YES;
    } else if(interfaceOrientation == UIInterfaceOrientationPortrait) {

        return YES;
    }
    return NO;
}

- (IBAction)handlePinchGesture:(UIPinchGestureRecognizer *)recognizer {

    _input.pinch = recognizer.scale;
    _gyLog(GY_DEBUG_VERBOSE, "recognizer.scale = %f", recognizer.scale);

}


- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
//    if(self.interfaceOrientation == UIInterfaceOrientationLandscapeRight) {
     //   [self.view setFrame:CGRectMake(0, 0, self.view.frame.size.height, self.view.frame.size.width)];
  //  }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    _input.up = _input.left = _input.right = _input.down = 0;
    const GYVector2 midPoint = { 0.5f*(float)self.view.frame.size.width, 0.5f*(float)self.view.frame.size.height };


    for (UITouch *aTouch in touches) {
        CGPoint pt = [aTouch locationInView:self.view];
        _input.cursorPos = { (float)pt.x, (float)pt.y };

        gyVec2Sub(&_input.cursorDir, &_input.cursorPos, &midPoint);
        _input.cursorMag = gyVec2MagAndNormalize(&_input.cursorDir);


        if(pt.x < 0.5f*self.view.frame.size.width) {
            _input.left = 1;

        } else if(pt.x > 0.5f*self.view.frame.size.width) {
            _input.right = 1;
        }

        if(pt.y < 0.5f*self.view.frame.size.height) {
            _input.up = 1;
        } else if(pt.y > 0.5f*self.view.frame.size.height){
            _input.down = 1;
        }
    }

}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    _input.up = _input.left = _input.right = _input.down = 0;
     const GYVector2 midPoint = { 0.5f*(float)self.view.frame.size.width, 0.5f*(float)self.view.frame.size.height };

    for (UITouch *aTouch in touches) {
        CGPoint pt = [aTouch locationInView:self.view];

        _input.cursorPos = { (float)pt.x, (float)pt.y };

        gyVec2Sub(&_input.cursorDir, &_input.cursorPos, &midPoint);
        _input.cursorMag = gyVec2MagAndNormalize(&_input.cursorDir);


        if(pt.x < 0.5f*self.view.frame.size.width) {
            _input.left = 1;
        } else if(pt.x > 0.5f*self.view.frame.size.width) {
            _input.right = 1;
        }

        if(pt.y < 0.5f*self.view.frame.size.height) {
            _input.up = 1;
        } else if(pt.y > 0.5f*self.view.frame.size.height){
            _input.down = 1;
        }
    }

}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
        _input.up = _input.left = _input.right = _input.down = 0;
        _input.cursorDir = GYVector2 { 0.0f, 0.0f };
#if 0
    for (UITouch *aTouch in touches) {
        CGPoint pt = [aTouch locationInView:self.view];

        if(pt.x < 0.5f*self.view.frame.size.width) {
            _input.left = 0;
        } else if(pt.x > 0.5f*self.view.frame.size.width) {
            _input.right = 0;
            //_gyLog(GY_DEBUG_VERBOSE, "%d", pt.x);
            cout << "TESTING    " << pt.x << endl;
        }

        if(pt.y < 0.5f*self.view.frame.size.height) {
            _input.up = 0;
        } else if(pt.y > 0.5f*self.view.frame.size.height){
            _input.down = 0;
        }
    }
#endif
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {

}


- (void)motionBegan:(UIEventSubtype)motion withEvent:(UIEvent*)event {
    if(event.type == UIEventSubtypeMotionShake) {
        _input.shake = 1;
    }
}



@end
