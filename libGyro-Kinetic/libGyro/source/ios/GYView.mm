/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

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


static GYView* _view = NULL;


int glalVidDefaultFbo(void) {
    return _view->_defaultFbo;

}

void glalVidBindDefaultFbo(void) {
    //fucking disgusting.
   // glBindFramebuffer(GL_FRAMEBUFFER, glalDefaultFbo());
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    [_view bindDrawable];
}


- (id)initWithFrame:(CGRect)frame context:(EAGLContext *)context
{
    self = [super initWithFrame:frame context:context];
    if (self) {
        

        if(gyAppInitCb) gyAppInitCb(0, NULL);
        glalVidOglInit(frame.size.width, frame.size.height, frame.size.width, frame.size.height);

        _view = self;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_view->_defaultFbo);


    }
    return self;
}


- (void)drawRect:(CGRect)rect
{
   // display.
    //clear the screen
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    //if([[UIDevice currentDevice] orientation] == ) {
        glalVidOglInit(rect.size.width, rect.size.height, rect.size.width, rect.size.height);
    //} else if([[UIDevice currentDevice] orientation] == ) {

    //}
    if(gyAppDrawCb) gyAppDrawCb();
    

}

-(void)dealloc
{

   // glDeleteVertexArraysOES(1, &vertexArray);

}

@end
