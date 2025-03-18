#pragma once

#include "../../config.h"
#include "../types/types.h"
#include "logger_internal.h"
#include <utility>
#include <string>
#include <stdarg.h>


namespace Borealis::Debug
{
    enum class LogLevel
    {
        INFO,
        WARNING,
        ERROR,
        ASSERTION,
    };

    struct DebugInfoDesc // 24 byte
    {
        DebugInfoDesc(const std::string msg, const std::string file, const int line, const LogLevel type)
            : line(line), debugType(type), fileName(file)
        {
            this->msg = "[";

            switch (type)
            {
                case LogLevel::WARNING:
                {
                    this->msg += "WARNING";
                    break;
                }
                case LogLevel::ERROR:
                {
                    this->msg += "ERROR";
                    break;
                }
                case LogLevel::ASSERTION:
                {
                    this->msg += "ASSERT";
                    break;
                }
                default:
                {
                    this->msg += "INFO";
                    break;
                }
            }
            
            this->msg += "] ";
            
            this->msg = "[";
            //this->msg += DateTime::GetTime();
            this->msg += "] " + msg;
        }

        ~DebugInfoDesc()
        { }

        int line = 0;
        LogLevel debugType = (LogLevel)0; // NONE
        std::string fileName = "";
        std::string msg = "";
    };



    #define Log(message, ...) LogInternal(__FILE__, __LINE__, message, ##__VA_ARGS__)
    #define LogWarning(message, ...) LogWarningInternal(__FILE__, __LINE__, message, ##__VA_ARGS__)
    #define LogError(message, ...) LogErrorInternal(__FILE__, __LINE__, message, ##__VA_ARGS__)
    #define Assert(message, assertion, ...) AssertInternal(__FILE__, __LINE__, message, assertion, ##__VA_ARGS__)


    BOREALIS_API int LogInternal(const char* file, const int line, const char* message, ...);

    BOREALIS_API int LogWarningInternal(const char* file, const int line, const char* message, ...);

    BOREALIS_API int LogErrorInternal(const char* file, const int line, const char* message, ...);

    BOREALIS_API int AssertInternal(const char* file, const int line, const bool assertion, const char* message, ...);
}
