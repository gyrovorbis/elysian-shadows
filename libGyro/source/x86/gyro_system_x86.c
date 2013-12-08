#include "gyro_system.h"
#include "gyro_defines.h"
#include "x86/gyro_sdl_x86.h"
#include "glal/gyro_video_glal.h"
#include "stdio.h"
#include "stdarg.h"

//execute x86 CPUID opcode
static inline void cpuid(int CPUInfo[4],int InfoType){
	__asm__ __volatile__ (
		"cpuid":
		"=a" (CPUInfo[0]),
		"=b" (CPUInfo[1]),
		"=c" (CPUInfo[2]),
		"=d" (CPUInfo[3]) :
		"a" (InfoType)
	);
}

static void _checkCPUCapabilities(void) {
	int x64     = 0;
	int MMX     = 0;
	int SSE     = 0;
	int SSE2    = 0;
	int SSE3    = 0;
	int SSSE3   = 0;
	int SSE41   = 0;
	int SSE42   = 0;
	int SSE4a   = 0;
	int AVX     = 0;
	int XOP     = 0;
	int FMA3    = 0;
	int FMA4    = 0;

	int info[4];
	cpuid(info, 0);
	int nIds = info[0];

	cpuid(info, 0x80000000);
	unsigned nExIds = info[0];

	//  Detect Instruction Set
	if (nIds >= 1){
		cpuid(info,0x00000001);
		MMX   = (info[3] & ((int)1 << 23)) != 0;
		SSE   = (info[3] & ((int)1 << 25)) != 0;
		SSE2  = (info[3] & ((int)1 << 26)) != 0;
		SSE3  = (info[2] & ((int)1 <<  0)) != 0;

		SSSE3 = (info[2] & ((int)1 <<  9)) != 0;
		SSE41 = (info[2] & ((int)1 << 19)) != 0;
		SSE42 = (info[2] & ((int)1 << 20)) != 0;

		AVX   = (info[2] & ((int)1 << 28)) != 0;
		FMA3  = (info[2] & ((int)1 << 12)) != 0;
	}

	if (nExIds >= 0x80000001){
		cpuid(info,0x80000001);
		x64   = (info[3] & ((int)1 << 29)) != 0;
		SSE4a = (info[2] & ((int)1 <<  6)) != 0;
		FMA4  = (info[2] & ((int)1 << 16)) != 0;
		XOP   = (info[2] & ((int)1 << 11)) != 0;
	}

	if(!SSE) {
		_gyLog(GY_DEBUG_FATAL, "\tSSE CPU Support Not Detected! Shit ain't gonna work!");
		//exit(0);
	} else {
		_gyLog(GY_DEBUG_VERBOSE, "\tSSE CPU Extensions detected! Fast-ass math enabled!");
	}


}

int gySysInit(void) {
  const char* platString =
#ifdef __APPLE__
  "OSX";
#elif defined(WIN32)
  "WINDOWS";
#else
  "LINUX";
#endif

	_gyLog(GY_DEBUG_VERBOSE, "Initializing libGyro_x86 [%s] v%s", platString, GYRO_LIB_VERSION);


	_checkCPUCapabilities();

    return 1;
}

int gySysUninit(void) {
    return 1;
}

void gyPrintf(const char *const str, ...) {
    va_list argptr;
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);

    fprintf(stdout, "%s", buffer);
	fflush(stdout);
}

unsigned int gySysStatsVram(void) {
    return 0;
}

unsigned int gySysStatsRam(void) {
    return 0;
}

