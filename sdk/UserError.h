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

inline Verbosity current_verbosity_level = Verbosity::All;

inline void setVerbosityLevel(Verbosity v) { current_verbosity_level = v; }


// function-local static: initialised on first use, safe across translation units
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