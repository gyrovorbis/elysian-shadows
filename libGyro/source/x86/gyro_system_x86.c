#include "gyro_system.h"
#include "gyro_defines.h"
#include "x86/gyro_sdl_x86.h"
#include "glal/gyro_video_glal.h"
#include "stdio.h"
#include "stdarg.h"
#ifdef __LINUX__
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#endif

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


#ifdef __LINUX__
static const char* gyDebugSigCodeToStr(int sig, int sig_code) {
	if(sig == SIGFPE)
		switch(sig_code) {
			case SI_USER:		return "SIGFPE (raised by program)";
			case FPE_INTDIV:	return "SIGFPE (integer divide by zero)";
			case FPE_INTOVF:	return "SIGFPE (integer overflow)";
			case FPE_FLTDIV:	return "SIGFPE (floating-point divide by zero)";
			case FPE_FLTOVF:	return "SIGFPE (floating-point overflow)";
			case FPE_FLTUND:	return "SIGFPE (floating-point underflow)";
			case FPE_FLTRES:	return "SIGFPE (floating-point inexact result)";
			case FPE_FLTINV:	return "SIGFPE (floating-point invalid operation)";
			case FPE_FLTSUB:	return "SIGFPE (subscript out of range)";
			default:			return "SIGFPE";
		}
	else if(sig == SIGSEGV)
		switch(sig_code) {
			case SI_USER:		return "SIGSEGV (rasied by program)";
			case SEGV_MAPERR:	return "SIGSEGV (address not mapped to object)";
			case SEGV_ACCERR:	return "SIGSEGV (invalid permissions for mapped object)";
			default:			return "SIGSEGV";
		}
	else if(sig == SIGTRAP)
		switch(sig_code) {
			case SI_USER:		return "SIGTRAP (raised by program)";
			case TRAP_BRKPT:	return "SIGTRAP (process breakpoint)";
			case TRAP_TRACE:	return "SIGTRAP	(process trace trap)";
			default:			return "SIGTRAP";
		}
	/* No suitable code found. */
	return strsignal(sig);
}

static void gyDebugSigHandler(int sig, siginfo_t* info, void* unused) {
	(void) sig;
	(void) unused;
	int i, num;
	void* buf[64];
	char** symbols;

	num = backtrace(buf, 64);
	symbols = backtrace_symbols(buf, num);

	_gyLog(GY_DEBUG_FATAL, "RECIEVED SIGNAL: %s!",
		   gyDebugSigCodeToStr(info->si_signo, info->si_code));
	for(i = 0; i < num; i++)
		_gyLog(GY_DEBUG_FATAL, "	%s", symbols[i]);
	_gyLog(GY_DEBUG_FATAL, "Report this to one of the code monkeys with the backtrace.");

	exit(1);
}
#endif

void gyDebugSigInit(void) {
#ifdef __LINUX__
	struct sigaction sa, so;

	/* Set up handler. */
	sa.sa_handler		= NULL;
	sa.sa_sigaction		= gyDebugSigHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags			= SA_SIGINFO;

	/* Attach signals. */
	sigaction(SIGSEGV, &sa, &so);
	if(so.sa_handler == SIG_IGN)
		_gyLog(GY_DEBUG_VERBOSE, "Unable to set up SIGSEGV signal handler.");
	sigaction(SIGFPE, &sa, &so);
	if(so.sa_handler == SIG_IGN)
		_gyLog(GY_DEBUG_VERBOSE, "Unable to set up SIGFPE signal handler.");
	sigaction(SIGTRAP, &sa, &so);
	if(so.sa_handler == SIG_IGN)
		_gyLog(GY_DEBUG_VERBOSE, "Unable to set up SIGTRAP signal handler.");
#endif
}
