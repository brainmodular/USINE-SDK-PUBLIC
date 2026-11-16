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

// Every module has its own error registry and its own verbosity, never shared with the other modules.
// Everything below is USINE_MODULE_LOCAL: an inline variable or function is a weak definition, and on
// macOS and Linux the dynamic linker unifies the weak definitions of the whole process (Windows never
// unifies across DLLs). Without it, the modules loaded together would share one registry and one
// verbosity, or call the RegisterError and sdkRaiseError of another module, which use its registry.
// The error constants of a module (inline const int ERROR_X = RegisterError(...)) must be
// USINE_MODULE_LOCAL too, so that each one holds an index in the registry of its own module.
inline Verbosity current_verbosity_level USINE_MODULE_LOCAL = Verbosity::All;

USINE_MODULE_LOCAL inline void setVerbosityLevel(Verbosity v) { current_verbosity_level = v; }


// the registry of the module, a function-local static: initialised on first use, safe across
// translation units
USINE_MODULE_LOCAL inline std::vector<Error>& errorList()
{
    static std::vector<Error> list;
    return list;
}

USINE_MODULE_LOCAL inline int RegisterError(Level level, const char* message)
{
    errorList().push_back(Error{level, message});
    return int(errorList().size()) -1;
}


USINE_MODULE_LOCAL inline void traceType(const Error& err)
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

USINE_MODULE_LOCAL inline void sdkRaiseError(int index, bool force_log = false)
{
    if (index < 0 || index >= (int)errorList().size())
        return;

    const Error& err = errorList()[index];

    if ((int)err.level <= (int)current_verbosity_level)
        traceType(err);

    if (force_log && err.level != Level::Error)
        sdkTraceLogChar(err.msg, FALSE);
}

USINE_MODULE_LOCAL inline void sdkRaiseError(int index, const char *extraMsg, bool force_log = false)
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

USINE_MODULE_LOCAL inline void sdkRaiseError(int index, const std::string &extraMsg, bool force_log = false)
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