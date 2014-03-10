#import "ios_system.h"
#import "ios_video.h"
#import "ios_gl_controller.h"

void IOS_TouchesBegan(void *set, void *ui);
void IOS_TouchesMoved(void *set, void *ui);
void IOS_TouchesEnded(void *set, void *ui);
void IOS_TouchesCancelled();


@interface GLView (private)
	-(id)initGLES;
	-(BOOL)createFrameBuffer;
	-(void)destroyFrameBuffer;
	-(void)startDraw;
	-(void)EndDraw;
@end

@implementation GLView


+ (Class) layerClass {
	return [CAEAGLLayer class];
}

-(id)initWithFrame:(CGRect)frame {
	self = [super initWithFrame:frame];
	if(self != nil) {
		self = [self initGLES];
		
		 CADisplayLink* displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];    
	}
	return self;
}

-(id)initGLES {
	CAEAGLLayer *eaglLayer = (CAEAGLLayer*) self.layer;
	eaglLayer.opaque = YES;
	
	/* Don't really know what the fuck this is for.
	eaglLayer.drawableProperties = 
		[NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
			kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];*/

	context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
	if(!context || ![EAGLContext setCurrentContext:context] || ![self createFrameBuffer]) {
		NSLog(@"initGLES error!");
		[self release];
		return nil;
	}
	return self;
}

-(void)layoutSubviews {
	[EAGLContext setCurrentContext:context];
	[self destroyFrameBuffer];
	[self createFrameBuffer];
	[self drawView];
}

-(BOOL)createFrameBuffer {
	glGenFramebuffersOES(1, &viewFrameBuffer);
	glGenRenderbuffersOES(1, &viewRenderBuffer);
	NSLog(@"view frame %d, view render %d", viewFrameBuffer, viewRenderBuffer);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFrameBuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderBuffer);

	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderBuffer);

	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);

	NSLog(@"backing width %d, backing height %d", backingWidth, backingHeight);

	glGenRenderbuffersOES(1, &depthRenderBuffer);
	NSLog(@"depth render %d", depthRenderBuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderBuffer);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderBuffer);

	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	GLenum err = glGetError();
	if(err)
		NSLog(@"createFramebuffer GL error %x", err);

	return YES;

}

-(void)destroyFrameBuffer {
	NSLog(@"Destroying frame buffers!");
	glDeleteFramebuffersOES(1, &viewFrameBuffer);
	viewFrameBuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderBuffer);
	viewRenderBuffer = 0;

	if(depthRenderBuffer) {
		glDeleteRenderbuffersOES(1, &depthRenderBuffer);
		depthRenderBuffer = 0;
	}
}

- (void)render:(CADisplayLink*)displayLink {

}

-(void)StartDraw {
	[EAGLContext setCurrentContext:context];
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFrameBuffer);
	GLenum err = glGetError();
	if(err)
		NSLog(@"StartDraw GL error %x", err);
}

-(void)EndDraw {
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderBuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];

	GLenum err = glGetError();
	if(err)
		NSLog(@"%x GL error", err);

}

-(void)drawView {
	if(!controllerSetup) {
		[controller setupView:self];
		controllerSetup = YES;
	}
	[self StartDraw];
	[controller drawView:self];
	[self EndDraw];
}

-(GLViewController*)controller {
	return controller;
}

-(void)setController:(GLViewController*)d {
	controller = d;
	controllerSetup = ![controller respondsToSelector:@selector(setupView:)];
}

-(void) startAnimation {
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}

-(void) stopAnimation {
	[animationTimer invalidate];
	animationTimer = nil;
}

-(void) setAnimationInterval:(NSTimeInterval)interval {
	animationInterval = interval;
	if(animationTimer) {
		[self stopAnimation];
		[self startAnimation];
	}
}

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event {
	IOS_TouchesBegan(touches, self);
}

-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {
	IOS_TouchesMoved(touches, self);
}

-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event {
	IOS_TouchesEnded(touches, self);
}
@end
