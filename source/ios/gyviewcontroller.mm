#import "GYViewController.h"
#include <gyro_system_api.h>
#include <gyro_input_api.h>
#include <iostream>

using namespace std;

extern GYAppUpdateCb   gyAppUpdateCb;

static GYInput _input = {
    0, 0, 0, 0
};

static GYMouse _mouse;

// ==== PUBLIC C INPUT API =====

void gyInputPoll(GYInput** const input) {
    *input = &_input;
}

void gyMousePoll(GYMouse* const mouse) {
    (void)mouse;
}

int gyInputInit(const GYInputInitParams * const params) {
    return 1;
}

// ==== VIEW CONTROLLER OBJ-C IMPLEMENTATION =====

@implementation GYViewController




- (void)viewDidLoad {
    [super viewDidLoad];

    // Set animation frame rate
    self.preferredFramesPerSecond = 60;


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
          [self outputAccelertionData:accelerometerData.acceleration];
                                                   if(error){

                                                       NSLog(@"%@", error);
                                             }
          }];
    }

}

-(void)outputAccelertionData:(CMAcceleration)acceleration
{
     cout<<"X = "<<acceleration.x<<endl;
      cout<<"Y = "<<acceleration.y<<endl;
      cout<<"Z = "<<acceleration.z<<endl;
}

- (void)update {
    if(gyAppUpdateCb) gyAppUpdateCb();
    memset(&_input, 0, sizeof(GYInput));
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    if(interfaceOrientation == UIInterfaceOrientationLandscapeRight) {
            return YES;
    }
    return NO;
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
    if(self.interfaceOrientation == UIInterfaceOrientationLandscapeRight) {
        [self.view setFrame:CGRectMake(0, 0, self.view.frame.size.height, self.view.frame.size.width)];
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *aTouch in touches) {
        CGPoint pt = [aTouch locationInView:self.view];

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
    for (UITouch *aTouch in touches) {
        CGPoint pt = [aTouch locationInView:self.view];

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
    for (UITouch *aTouch in touches) {
        CGPoint pt = [aTouch locationInView:self.view];

        if(pt.x < 0.5f*self.view.frame.size.width) {
            _input.left = 1;
        } else if(pt.x > 0.5f*self.view.frame.size.width) {
            _input.right = 1;
            //_gyLog(GY_DEBUG_VERBOSE, "%d", pt.x);
            cout << "TESTING    " << pt.x << endl;
        }

        if(pt.y < 0.5f*self.view.frame.size.height) {
            _input.up = 1;
        } else if(pt.y > 0.5f*self.view.frame.size.height){
            _input.down = 1;
        }
    }

}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {

}

@end
