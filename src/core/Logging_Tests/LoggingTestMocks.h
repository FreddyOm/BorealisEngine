#pragma once

#include "../helpers/macros.h"
#include "../Logging/ILogger.h"

namespace Borealis::Core::Tests
{
    // Mock classes for logging tests

    class TestLogger : public ILogger
    {
       public:
        TestLogger() = default;
        virtual ~TestLogger() = default;

        BOREALIS_DELETE_COPY_ASSIGN(TestLogger)
        BOREALIS_DELETE_COPY_CONSTRUCT(TestLogger)
        BOREALIS_DELETE_MOVE_ASSIGN(TestLogger)
        BOREALIS_DELETE_MOVE_CONSTRUCT(TestLogger)

        void Log(const char* message) override
        {
            // Mock implementation for testing
        }

        void LogInfo(const char* message) override
        {
            // Mock implementation for testing
        }

        void LogWarning(const char* message) override
        {
            // Mock implementation for testing
        }

        void LogError(const char* message) override
        {
            // Mock implementation for testing
        }

        void SetLogLevel(LogLevel level) override { m_logLevel = level; }

        LogLevel GetLogLevel() const override { return m_logLevel; }

       private:
        LogLevel m_logLevel = LogLevel::Info;
    };
}    // namespace Borealis::Core::Tests