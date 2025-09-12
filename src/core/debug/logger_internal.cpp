#include "logger_internal.h"
#include "logger.h"

#ifdef BOREALIS_WIN 
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace Borealis::Debug
{
	Borealis::Types::int16 LogMessageInternal(const DebugInfoDesc desc)
	{
        #ifdef BOREALIS_WIN
        
        static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        switch (desc.logType)
        {
            case LogLevel::WARNING:
            {
                SetConsoleTextAttribute(hConsole, 14);
                break;
            }
            case LogLevel::ERR:
            {
                SetConsoleTextAttribute(hConsole, 12);
                break;
            }
            case LogLevel::ASSERTION:
            {
                SetConsoleTextAttribute(hConsole, 12);
                break;
            }
            default:
            {
                SetConsoleTextAttribute(hConsole, 15);
                break;
            }
        }
        #endif

		const Borealis::Types::int16 result = printf("%s", desc.msg.c_str());
		printf("\n%s, %u\n", desc.fileName.c_str(), desc.line);
		
        #ifdef BOREALIS_WIN
        SetConsoleTextAttribute(hConsole, 15);
        #endif
        
        return result;
	}
}