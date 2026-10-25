/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GY_VIEW_CONTROLLER_H
#define GY_VIEW_CONTROLLER_H

#import <GLKit/GLKit.h>
#import <CoreMotion/CoreMotion.h>

@interface GYViewController : GLKViewController {

    CMMotionManager *motionManager;
    UIPinchGestureRecognizer* pinchGesture;
    NSOperationQueue *queue;

};


- (void)update;
- (void)initTracking;
- (BOOL)shouldAutorotateToInterfaceOrientation: (UIInterfaceOrientation) interfaceOrientation;
- (void)didRotateFromInterfaceorientation: (UIInterfaceOrientation)fromInterfaceOrientation;
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)motionBegan:(UIEventSubtype)motion withEvent:(UIEvent*)event;


@end

#endif // GYVIEWCONTROLLER_H

