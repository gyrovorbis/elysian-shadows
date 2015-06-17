#include <gyro_system_api.h>
#include "gyro_defines.h"
//#include <x86/gyro_glfw_x86.inl>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_gl.h>
#include <stdio.h>
#include <stdarg.h>

#include "../common/gyro_system_callbacks.h"

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
        _gyLog(GY_DEBUG_ERROR, "SSE CPU Support Not Detected! Prepare for crash... and maybe consider a processor from this decade.");
        //exit(0);
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "SSE CPU Extensions detected! Fast-ass math enabled!");
    }
}


#if defined(__LINUX__) || defined(__APPLE__)
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#include <imagehlp.h>
#endif

static GYSysParams _sysParams;

void gyDebugSigInit(void);

int gySysInit(const GYSysParams*const params) {
    char strBuff[40];
    //gyDebugInit();

  const char* platString =
#ifdef __APPLE__
  "OSX";
#elif defined(_WIN32)
  "WINDOWS";
#else
  "LINUX";
#endif
    _gyLog(GY_DEBUG_VERBOSE, "Initializing libGyro_x86_64 [%s] v%s", platString, GYRO_LIB_VERSION);
    _gyPush();
#ifdef GYRO_LOCAL_BUILD
    _gyLog(GY_DEBUG_WARNING, "THIS IS AN UNOFFICIAL BUILD!");
#else
    _gyLog(GY_DEBUG_VERBOSE, "THIS IS AN OFFICIAL BUILD!");
#endif
    _gyPush();
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Git branch name", GYRO_STRINGIFY_MACRO(GYRO_BUILD_BRANCH));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Git commit ID", GYRO_STRINGIFY_MACRO(GYRO_BUILD_COMMIT));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Build number", GYRO_STRINGIFY_MACRO(GYRO_BUILD_NUMBER));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Build node", GYRO_STRINGIFY_MACRO(GYRO_BUILD_NODE));
    sprintf(strBuff, "%s %s", __DATE__, __TIME__);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Build timestamp", strBuff);
    _gyPop(1);

    gyDebugSigInit();
    _checkCPUCapabilities();
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
    glfwInit();
#endif

    memcpy(&_sysParams, params, sizeof(GYSysParams));

    _gyPop(1);

    return 1;
}

int gySysUninit(void) {
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
    glfwTerminate();
#endif

    //gyDebugUninit();
    return 1;
}

#if 0
void gyPrintf(const char *const str, ...) {
    va_list argptr;
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);

    fprintf(stdout, "%s", buffer);
    fflush(stdout);
}
#endif

unsigned int gySysStatsVram(void) {
    return 0;
}

unsigned int gySysStatsRam(void) {
    return 0;
}


#if defined(__LINUX__) || defined(__APPLE__)
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
    char backTrace[3000];
    (void) sig;
    (void) unused;
    int i, num;
    void* buf[64];
    char** symbols;

    num = backtrace(buf, 64);
    symbols = backtrace_symbols(buf, num);

    _gyLog(GY_DEBUG_ERROR, "**** BEGIN C/++ BACKTRACE ****");
    _gyPush();
    _gyLog(GY_DEBUG_ERROR, "RECIEVED SIGNAL: %s!",
           gyDebugSigCodeToStr(info->si_signo, info->si_code));
    for(i = 0; i < num; i++)
        _gyLog(GY_DEBUG_ERROR, "	%s", symbols[i]);
    _gyLog(GY_DEBUG_ERROR, "**** END C/++ BACKTRACE ****");
    _gyPop(1);

    if(_sysParams.segFaultHandler) {
        _sysParams.segFaultHandler(_sysParams.context);
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "Exitting with default segfault handler.");
        exit(1);
    }
}
#elif defined(_WIN32)

const char* execPipe(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "PIPE DBG_ERROR!";
    static char buffer[1280];
    char tempBuff[129];
    buffer[0] = 0;
    while(!feof(pipe)) {
        if(fgets(tempBuff, 128, pipe) != NULL)
            strcat(buffer, tempBuff);
    }
    pclose(pipe);
    return buffer;
}

/* Resolve symbol name and source location given the path to the executable
   and an address */
int addr2line(char const * const program_name, void const * const addr)
{
  char addr2line_cmd[512] = {0};

  /* have addr2line map the address to the relent line in the code */
   sprintf(addr2line_cmd,"addr2line 2>&1 -f -p -e %.256s %p", program_name, addr);

  /* This will print a nicely formatted string specifying the
     function and source line of the address */

   _gyLog(GY_DEBUG_VERBOSE, execPipe(addr2line_cmd));
   return 1;
  //return system(addr2line_cmd);
}

