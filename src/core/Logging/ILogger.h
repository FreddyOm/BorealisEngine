#pragma once

#include "LogLevel.h"

namespace Borealis::Core
{
    /// <summary>
    /// The ILogger interface is used to log messages, warnings, and errors to the console or a file.
    /// </summary>
    struct ILogger
    {
        virtual ~ILogger() = default;

        /// <summary>
        /// Logs a simple message to the console or a file.
        /// </summary>
        /// <param name="message">The message to log</param>
        virtual void Log(const char* message) = 0;

        /// <summary>
        /// Logs an informational message to the console or a file.
        /// </summary>
        /// <param name="message">The message to log</param>
        virtual void LogInfo(const char* message) = 0;

        /// <summary>
        /// Logs a warning message to the console or a file.
        /// </summary>
        /// <param name="message">The message to log</param>
        virtual void LogWarning(const char* message) = 0;

        /// <summary>
        /// Logs an error message to the console or a file.
        /// </summary>
        /// <param name="message">The message to log</param>
        virtual void LogError(const char* message) = 0;

        /// <summary>
        /// Sets the log level for the logger.
        /// </summary>
        /// <param name="level">The log level to set</param>
        virtual void SetLogLevel(LogLevel level) = 0;

        /// <summary>
        /// Returns the current log level for the logger.
        /// </summary>
        /// <returns>The current log level.</returns>
        virtual LogLevel GetLogLevel() const = 0;
    };
}    // namespace Borealis::Core