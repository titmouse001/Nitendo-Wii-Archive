#ifndef DEBUG_H_
#define DEBUG_H_

#include "Debug.h"
#include "GCTypes.h"

#include <gccore.h>
#include <wiiuse/wpad.h>

#include <stdarg.h> 
#include <stdio.h> 
#include <stdlib.h> 

#include "Singleton.h"
#include "WiiManager.h"

//#define ENABLE_DEBUGPRINTF

FILE* DebugLoggingFile;

void Debug::StartDebugLogging()
{
//#if defined (ENABLE_DEBUGPRINTF)
    DebugLoggingFile = fopen("sd:/log.txt", "wt");
	if (DebugLoggingFile==NULL)
		ExitPrintf("debug logging failed");
	fprintf(DebugLoggingFile, "- %s %s Start Debug Logging\n",__DATE__,__TIME__);
//	return DebugLoggingFile;
//#endif
}

void Debug::StopDebugLogging()
{
//#if defined (ENABLE_DEBUGPRINTF)
	fprintf(DebugLoggingFile, "\n- Stop Debug Logging");
    fclose(DebugLoggingFile);
//#endif
}


/*!
\namespace	Debug
\brief		A namespace to hold debug functions to be used wherever.
*/
void Debug::_ExitPrintf(const char* pFormat, ...)
{
	static const u32 BufferSize(128);

	va_list tArgs;
	va_start(tArgs, pFormat);
	char cBuffer[BufferSize+1];
	vsnprintf(cBuffer, BufferSize, pFormat, tArgs);	
	va_end(tArgs);

	if (Singleton<WiiManager>::GetInstanceByRef().GetScreenBufferId() == 1)  // switch to buffer 0, so printf outputs to buffer ZERO
	{
		Singleton<WiiManager>::GetInstanceByRef().SwapScreen();
	}
	printf(cBuffer);
	printf("\nPress 'Home'");

	do 
	{
		VIDEO_WaitVSync();
		WPAD_ScanPads();
	} while( (WPAD_ButtonsUp(0) & WPAD_BUTTON_HOME)== 0 );

	printf("\nExiting");

	exit(1);

}

void Debug::_DebugPrintf(const char* pFormat, ...)
{
	static const u32 BufferSize(128);

	va_list tArgs;
	va_start(tArgs, pFormat);
	char cBuffer[BufferSize+1];
	vsnprintf(cBuffer, BufferSize, pFormat, tArgs);	
	va_end(tArgs);

	//fprintf(DebugLoggingFile,"LINE %06d FILE %s, FUNC %s\n", __LINE__,__FILE__,__FUNCTION__);
	fprintf(DebugLoggingFile, "%s\n", cBuffer);
	fflush(DebugLoggingFile);
}

#endif
