#include "WiiFile.h"
#include <stdlib.h>
#include "fat.h"
#include "debug.h"
#include "ogcsys.h"
#include "config.h"

#if (BYTE_ORDER == BIG_ENDIAN)
#pragma message( "WiiFile using big-endian" ) 
#define ENDIAN16(Value) (Value = ((Value&0x00ff)<<8) |  ((Value&0xff00)>>8))
#define ENDIAN32(Value) (Value = ((Value&0xff000000)>>24) |  ((Value&0x00ff0000)>>8) | ((Value&0x0000ff00)<<8) |  ((Value&0x000000ff)<<24))
#else
#error ( "This looks bad for the wii, why are you using little-endian" ) 
#define ENDIAN16(Value) (Value)
#define ENDIAN32(Value) (Value)
#endif


std::string WiiFile::GetGamePath()
{
#ifdef BUILD_FINAL_RELEASE 
	std::string TempGamePath = ""; //  Final build - use path relative to the executable, must be blank
#else 
	#warning  "using debug mode - see Config.h"
	std::string TempGamePath = "sd://apps/GameFrameWork/"; // debug only
#endif

	return TempGamePath;
}

void WiiFile::SleepForMilisec(unsigned long milisec)  // TODO ..move to utils
{
    time_t sec = (int)(milisec / 1000);
    milisec -= (sec * 1000);
	timespec req = { sec, (milisec * 1000000L) };
    while (nanosleep(&req) == -1)
        continue;  // keep trying until it really does pause
}

void WiiFile::InitFileSystem()
{
	if (!fatInitDefault()) 
	{
		ExitPrintf("error: calling fatInitDefault (using emulator? UnMount 'sd.raw')\n");
	}
}

int WiiFile::GetFileSize(FILE* pFile) 
{
	int index( fseek(pFile, 0, SEEK_CUR) ); 
    fseek(pFile, 0, SEEK_END);   
    int FileLength( ftell(pFile) );   
    fseek(pFile, index, SEEK_SET);

	return FileLength;
}


FILE* WiiFile::FileOpenForRead(const char* const pFileName)
{

	FILE* pFile(fopen(pFileName,"rb"));
	if (pFile == NULL)
	{
		printf("file not found '%s'\n",pFileName);
		SleepForMilisec(1000*3);   
		exit(1);
	}
	else
	{
		printf("loading... '%s'\n",pFileName);
	}

	return pFile;
}

bool WiiFile::CheckFileExist(const char* FileName)
{
	if (FILE * file = fopen(FileName, "r"))
	{
		fclose(file);
		return true;
	}
	return false;
}

u32 WiiFile::ReadInt32(FILE* pFile)
{
	u32 uData;

	if ( fread ( &uData, 1, sizeof(uData), pFile ) != sizeof(uData) )
		exit(1);

	ENDIAN32(uData);
	return uData;
}

u16 WiiFile::ReadInt16(FILE* pFile)
{
	u16 uData;
	if ( fread ( &uData, 1, sizeof(uData), pFile ) != sizeof(uData) )
		exit(1);

	ENDIAN16(uData);
	return uData;
}

u8 WiiFile::ReadInt8(FILE* pFile)
{
	u8 uData;
	if ( fread ( &uData, 1, sizeof(uData), pFile ) != sizeof(uData) )
		exit(1);

	return uData;
}

string WiiFile::ReadString(FILE* pFile)
{
	// 'first byte read' gives the length of string (length given includes null)
	u8 size( WiiFile::ReadInt8(pFile) ); 
	char* str( new char[size] );

	if ( fread ( str, 1, size, pFile ) != size )
		exit(1);

	string Result = str;
	delete [] str;

	return Result;
}

void WiiFile::WriteInt32(s32 Value ,FILE* pFile )
{
	ENDIAN32(Value);

	if ( fwrite ( &Value, sizeof(Value), 1, pFile ) != 1 )
		exit(1);
}

void WiiFile::WriteInt16(s16 Value, FILE* pFile)
{
	ENDIAN16(Value);

	if ( fwrite ( &Value, sizeof(Value), 1, pFile ) != 1 )
		exit(1);
}