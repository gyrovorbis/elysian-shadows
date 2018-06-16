#include <gyro_system_api.h>
#include "gyro_defines.h"
//#include <x86/gyro_glfw_x86.inl>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_gl.h>
#include <stdio.h>
#include <stdarg.h>
#if GYRO_PLATFORM == GYRO_PLATFORM_RASPI
#include <SDL2/SDL.h>

#elif defined(__WIN32)
#include <sdl/SDL.h>

#elif defined(__linux__)
#include <SDL2/SDL.h>

#else
#include <SDL.h>
#endif

#include "../common/gyro_system_callbacks.h"



#if !(defined(__linux__) && defined(__i386__))



#  if defined(__linux__) && defined(__x86_64__)

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
#endif
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#include <imagehlp.h>
#include <process.h>
#include <dbghelp.h>
#endif

#if GYRO_PLATFORM == GYRO_PLATFORM_Qt
extern int gyQtInit(void);
#endif

static GYSysInitParams _sysParams;

void gyDebugSigInit(void);
void _gySysPrintBuildInfo(const char*const platString);


static GYSystemInfo _systemInfo;

const GYSystemInfo* gySystemQueryInfo(void) {
    return &_systemInfo;
}

int _sdlQuerySystemInfo(GYSysInitParams* params) {
    int success = 1;
    memset(&_systemInfo, 0, sizeof(GYSystemInfo));

    _gyLog(GY_DEBUG_VERBOSE, "[GY_SYSTEM_API]: Querying System Information");
    _gyPush();

    strncpy(_systemInfo.platformName, SDL_GetPlatform(), GYRO_SYSTEM_PLATFORM_NAME_SIZE);


    _gyPop(1);
    return success;
}

