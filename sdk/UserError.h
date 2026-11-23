#pragma once
# include <string>
# include <vector>
# include "UsineFunctions.h"


enum class UReportLevel     { Error = 1, Warn, Info };
enum class UReportVerbosity { None, Error, Warn, All };

struct UReport {
    UReportLevel       level;
    std::string        msg;
};

// Every module binary has its own report registry, never shared with the other modules.
// Everything below is USINE_MODULE_LOCAL: an inline variable or function is a weak definition, and on
// macOS and Linux the dynamic linker unifies the weak definitions of the whole process (Windows never
// unifies across DLLs). Without it, the modules loaded together would share one registry, or call the
// registerReport of another module, which uses its registry.
// The report constants of a module (inline const int ERROR_X = registerReport(...)) must be
// USINE_MODULE_LOCAL too, so that each one holds an index in the registry of its own module.
// The registry is shared by every instance of the module. The verbosity is not: each instance
// has its own "verbosity level" combo box, read by UserModuleBase::sdkReport.

// the registry of the module, a function-local static: initialised on first use, safe across
// translation units
USINE_MODULE_LOCAL inline std::vector<UReport>& uReportList()
{
    static std::vector<UReport> list;
    return list;
}

USINE_MODULE_LOCAL inline int registerReport(UReportLevel level, const char* message)
{
    uReportList().push_back(UReport{level, message});
    return int(uReportList().size()) -1;
}

// Traces a registered report if its level passes the given verbosity. An invalid index is ignored.
// forceLog also writes a warning or an info in the Usine log file, even when the verbosity hides it.
// Modules call UserModuleBase::sdkReport, which filters with the verbosity of the instance. Call this one
// only from code that has no module instance (a global library callback for example), with a verbosity
// of your own.
USINE_MODULE_LOCAL inline void reportWithVerbosity(UReportVerbosity verbosity, int index,
                                                   const std::string& extraMsg = {}, bool forceLog = false)
{
    if (index < 0 || index >= static_cast<int>(uReportList().size()))
        return;

    const UReport& registered = uReportList()[index];
    const std::string msg = registered.msg + extraMsg;

    if (static_cast<int>(registered.level) <= static_cast<int>(verbosity)) {
        switch (registered.level) {
            case UReportLevel::Error:
                sdkTraceErrorChar(msg);
                break;
            case UReportLevel::Warn:
                sdkTraceWarningChar(msg);
                break;
            case UReportLevel::Info:
                sdkTraceChar(msg);
                break;
        }
    }
    // forceLog has no effect on errors
    if (forceLog && registered.level != UReportLevel::Error)
        sdkTraceLogChar(msg, FALSE);
}
