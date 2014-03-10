#import "ios_log.h"
#import "ios_system.h"
#import <string.h>
/*
#import <CoreFoundation/CFLogUtilities.h>
CFStringRef cfStringRef;
void IOS_LogWrite(const char *s, va_list a) {
	char buff[512] = {0,};
	vsprintf(buff,s,a);
	cfStringRef = CFStringCreateWithCStringNoCopy(NULL, buff,  kCFStringEncodingASCII, kCFAllocatorNull);
	CFLog(kCFLogLevelDebug,cfStringRef);
	CFRelease(cfStringRef);
}

void IOS_LogWrites(const char *s) {
	cfStringRef = CFStringCreateWithCStringNoCopy(NULL, s,  kCFStringEncodingASCII, kCFAllocatorNull);
	CFLog(kCFLogLevelDebug,cfStringRef);
	CFRelease(cfStringRef);
}
*/