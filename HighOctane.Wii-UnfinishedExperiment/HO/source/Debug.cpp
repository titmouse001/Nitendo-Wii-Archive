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

	Singleton<WiiManager>::GetInstanceByRef().ResetSwapScreen();

	printf("\n\t\t%s\n\t\tPress 'Home' again to exit",cBuffer);

	do 
	{
		VIDEO_WaitVSync();
		WPAD_ScanPads();
	} while( (WPAD_ButtonsUp(0) & WPAD_BUTTON_HOME)== 0 );

	printf("\n\t\tExiting...");

	exit(1);

}


#endif