void windows_print_stacktrace(CONTEXT* context)
{

  SymInitialize(GetCurrentProcess(), 0, 1);

  STACKFRAME frame = { 0 };

  /* setup initial stack frame */
  frame.AddrPC.Offset         = context->Eip;
  frame.AddrPC.Mode           = AddrModeFlat;
  frame.AddrStack.Offset      = context->Esp;
  frame.AddrStack.Mode        = AddrModeFlat;
  frame.AddrFrame.Offset      = context->Ebp;
  frame.AddrFrame.Mode        = AddrModeFlat;

  while (StackWalk(IMAGE_FILE_MACHINE_I386 ,
                   GetCurrentProcess(),
                   GetCurrentThread(),
                   &frame,
                   context,
                   0,
                   SymFunctionTableAccess,
                   SymGetModuleBase,
                   0 ) )
  {
    addr2line(__argv[0], (void*)frame.AddrPC.Offset);
  }

  SymCleanup( GetCurrentProcess() );

}

static LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS * ExceptionInfo)
{
    (void)ExceptionInfo;

    _gyLog(GY_DEBUG_VERBOSE, "===================================");
  switch(ExceptionInfo->ExceptionRecord->ExceptionCode)
  {
    case EXCEPTION_ACCESS_VIOLATION:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_ACCESS_VIOLATION", stderr);
      break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_ARRAY_BOUNDS_EXCEEDED", stderr);
      break;
    case EXCEPTION_BREAKPOINT:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_BREAKPOINT", stderr);
      break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_DATATYPE_MISALIGNMENT", stderr);
      break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_FLT_DENORMAL_OPERAND", stderr);
      break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_FLT_DIVIDE_BY_ZERO", stderr);
      break;
    case EXCEPTION_FLT_INEXACT_RESULT:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_FLT_INEXACT_RESULT", stderr);
      break;
    case EXCEPTION_FLT_INVALID_OPERATION:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_FLT_INVALID_OPERATION", stderr);
      break;
    case EXCEPTION_FLT_OVERFLOW:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_FLT_OVERFLOW", stderr);
      break;
    case EXCEPTION_FLT_STACK_CHECK:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_FLT_STACK_CHECK", stderr);
      break;
    case EXCEPTION_FLT_UNDERFLOW:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_FLT_UNDERFLOW", stderr);
      break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_ILLEGAL_INSTRUCTION", stderr);
      break;
    case EXCEPTION_IN_PAGE_ERROR:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_IN_PAGE_ERROR", stderr);
      break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_INT_DIVIDE_BY_ZERO", stderr);
      break;
    case EXCEPTION_INT_OVERFLOW:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_INT_OVERFLOW", stderr);
      break;
    case EXCEPTION_INVALID_DISPOSITION:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_INVALID_DISPOSITION", stderr);
      break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_NONCONTINUABLE_EXCEPTION", stderr);
      break;
    case EXCEPTION_PRIV_INSTRUCTION:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_PRIV_INSTRUCTION", stderr);
      break;
    case EXCEPTION_SINGLE_STEP:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_SINGLE_STEP", stderr);
      break;
    case EXCEPTION_STACK_OVERFLOW:
      _gyLog(GY_DEBUG_VERBOSE, "Error: EXCEPTION_STACK_OVERFLOW", stderr);
      break;
    default:
      _gyLog(GY_DEBUG_VERBOSE, "Error: Unrecognized Exception", stderr);
      break;
  }
  //fflush(stderr);

  _gyLog(GY_DEBUG_VERBOSE, "===================================");

  if (EXCEPTION_STACK_OVERFLOW != ExceptionInfo->ExceptionRecord->ExceptionCode)
  {
      windows_print_stacktrace(ExceptionInfo->ContextRecord);
  }

  return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void gyDebugSigInit(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Registering exception handler to POSIX signals.");
#if defined(__LINUX__) || defined(__APPLE__)
    struct sigaction sa, so;

    /* Set up handler. */
    sa.sa_handler		= NULL;
    sa.sa_sigaction		= gyDebugSigHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags			= SA_SIGINFO;

    /* Attach Q_SIGNALS. */
    sigaction(SIGSEGV, &sa, &so);
    if(so.sa_handler == SIG_IGN)
        _gyLog(GY_DEBUG_VERBOSE, "Unable to set up SIGSEGV signal handler.");
    sigaction(SIGFPE, &sa, &so);
    if(so.sa_handler == SIG_IGN)
        _gyLog(GY_DEBUG_VERBOSE, "Unable to set up SIGFPE signal handler.");
    sigaction(SIGTRAP, &sa, &so);
    if(so.sa_handler == SIG_IGN)
        _gyLog(GY_DEBUG_VERBOSE, "Unable to set up SIGTRAP signal handler.");
#elif defined(_WIN32)
    //SetUnhandledExceptionFilter(&windows_exception_handler);

#endif
}



#if GYRO_PLATFORM != GYRO_PLATFORM_Qt
int main(int argc, char *argv[])
{
    return gyro_entry_point(argc, argv);
}
#endif
