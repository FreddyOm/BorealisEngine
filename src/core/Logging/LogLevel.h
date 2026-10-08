#pragma once

enum class LogLevel
{
    None = 1 << 0,
    Error = 1 << 1,
    Warning = 1 << 2,
    Info = 1 << 3,
};