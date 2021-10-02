#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "GCTypes.h"
#include <string.>
#include <stdarg.h> 


#define ExitPrintf			Debug::_ExitPrintf
#define LogPrintf			Debug::_DebugPrintf

extern FILE* DebugLoggingFile;

namespace Debug 
{
	void _ExitPrintf(const char* pFormat, ...);
	//log to file
	void _DebugPrintf(const char* pFormat, ...);

	void StartDebugLogging();
	void StopDebugLogging();
};


#endif
