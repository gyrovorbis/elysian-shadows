#import <assert.h>
#import "gyro_file.h"
#import "gyro_system.h"
#import <Foundation/NSBundle.h>
#import <Foundation/NSString.h>
#import <string.h>
#import <stdlib.h>
//#define GYRO_iOS_TEMP_FILENAME_BUFFER_SIZE 500

static NSBundle *_appBundle = NULL;
//static char _tempFileBuffer[GYRO_iOS_TEMP_FILENAME_BUFFER_SIZE];

// Stole this shit form the internet, because doing this safely is a bitch,
// and there was no way in FUCK I would want to do it in Obj-C.
// Oh, and I adulterated the shit out of it, since iOS doesn't want a leading '.'
// on the extensions. --Gyro
typedef struct
{
    char * directory;
    char * filename;
    char * extension;
} PATHINFO;


// Allocate a block of memory that holds the PATHINFO at the beginning
// followed by the actual path. An extra byte is allocated (+2 instead
// of just +1) to deal with shifting the extension to protect the
// leading '.'. This extra byte also doubles as the empty string, as
// well as a pad to keep from reading past the memory block.
//
// Returns a PATHINFO structure that should be freed using FreePathInfo().
static PATHINFO * _SplitPath(const char * pathSrc)
{
    size_t length = strlen(pathSrc);
    PATHINFO * pathinfo = (PATHINFO *) malloc(sizeof(PATHINFO) + length + 2);
    
    if (pathinfo)
    {
        char * path = (char *) &pathinfo[1];    // copy of the path
        char * theEnd = &path[length + 1];      // second null terminator
        char * extension;
        char * lastSep;
        
        // Copy the original string and double null terminate it.
        strcpy(path, pathSrc);
        *theEnd = '\0';
        pathinfo->directory = theEnd;   // Assume no path
        pathinfo->extension = theEnd;   // Assume no extension
        pathinfo->filename = path;      // Assume filename only
        
        lastSep = strrchr(path, '/');
        if (lastSep)
        {
            pathinfo->directory = path;     // Pick up the path
            *lastSep++ = '\0';              // Truncate directory
            pathinfo->filename = lastSep;   // Pick up name after path  
        }
        
        // Start at the second character of the filename to handle
        // filenames that start with '.' like ".login".
        // We don't overrun the buffer in the cases of an empty path
        // or a path that looks like "/usr/bin/" because of the extra
        // byte.
        extension = strrchr(&pathinfo->filename[1], '.');
        if (extension)
        {
            ++extension;
            // Shift the extension over to protect the leading '.' since
            // we need to truncate the filename.
            //memmove(extension + 1, extension, strlen(extension));
            pathinfo->extension = extension;// + 1;
            
            *(extension-1) = '\0';          // Truncate filename
        }
    }
    
    return pathinfo;
}


int gyFileInit(void) {
    _appBundle = [NSBundle mainBundle];
    return (_appBundle)? 1 : 0;
}


const char* gyFilePath(const char *const name) {
    assert(_appBundle); //initialize the API first, fuckmite!
    
    PATHINFO *pathInfo = _SplitPath(name);
    
    NSString *fullPath = [_appBundle pathForResource:[NSString stringWithUTF8String:pathInfo->filename] ofType:[NSString stringWithUTF8String:pathInfo->extension] inDirectory:[NSString stringWithUTF8String:pathInfo->directory]];
    
    free(pathInfo);
    
    if(!fullPath) {
        _gyLog(GY_DEBUG_CRITICAL, "! - Unable to locate resource in iOS bundle! - %s", name);
        return "SHIT IS FUCKED!";
    }
    
    return [fullPath cStringUsingEncoding:NSUTF8StringEncoding];
}
