#import "ios_gl_controller.h"
#import "ios_video.h"
#import "bootstrap.h"
#import "gyro_log.h"

#define RenderFreq 60.0

void IOS_RenderTouchAreas(UIView *v);

@implementation GLViewController
@synthesize glview;

-(id)initWithFrame:(CGRect)_rect {
	rect = _rect;
	LogWrite("rect.width %f, rect.height %f", rect.size.width, rect.size.height);
	if((self = [super init])) {
		glview = [[GLView alloc] initWithFrame:rect];
		glview.controller = self;
		glview.alpha = 0.0;
		glview.animationInterval = 1.0 / RenderFreq;
		glview.multipleTouchEnabled = YES;
		glview.exclusiveTouch = YES;
		return self;
	}
	return nil;
}

-(void)loadView {
	NSLog(@"loadView called");
	self.view = [[UIView alloc] initWithFrame:rect];
	self.view.backgroundColor = [UIColor blackColor];
}

-(void)viewDidLoad {
	NSLog(@"viewDidLoad called");
        [UIView beginAnimations:nil];
	[UIView setAnimationCurve:UIViewAnimationCurveEaseIn];
	[UIView setAnimationDuration:0.50];
	[self.view addSubview:self.glview];
	self.glview.alpha = 1.0;
	[UIView endAnimations];
	[self.glview startAnimation];
}

-(void)drawView:(GLView*)v {
	IOS_CallRenderFunc();
}

-(void)setupView:(GLView*)v {
	if(self.interfaceOrientation == UIInterfaceOrientationPortrait)
		IOS_CallInitRenderFunc(rect.size.width, rect.size.height);
	else if(self.interfaceOrientation == UIInterfaceOrientationLandscapeRight)
		IOS_CallInitRenderFunc(rect.size.height, rect.size.width);
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	if(interfaceOrientation == UIInterfaceOrientationLandscapeRight) {
        	return YES;
	}
	return NO;
}

-(void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
	if(self.interfaceOrientation == UIInterfaceOrientationLandscapeRight) {
		[self.glview setFrame:CGRectMake(0, 0, rect.size.height, rect.size.width)];
	}
}

-(void)dealloc {
	[glview dealloc];
	[super dealloc];
}

@end
