#import "GYView.h"
#include <gyro_system_api.h>
#include <glal/gyro_video_glal.h>

@implementation GYView{
    GLKBaseEffect * effect;
    GLuint vertexArray;
    GLuint vertexBuffer;
    GLuint indexBuffer;
}

extern GYAppInitCb     gyAppInitCb;
extern GYAppUpdateCb   gyAppUpdateCb;
extern GYAppInputCb    gyAppInputCb;
extern GYAppDrawCb     gyAppDrawCb;
extern GYAppQuitCb     gyAppQuitCb;


- (id)initWithFrame:(CGRect)frame context:(EAGLContext *)context
{
    self = [super initWithFrame:frame context:context];
    if (self) {
        

        if(gyAppInitCb) gyAppInitCb(0, NULL);
        glalVidOglInit(frame.size.width, frame.size.height, frame.size.width, frame.size.height);



    }
    return self;
}


- (void)drawRect:(CGRect)rect
{

    //clear the screen
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
     glalVidOglInit(rect.size.width, rect.size.height, rect.size.width, rect.size.height);
    if(gyAppDrawCb) gyAppDrawCb();
    

}

-(void)dealloc
{

   // glDeleteVertexArraysOES(1, &vertexArray);

}

@end
