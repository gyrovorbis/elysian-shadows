#import "GYViewController.h"
#include <gyro_system_api.h>
#include <gyro_input_api.h>

extern GYAppUpdateCb   gyAppUpdateCb;
static GYInput _input;
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

}

- (void)update {
    if(gyAppUpdateCb) gyAppUpdateCb();
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
        _gyLog(GY_DEBUG_VERBOSE, "Touched some shit: <%f, %f>", pt.x, pt.y);

    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {

}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {

}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {

}

@end
