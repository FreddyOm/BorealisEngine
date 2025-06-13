#pragma once

#include <stdarg.h>
#include <utility>
#include <string>

#include "../../config.h"
#include "../types/types.h"
#include "logger_internal.h"

#ifdef BOREALIS_WIN
    #include <comdef.h>
#endif

#ifndef THROW_ON_ASSERT
#define THROW_ON_ASSERT
#endif

namespace Borealis::Debug
{
    enum class LogLevel
    {
        INFO,
        WARNING,
        ERR,
        ASSERTION,
    };

    struct DebugInfoDesc // 24 byte
    {
        DebugInfoDesc(const std::string msg, const std::string file, const Borealis::Types::int16 line, const LogLevel type)
            : line(line), logType(type), fileName(file)
        {
            this->msg = "[";

            switch (type)
            {
                case LogLevel::WARNING:
                {
                    this->msg += "WARNING";
                    break;
                }
                case LogLevel::ERR:
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
            
            this->msg += "[";
            //this->msg += DateTime::GetTime();
            this->msg += "] " + msg;
        }

        ~DebugInfoDesc()
        { }

        Borealis::Types::int16 line = 0;
        LogLevel logType = (LogLevel)0; // NONE
        std::string fileName = "";
        std::string msg = "";
    };

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

#ifdef BOREALIS_WIN
    #define StrFromHResult(hr) _com_error(hr).ErrorMessage()
#else
    #define StrFromHResult(hr)
#endif
    
    #define Log(message, ...) Borealis::Debug::LogInternal(__FILE__, __LINE__, message, ##__VA_ARGS__)
    #define LogWarning(message, ...) Borealis::Debug::LogWarningInternal(__FILE__, __LINE__, message, ##__VA_ARGS__)
    #define LogError(message, ...) Borealis::Debug::LogErrorInternal(__FILE__, __LINE__, message, ##__VA_ARGS__)
    #define Assert(assertion, message, ...) Borealis::Debug::AssertInternal(__FILE__, __LINE__, assertion, message, ##__VA_ARGS__)

#else

    #define StrFromHResult(hr)

    #define Log(message, ...)
    #define LogWarning(message, ...)
    #define LogError(message, ...)
    #define Assert(message, assertion, ...)

#endif

    BOREALIS_API Borealis::Types::int16 LogInternal(const char* file, const Borealis::Types::int16 line, const char* message, ...);

    BOREALIS_API Borealis::Types::int16 LogWarningInternal(const char* file, const Borealis::Types::int16 line, const char* message, ...);

    BOREALIS_API Borealis::Types::int16 LogErrorInternal(const char* file, const Borealis::Types::int16 line, const char* message, ...);

    BOREALIS_API Borealis::Types::int16 AssertInternal(const char* file, const Borealis::Types::int16 line, const bool assertion, const char* message, ...);
}
