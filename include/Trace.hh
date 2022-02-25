/**
 * @file Trace.hh
 *
 * Author: Peter Helfer
 * Date: 2011-05-23
 */

#ifndef TRACE_HH
#define TRACE_HH

#include <string>
#include <unordered_set>
using std::string;
using std::unordered_set;

#include "fmt/format.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/**
 * Trace class
 */
class Trace
{
public:
    enum TraceLevel {
        TRACE_Flow,
        TRACE_Debug3,
        TRACE_Debug2,
        TRACE_Debug1,
        TRACE_Debug,
        TRACE_Info1,
        TRACE_Info,
        TRACE_Warn,
        TRACE_Error,
        TRACE_Fatal,

        TRACE_Maxval
    };

    static void setTraceLevel(TraceLevel level)
    {
        traceLevel = level;
    }

    static bool setTraceLevel(const char *levelString)
    {
        for (uint i = 0; i < TRACE_Maxval; i++) {
            if (strcasecmp(levelString, traceLevelString((TraceLevel) i)) == 0) {
                setTraceLevel((TraceLevel) i);
                return true;
            }
        }
        fmt::print(stderr, "Unknown trace level '{}'\n", levelString);
        return false;
    }

    static TraceLevel getTraceLevel()
    {
        return traceLevel;
    }

    static const char *getTraceLevelString()
    {
        return traceLevelString(traceLevel);
    }

    static void setTraceTag(const string &tag)
    {
        traceTags.insert(tag);
    }

    static bool isSet(const string &tag)
    {
        return traceTags.find(tag) != traceTags.end();
    }

#ifdef TRACE_ON
    #define TRACE(lvl, ...) \
        do { \
            if (lvl >= Trace::getTraceLevel()) { \
                Trace::trace(lvl, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); \
            } \
        } while(0);

#define TTRACE(tag, lvl, ...)               \
        do { \
            if (Trace::isSet(tag) && lvl >= Trace::getTraceLevel()) {   \
                Trace::trace(lvl, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); \
            } \
        } while(0);

