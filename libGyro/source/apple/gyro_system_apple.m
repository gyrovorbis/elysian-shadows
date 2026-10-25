#include <Foundation/Foundation.h>
#include <sys/utsname.h>
#include <string.h>


void _gyQueryOperatingSystemInfoString(char* buffer, int buffSize) {
    char linuxBuff[buffSize];
    char macBuff[buffSize];
    NSString* version = [[NSProcessInfo processInfo] operatingSystemVersionString];
    assert(version);

    if(![version getCString:macBuff maxLength:buffSize encoding:NSUTF8StringEncoding]) {
        assert(0);
    }

    struct utsname uts;
    if(uname(&uts)) {
        linuxBuff[0] = 0;
    } else {
        snprintf(linuxBuff, buffSize, "%s %s", uts.sysname, uts.release, uts.version);
    }

    snprintf(buffer, buffSize, "%s: %s", macBuff, linuxBuff);
}
