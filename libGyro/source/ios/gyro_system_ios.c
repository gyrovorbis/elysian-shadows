#include "gyro_system_api.h"
#include "ios_log.h"
#include "gyro_defines.h"
#include <stdarg.h>
#include <stdio.h>
//#include <sys/sysctl.h>  
//#include <mach/mach.h>

/* Needs to take initialization arguments. */
//int gySysInit() {
//    return 1;
//}
//
//int gySysUninit() {
//    return 1;
//}
//
//void gyPrintf(const char *const str, ...) {
//    va_list argptr;
//    static char buffer[GYRO_PRINTF_BUFFER_SIZE];
//
//    va_start(argptr, str);
//    vsprintf(buffer, str, argptr);
//    va_end(argptr);
//  //  IOS_LogWrites(buffer);
//   // NSLog(@"%s, buffer);
//}
//
//unsigned int gySysStatsRam() {
///*    static vm_statistics_data_t vm_stats;
//    static mach_msg_type_number_t info_count = HOST_VM_INFO_COUNT;
//    kern_return_t res = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_stats, &info_count);
//    if(res == KERN_SUCCESS)
//        return vm_stats.wire_count;*/
//    return 0;
//}
//
//unsigned int gySysStatsVram() {
//    return 0;
//}