int gySysInit(const GYSysInitParams*const params) {
    //char strBuff[40];
    //gyDebugInit();

  const char* platString =
#if GYRO_PLATFORM == GYRO_PLATFORM_RASPI
  "RaspberryPi";
#elif defined(__APPLE__)
  "MacOS";
#elif defined(_WIN32)
  "Windows";
#else
  "Linux";
#endif

    _gySysPrintBuildInfo(platString);
#if defined(__linux__) && defined(__x86_64__)
    _checkCPUCapabilities();
#endif

#if GYRO_PLATFORM == GYRO_PLATFORM_x86
    glfwInit();
#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt
    gyQtInit();
#endif

    memcpy(&_sysParams, params, sizeof(GYSysInitParams));

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



#if defined(_WIN32)

extern void backtrace_register();

#define TRACE_MAX_FUNCTION_NAME_LENGTH  255

void windows_print_stacktrace(EXCEPTION_POINTERS * ExceptionInfo) {

    // Initialise the symbol table to get function names:
       HANDLE thread = GetCurrentThread();
       HANDLE process = GetCurrentProcess();

       CONTEXT context = *(ExceptionInfo->ContextRecord);
       //memset(&context, 0, sizeof(CONTEXT));
       //context.ContextFlags = CONTEXT_FULL;
       //RtlCaptureContext(&context);



    // Store the current stack frame here:
    STACKFRAME frame = { 0 };
     DWORD image;
     ZeroMemory(&frame, sizeof(STACKFRAME));
 #ifdef _M_IX86
     image = IMAGE_FILE_MACHINE_I386;
     frame.AddrPC.Offset = context.Eip;
     frame.AddrPC.Mode = AddrModeFlat;
     frame.AddrFrame.Offset = context.Ebp;
     frame.AddrFrame.Mode = AddrModeFlat;
     frame.AddrStack.Offset = context.Esp;
     frame.AddrStack.Mode = AddrModeFlat;
 #elif _M_X64
     image = IMAGE_FILE_MACHINE_AMD64;
     frame.AddrPC.Offset = context->Rip;
     frame.AddrPC.Mode = AddrModeFlat;
     frame.AddrFrame.Offset = context->Rbp;
     frame.AddrFrame.Mode = AddrModeFlat;
     frame.AddrStack.Offset = context->Rsp;
     frame.AddrStack.Mode = AddrModeFlat;
 #elif _M_IA64
     image = IMAGE_FILE_MACHINE_IA64;
     frame.AddrPC.Offset = context->StIIP;
     frame.AddrPC.Mode = AddrModeFlat;
     frame.AddrFrame.Offset = context->IntSp;
     frame.AddrFrame.Mode = AddrModeFlat;
     frame.AddrBStore.Offset = context->RsBSP;
     frame.AddrBStore.Mode = AddrModeFlat;
     frame.AddrStack.Offset = context->IntSp;
     frame.AddrStack.Mode = AddrModeFlat;
 #else
 #error "This platform is not supported."
 #endif
#if 0
     for (size_t i = 0; i < 25; i++) {

       BOOL result = StackWalk64(
         image, process, thread,
         &frame, &context, NULL,
         SymFunctionTableAccess64, SymGetModuleBase64, NULL);

        _gyLog(GY_DEBUG_VERBOSE, "STILL ALIEVE");

        DWORD error = GetLastError();
        if (error && TRUE)
        {
            _gyLog(GY_DEBUG_VERBOSE, "Error from StackWalk64: %lu.\n", error);
        }

       if (!result) { break;  }

       char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
       PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
       symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
       symbol->MaxNameLen = MAX_SYM_NAME;

       DWORD64 displacement = 0;
       if (SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol)) {
         _gyLog(GY_DEBUG_VERBOSE, "[%i] %s\n", i, symbol->Name);
       } else {
         _gyLog(GY_DEBUG_VERBOSE, "[%i] ???\n", i);
       }

     }
#elif 1

     SYMBOL_INFO *symbol = (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO)+(TRACE_MAX_FUNCTION_NAME_LENGTH ) * sizeof(TCHAR));
        symbol->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
        line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD displacement;
        while (StackWalk(image, process, NULL, &frame, &context, NULL, NULL, NULL, NULL))
        {
            if (SymFromAddr(process, frame.AddrPC.Offset, NULL, symbol))
            {
                if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &displacement, line))
                {
                    _gyLog(GY_DEBUG_VERBOSE, "\tat %s in %s: line: %lu: address: 0x%0X\n", symbol->Name, line->FileName, line->LineNumber, symbol->Address);
                }
                else if (TRUE)
                {
                    _gyLog(GY_DEBUG_VERBOSE, "Error from SymGetLineFromAddr64: %lu.\n", GetLastError());
                }
            }
            else if (TRUE)
            {
                _gyLog(GY_DEBUG_VERBOSE, "Error from SymFromAddr: %lu.\n", GetLastError());
            }
        }
        DWORD error = GetLastError();
        if (error && TRUE)
        {
            _gyLog(GY_DEBUG_VERBOSE, "Error from StackWalk64: %lu.\n", error);
        }
        _gyLog(GY_DEBUG_VERBOSE, "DUN WALKIN DA STACK");

        /*
#define EXCEPTION_FILE L"Exception.dmp"
        HANDLE dumpFile = CreateFile(EXCEPTION_FILE, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        MINIDUMP_EXCEPTION_INFORMATION exceptionInformation;
        exceptionInformation.ThreadId = GetCurrentThreadId();
        exceptionInformation.ExceptionPointers = ExceptionInfo;
        exceptionInformation.ClientPointers = FALSE;
        if (MiniDumpWriteDump(process, GetProcessId(process), dumpFile, MiniDumpNormal, ExceptionInfo ? &exceptionInformation : NULL, NULL, NULL))
        {
            printf("Wrote a dump.");
        }
*/
#else
     typedef USHORT (WINAPI CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID, __out_opt PULONG);
       CaptureStackBackTraceType func = (CaptureStackBackTraceType)(GetProcAddress(LoadLibrary("kernel32.dll"), "RtlCaptureStackBackTrace"));

       if(func == NULL)
           return; // WOE 29.SEP.2010

       // Quote from Microsoft Documentation:
       // ## Windows Server 2003 and Windows XP:
       // ## The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
       const int kMaxCallers = 62;

       void* callers[kMaxCallers];
       int count = (func)(0, kMaxCallers, callers, NULL);
       for(int i = 0; i < count; i++)
          printf(TraceFile, "*** %d called from %016I64LX\n", i, callers[i]);

