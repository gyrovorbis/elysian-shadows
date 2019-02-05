#include <gyro_texture_api.h>
#import <OpenGLES/ES3/gl.h>
#import <UIKit/UIKit.h>
#import <UIKit/UIImage.h>
#include <gyro_file_api.h>

const char* gyFilePath(const char *const name);

GLuint create_texture_png(const char *p, GLenum wrap, GLenum mag, GLenum min, unsigned *const success) {
    //char *imagePrefix = strtok(p, ".");
    *success = 1;

    /*NSString *path = [NSString stringWithCString:p length:strlen(p)];
    NSData *texData = [[NSData alloc] initWithContentsOfFile:path];
    UIImage *image = [[UIImage alloc] initWithData:texData];
    */

    //NSLog(@"PATH - %s\n", imagePrefix);


    //NSString *nsImagePrefix = [NSString stringWithCString:imagePrefix length:strlen(imagePrefix)];
    //NSString *imagePath = [[NSBundle mainBundle] pathForResource:nsImagePrefix ofType:@"png"];
    //NSLog(@"****iOS REEST IMAGE PATH - %s****\n", [imagePath cStringUsingEncoding:NSUTF8StringEncoding]);
    NSData *texData = [[NSData alloc] initWithContentsOfFile:[NSString stringWithUTF8String:gyFilePath(p)]];
    //if(!texData)
        //NSLog(@"Failed to load %s, shocker", p);
    UIImage *image = [[UIImage alloc] initWithData:texData];
    //GLuint errr = glGetError();
    //NSLog(@"error creating texture id - %d", errr);

    if (image == nil) {
       // NSLog(@"error creating UIImage");
        *success = 0;
        return 0;
    }

    GLuint id = 0;
    glGenTextures(1, &id);
    if(id == 0) {
        GLuint err = glGetError();
        //NSLog(@"error creating texture id - %d", err);
        *success = 0;
        //return 0;
    }
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    GLuint width = CGImageGetWidth(image.CGImage);
    GLuint height = CGImageGetHeight(image.CGImage);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    void *imageData = malloc( height * width * 4 );
    if(!imageData) {
        *success = 0;
    }
    CGContextRef context = CGBitmapContextCreate( imageData, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );

    CGColorSpaceRelease( colorSpace );
    CGContextClearRect( context, CGRectMake( 0, 0, width, height ) );
    CGContextTranslateCTM( context, 0, height - height );
    CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), image.CGImage );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    GLenum err = glGetError();
    if(err) {
        *success = 0;
    }

    CGContextRelease(context);

    free(imageData);
    [image release];
    [texData release];
    return id;
}

int _gyVidTexAlloc(GYTexture *const tex) {
    unsigned success = 0;
        *(GLuint*)(&tex->_data) = create_texture_png(tex->filename, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, &success);
  #if 0
    switch(tex->bpp) {
    case GY_TEX_16BPP:

        break;
    case GY_TEX_8BPP: //8bpp paletted texture

        break;
    case GY_TEX_4BPP: //4bpp paletted texture

        break;
    }
#endif

    return success;
}

int _gyVidTexFree(GYTexture *const tex) {
    glDeleteTextures(1, (GLuint*)&tex->_data);
    return 1;
}

