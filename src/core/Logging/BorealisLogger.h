#pragma once

#include "../helpers/macros.h"
#include "ILogger.h"

namespace Borealis::Core
{
    class BorealisLogger : public ILogger
    {
       public:
        BorealisLogger();
        virtual ~BorealisLogger();

        BOREALIS_DELETE_COPY_ASSIGN(BorealisLogger)
        BOREALIS_DELETE_COPY_CONSTRUCT(BorealisLogger)
        BOREALIS_DELETE_MOVE_ASSIGN(BorealisLogger)
        BOREALIS_DELETE_MOVE_CONSTRUCT(BorealisLogger)

        void Log(const char* message) override;
        void LogInfo(const char* message) override;
        void LogWarning(const char* message) override;
        void LogError(const char* message) override;

        void SetLogLevel(LogLevel level) override;
        LogLevel GetLogLevel() const override;

       private:
        LogLevel m_logLevel = LogLevel::None;
    };
}    // namespace Borealis::Core