#endif



     SymCleanup(process);
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
      windows_print_stacktrace(ExceptionInfo);
  }

  return EXCEPTION_EXECUTE_HANDLER;
}
#endif



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
    sigset_t x;
    sigfillset(&x);
    //DO NOT FUCKING FUCK WITH ME WHILE I'M HANDLING A SIGNAL!!!111
    sigprocmask(SIG_BLOCK, &x, NULL);

    (void) sig;
    (void) unused;
    int i, num;
    void* buf[64];
    char** symbols;

    num = backtrace(buf, 64);
    symbols = backtrace_symbols(buf, num);

    _gyLog(GY_DEBUG_VERBOSE, "**** BEGIN C/++ BACKTRACE ****");
    _gyPush();
    _gyLog(GY_DEBUG_VERBOSE, "RECIEVED SIGNAL: %s!",
           gyDebugSigCodeToStr(info->si_signo, info->si_code));
    for(i = 0; i < num; i++)
        _gyLog(GY_DEBUG_VERBOSE, "	%s", symbols[i]);
    _gyLog(GY_DEBUG_VERBOSE, "**** END C/++ BACKTRACE ****");
    _gyPop(1);

    if(_sysParams.segFaultHandler) {
        _sysParams.segFaultHandler(_sysParams.context);
        _exit(2);
    } else {
        _gyLog(GY_DEBUG_VERBOSE, "Exitting with default segfault handler.");
        //exit(-1);
    }
}


static void _regSigAction(const int sig, const char* name, struct sigaction*const sa) {
    struct sigaction so;
    sigaction(sig, sa, &so);
     if(so.sa_handler == SIG_IGN)
         _gyLog(GY_DEBUG_WARNING, "Unable to set up %s signal handler!", name);
}


#define _GY_SIG(a) a, GYRO_STRINGIFY_MACRO(#a)

#endif

void gyDebugSigInit(void) {
#if defined(__LINUX__) || defined(__APPLE__)
    _gyLog(GY_DEBUG_VERBOSE, "Registering exception handler to POSIX signals.");
    //return;
    struct sigaction sa;

    /* Set up handler. */
    sa.sa_handler		= NULL;
    sa.sa_sigaction		= gyDebugSigHandler;

    sigfillset(&sa.sa_mask);
    sa.sa_flags			= SA_SIGINFO;

    /* Register signal handlers */
    _regSigAction(_GY_SIG(SIGSEGV), &sa);
    _regSigAction(_GY_SIG(SIGFPE), &sa);
    _regSigAction(_GY_SIG(SIGTRAP), &sa);
    _regSigAction(_GY_SIG(SIGBUS), &sa);
    _regSigAction(_GY_SIG(SIGHUP), &sa);
    _regSigAction(_GY_SIG(SIGQUIT), &sa);
    _regSigAction(_GY_SIG(SIGILL), &sa);
    _regSigAction(_GY_SIG(SIGABRT), &sa);
    //_regSigAction(_GY_SIG(SIGKILL), &sa);
    _regSigAction(_GY_SIG(SIGTERM), &sa);

#elif defined(_WIN32)
    //LoadLibraryA("backtrace.dll");
    SetUnhandledExceptionFilter(&windows_exception_handler);
   // SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    //_gyLog(GY_DEBUG_VERBOSE, "RETVAL - %d", retVal);
    //backtrace_register();
#endif
}

#if 0
#if GYRO_PLATFORM != GYRO_PLATFORM_Qt
int main(int argc, char *argv[])
{
    return gyro_entry_point(argc, argv);
}
#endif
#endif