    #define TRACE_FLOW_IS_ON    (Trace::TRACE_Flow   >= Trace::getTraceLevel())
    #define TRACE_DEBUG3_IS_ON  (Trace::TRACE_Debug3 >= Trace::getTraceLevel())
    #define TRACE_DEBUG2_IS_ON  (Trace::TRACE_Debug2 >= Trace::getTraceLevel())
    #define TRACE_DEBUG1_IS_ON  (Trace::TRACE_Debug1 >= Trace::getTraceLevel())
    #define TRACE_DEBUG_IS_ON   (Trace::TRACE_Debug  >= Trace::getTraceLevel())
    #define TRACE_INFO1_IS_ON   (Trace::TRACE_Info1  >= Trace::getTraceLevel())
    #define TRACE_INFO_IS_ON    (Trace::TRACE_Info   >= Trace::getTraceLevel())
    #define TRACE_WARN_IS_ON    (Trace::TRACE_Warn   >= Trace::getTraceLevel())
    #define TRACE_ERROR_IS_ON   (Trace::TRACE_Error  >= Trace::getTraceLevel())
#else
    #define TRACE(lvl, ...)
    #define TTRACE(tag, lvl, ...)
    #define TRACE_FLOW_IS_ON    false
    #define TRACE_DEBUG3_IS_ON  false
    #define TRACE_DEBUG2_IS_ON  false
    #define TRACE_DEBUG1_IS_ON  false
    #define TRACE_DEBUG_IS_ON   false
    #define TRACE_INFO1_IS_ON   false
    #define TRACE_INFO_IS_ON    false
    #define TRACE_WARN_IS_ON    false
    #define TRACE_ERROR_IS_ON   false
#endif

#define TRACE_FLOW(...)  TRACE(Trace::TRACE_Flow,    __VA_ARGS__)
#define TRACE_DEBUG3(...) TRACE(Trace::TRACE_Debug3, __VA_ARGS__)
#define TRACE_DEBUG2(...) TRACE(Trace::TRACE_Debug2, __VA_ARGS__)
#define TRACE_DEBUG1(...) TRACE(Trace::TRACE_Debug1, __VA_ARGS__)
#define TRACE_DEBUG(...) TRACE(Trace::TRACE_Debug,   __VA_ARGS__)
#define TRACE_INFO1(...) TRACE(Trace::TRACE_Info1,   __VA_ARGS__)
#define TRACE_INFO(...)  TRACE(Trace::TRACE_Info,    __VA_ARGS__)
#define TRACE_WARN(...)  TRACE(Trace::TRACE_Warn,    __VA_ARGS__)
#define TRACE_ERROR(...) TRACE(Trace::TRACE_Error,   __VA_ARGS__)

#define TTRACE_FLOW(tag, ...)   TTRACE(tag, Trace::TRACE_Flow,   __VA_ARGS__)
#define TTRACE_DEBUG3(tag, ...) TTRACE(tag, Trace::TRACE_Debug3, __VA_ARGS__)
#define TTRACE_DEBUG2(tag, ...) TTRACE(tag, Trace::TRACE_Debug2, __VA_ARGS__)
#define TTRACE_DEBUG1(tag, ...) TTRACE(tag, Trace::TRACE_Debug1, __VA_ARGS__)
#define TTRACE_DEBUG(tag, ...)  TTRACE(tag, Trace::TRACE_Debug,  __VA_ARGS__)
#define TTRACE_INFO1(tag, ...)  TTRACE(tag, Trace::TRACE_Info1,  __VA_ARGS__)
#define TTRACE_INFO(tag, ...)   TTRACE(tag, Trace::TRACE_Info,   __VA_ARGS__)
#define TTRACE_WARN(tag, ...)   TTRACE(tag, Trace::TRACE_Warn,   __VA_ARGS__)
#define TTRACE_ERROR(tag, ...)  TTRACE(tag, Trace::TRACE_Error,  __VA_ARGS__)

#define TRACE_FATAL(...)                                           \
    do {                                                                \
        Trace::trace(Trace::TRACE_Fatal, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); \
        abort(); \
    } while (0)
// TRACE_FATAL will always abort the process, even when compiled without TRACE_ON

#define TTRACE_FLOW_IS_ON(tag)   (Trace::isSet(tag) && TRACE_FLOW_IS_ON)
#define TTRACE_DEBUG3_IS_ON(tag) (Trace::isSet(tag) && TRACE_DEBUG3_IS_ON)
#define TTRACE_DEBUG2_IS_ON(tag) (Trace::isSet(tag) && TRACE_DEBUG2_IS_ON)
#define TTRACE_DEBUG1_IS_ON(tag) (Trace::isSet(tag) && TRACE_DEBUG1_IS_ON)
#define TTRACE_DEBUG_IS_ON(tag)  (Trace::isSet(tag) && TRACE_DEBUG_IS_ON)
#define TTRACE_INFO1_IS_ON(tag)  (Trace::isSet(tag) && TRACE_INFO1_IS_ON)
#define TTRACE_INFO_IS_ON(tag)   (Trace::isSet(tag) && TRACE_INFO_IS_ON)
#define TTRACE_WARN_IS_ON(tag)   (Trace::isSet(tag) && TRACE_WARN_IS_ON)
#define TTRACE_ERROR_IS_ON(tag)  (Trace::isSet(tag) && TRACE_ERROR_IS_ON)


/**
  * Abort if a condition is true. Unlike assert(3), ABORT_IF cannot be
  * disabled by a compile option like -DNDEBUG.
  * @param cond The condition
  * @param fmt fmtlib format string for error message
  * @param ... Optional parameters values for error message
  */
#define ABORT_IF(cond, ...) \
    if (cond) {                  \
        TRACE_DEBUG("Aborting because: {}", #cond);     \
        TRACE_FATAL( __VA_ARGS__);    \
    }

#define ABORT_UNLESS(cond, ...) \
    ABORT_IF(!(cond), __VA_ARGS__)

#define TRACE_ENTER()   do { TRACE_FLOW("-->"); Trace::incrIndent(); } while (0);
#define TRACE_EXIT()    do { Trace::decrIndent(); TRACE_FLOW("<--"); } while (0);
#define TRACE_RETURN(x) do { TRACE_EXIT(); return x; } while (0)
     
    template <typename... Args>
    static void trace(TraceLevel lvl,
                      const char *file,
                      int line,
                      const char *func,
                      const char *fmt,
                      Args  ... args)
    {
        FILE *dest = lvl >= TRACE_Warn ? stderr : stdout;
        fmt::print(dest, "{}{} {}[{}] {}(): ",
                indentStr(), traceLevelString(lvl), file, line, func);
        fmt::vprint(dest, fmt, fmt::make_format_args(args...));
        fmt::print(dest, "\n");
    }
    
    static void incrIndent()
    {
        if (indentLevel < MAXINDENT) indentLevel++;
    }

        static void decrIndent()
    {
        if (indentLevel > 0) indentLevel--;
    }
    
    static const char *traceLevelString(TraceLevel lvl)
    {
        switch (lvl) {
            case TRACE_Flow:   return "FLOW";
            case TRACE_Debug3: return "DEBUG3";
            case TRACE_Debug2: return "DEBUG2";
            case TRACE_Debug1: return "DEBUG1";
            case TRACE_Debug:  return "DEBUG";
            case TRACE_Info1:  return "INFO1";
            case TRACE_Info:   return "INFO";
            case TRACE_Warn:   return "WARN";
            case TRACE_Error:  return "ERROR";
            case TRACE_Fatal:  return "FATAL";
            default:           return "UNKNOWN TRACE LEVEL";
        }
    }

private:
    static TraceLevel traceLevel;
    static unordered_set<string> traceTags;
    static uint indentLevel;
    
    enum { MAXINDENT = 128 };
    
    static const char *indentStr()
    {
        if (traceLevel <= TRACE_Flow) {
            static char buf[MAXINDENT + 1];
            static bool firstTime = true;
            if (firstTime) {
                for (uint i = 0; i < MAXINDENT; i++) {
                    buf[i] = ' ';
                }
                buf[MAXINDENT] = 0;
                firstTime = false;
            }
            return buf + MAXINDENT - indentLevel;
        } else {
            return "";
        }
    }
};

#endif // TRACE_HH
