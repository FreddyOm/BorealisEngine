#include "BorealisLogger.h"

#include <cstdio>

namespace Borealis::Core
{
    BorealisLogger::BorealisLogger() : m_logLevel(LogLevel::Info) { }

    BorealisLogger::~BorealisLogger() { }

    void BorealisLogger::Log(const char* message)
    {
        if(m_logLevel >= LogLevel::Info)
        {
            printf("[INFO]: %s\n", message);
        }
    }

    void BorealisLogger::LogInfo(const char* message)
    {
        if(m_logLevel >= LogLevel::Info)
        {
            printf("[INFO]: %s\n", message);
        }
    }

    void BorealisLogger::LogWarning(const char* message)
    {
        if(m_logLevel >= LogLevel::Warning)
        {
            printf("[WARNING]: %s\n", message);
        }
    }

    void BorealisLogger::LogError(const char* message)
    {
        if(m_logLevel >= LogLevel::Error)
        {
            printf("[ERROR]: %s\n", message);
        }
    }

    void BorealisLogger::SetLogLevel(LogLevel level) { m_logLevel = level; }

    LogLevel BorealisLogger::GetLogLevel() const { return m_logLevel; }
}    // namespace Borealis::Core