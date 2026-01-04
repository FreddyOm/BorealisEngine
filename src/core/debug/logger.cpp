#include "logger.h"
#include <cstring>
#include <assert.h>

namespace Borealis::Debug
{
	char logInfoBuffer[1024] = { 0 };

	Borealis::Types::int16 LogInternal(const char* file, const Borealis::Types::int16 line, const char* message, ...)
	{
		memset(&logInfoBuffer, 0, 1024);
		
		va_list args;
		va_start(args, message);
		vsnprintf(logInfoBuffer, 1024, message, args);
		va_end(args);

		DebugInfoDesc debugInfo(logInfoBuffer, file, line, LogLevel::INFO);

		return LogMessageInternal(debugInfo);
	}


	Borealis::Types::int16 LogWarningInternal(const char* file, const Borealis::Types::int16 line, const char* message, ...)
	{
		memset(&logInfoBuffer, 0, 1024);

		va_list argList;
		va_start(argList, message);
		vsnprintf(logInfoBuffer, 1024, message, argList);
		va_end(argList);

		DebugInfoDesc debugInfo(logInfoBuffer, file, line, LogLevel::WARNING);

		return LogMessageInternal(debugInfo);
	}

	Borealis::Types::int16 LogErrorInternal(const char* file, const Borealis::Types::int16 line, const char* message, ...)
	{
		memset(&logInfoBuffer, 0, 1024);

		va_list argList;
		va_start(argList, message);
		vsnprintf(logInfoBuffer, 1024, message, argList);
		va_end(argList);

		DebugInfoDesc debugInfo(logInfoBuffer, file, line, LogLevel::ERR);

		return LogMessageInternal(debugInfo);
	}

	inline Borealis::Types::int16 AssertInternal(const char* file, const Borealis::Types::int16 line, const bool assertion, const char* message, ...)
	{
		if (assertion) { return 0; }
			
		memset(&logInfoBuffer, 0, 1024);

		va_list argList;
		va_start(argList, message);
		vsnprintf(logInfoBuffer, 1024, message, argList);
		va_end(argList);

		DebugInfoDesc debugInfo(logInfoBuffer, file, line, LogLevel::ASSERTION);

		Types::int16 returnVal = LogMessageInternal(debugInfo);

#ifdef THROW_ON_ASSERT
		assert(false && debugInfo.msg.c_str());
		//throw std::exception(debugInfo.msg.c_str());
		//exit(668);
#endif
		return returnVal;
	}
}