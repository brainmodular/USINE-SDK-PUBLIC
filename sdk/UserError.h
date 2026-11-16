#pragma once
# include <string>
# include <vector>
# include "UsineFunctions.h"


enum class Level     { Error, Warn, Info };
enum class Verbosity { None = -1, Error, Warn, Info, All = Info };

struct Error {
    Level       level;
    std::string msg;
};

// hidden: an inline variable is a weak definition, and the dynamic linker unifies the weak
// definitions of the whole process, so the trace level set in one module would filter the traces of
// every other module (Windows never unifies across DLLs)
inline Verbosity current_verbosity_level USINE_MODULE_LOCAL = Verbosity::All;

inline void setVerbosityLevel(Verbosity v) { current_verbosity_level = v; }


// function-local static: initialised on first use, safe across translation units.
// NOT hidden, unlike the verbosity: an `inline const int ERROR_X = RegisterError(...)` has a dynamic
// initializer, so it is a weak definition the dynamic linker unifies between two modules that use the
// same constant name; the index it holds must therefore stay valid in ONE registry shared by all the
// modules of the process. A per-module registry would silently drop those errors.
inline std::vector<Error>& errorList()
{
    static std::vector<Error> list;
    return list;
}

inline int RegisterError(Level level, const char* message)
{
    errorList().push_back(Error{level, message});
    return int(errorList().size()) -1;
}


inline void    traceType(const Error& err)
{
    switch (err.level)
    {
        case Level::Error:
            sdkTraceErrorChar(err.msg); break;
        case Level::Warn:
            sdkTraceWarningChar(err.msg); break;
        case Level::Info:
            sdkTraceChar(err.msg); break;
    }
}

inline void sdkRaiseError(int index, bool force_log = false)
{
    if (index < 0 || index >= (int)errorList().size())
        return;

    const Error& err = errorList()[index];

    if ((int)err.level <= (int)current_verbosity_level)
        traceType(err);

    if (force_log && err.level != Level::Error)
        sdkTraceLogChar(err.msg, FALSE);
}

inline void sdkRaiseError(int index, const char *extraMsg, bool force_log = false)
{
    if (index < 0 || index >= (int)errorList().size())
        return;

    Error err = errorList()[index];
    if (extraMsg != nullptr)  
        err.msg = err.msg + extraMsg;    

    if ((int)err.level <= (int)current_verbosity_level)
        traceType(err);

    if (force_log && err.level != Level::Error)
        sdkTraceLogChar(err.msg, FALSE);
}

inline void sdkRaiseError(int index, const std::string &extraMsg, bool force_log = false)
{
    if (index < 0 || index >= (int)errorList().size())
        return;

    Error err = errorList()[index];
    err.msg = err.msg + extraMsg; 

    if ((int)err.level <= (int)current_verbosity_level)
        traceType(err);

    if (force_log && err.level != Level::Error)
        sdkTraceLogChar(err.msg, FALSE);
}