#ifndef GYRO_IOS_GL_CONTROLLER_H
#define GYRO_IOS_GL_CONTROLLER_H

#import "ios_system.h"

@class GLView;
@interface GLViewController : UIViewController {
	CGRect rect;
	GLView *glview;
	BOOL touched;
}
@property (nonatomic, retain) GLView *glview;
//@property(nonatomic, retain) CGRect rect;
-(id)initWithFrame:(CGRect)_rect;
-(void)drawView:(GLView*)v;
-(void)setupView:(GLView*)v;
@end

#endif
