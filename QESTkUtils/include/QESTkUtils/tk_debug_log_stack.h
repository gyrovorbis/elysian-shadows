#ifndef TK_DEBUG_LOG_STACK_H
#define TK_DEBUG_LOG_STACK_H

#define Q_CSTR(a) (a.toStdString().c_str())

namespace QESTkUtils {

    class Debug {
    public:
        enum LOG_SEVERITY {
            LOG_VERBOSE,
            LOG_WARNING,
            LOG_ERROR
        };

        struct LogAdapter {
            virtual         ~LogAdapter(void) = default;
            virtual void    log(LOG_SEVERITY severity, const char* fmt, ...)   = 0;
            virtual void    push(void)                                         = 0;
            virtual void    pop(unsigned depth=1)                              = 0;
        };

        static void         setLogAdapter(LogAdapter* iface);

        template<typename... Args>
        static void         log(LOG_SEVERITY severity, const char* fmt, Args... args);
        static void         push(void);
        static void         pop(unsigned depth=1);

    private:
        static LogAdapter*  _logAdapter;


    };





    //=====INLINEZ=====
    inline void Debug::setLogAdapter(LogAdapter* iface) { _logAdapter = iface; }
    inline void Debug::push(void) { if(_logAdapter) _logAdapter->push(); }
    inline void Debug::pop(unsigned depth) { if(_logAdapter) _logAdapter->pop(depth); }
    template<typename... Args>
    inline void Debug::log(LOG_SEVERITY severity, const char* fmt, Args... args) {
        if(_logAdapter) {
            _logAdapter->log(severity, fmt, args...);
        }
    }


}

#endif // TK_DEBUG_STACK_H
