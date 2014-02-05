#ifndef GYRO_IOS_VIDEO_PRIVATE_H
#define GYRO_IOS_VIDEO_PRIVATE_H

#import "ios_system.h"
#import <UIKit/UIView.h>
@class GLViewController;
@interface GLView : UIView {
@private
	GLint backingWidth;
	GLint backingHeight;
	
	EAGLContext *context;
	NSTimer *animationTimer;
	NSTimeInterval animationInterval;
	GLuint viewRenderBuffer, viewFrameBuffer;
	
	GLuint depthRenderBuffer;
	GLViewController *controller;
	BOOL controllerSetup;
}
@property(nonatomic, assign) GLViewController *controller;
@property(nonatomic) NSTimeInterval animationInterval;

-(void)startAnimation;
-(void)stopAnimation;
-(void)drawView;
@end
#endif
