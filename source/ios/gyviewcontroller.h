#ifndef GY_VIEW_CONTROLLER_H
#define GY_VIEW_CONTROLLER_H

#import <GLKit/GLKit.h>

@interface GYViewController : GLKViewController {



};


-(void)update;
-(void)viewDidLoad;
-(BOOL)shouldAutorotateToInterfaceOrientation:
    (UIInterfaceOrientation) interfaceOrientation;

@end

#endif // GYVIEWCONTROLLER_H

