#import "GYViewController.h"
#include <gyro_system_api.h>

extern GYAppUpdateCb   gyAppUpdateCb;

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



@end
