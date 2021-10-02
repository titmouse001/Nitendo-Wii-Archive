#include "WiiFile.h"
#include <stdlib.h>
#include "fat.h"
#include "debug.h"


#if (BYTE_ORDER == BIG_ENDIAN)
#define ENDIAN16(Value) (Value = ((Value&0x00ff)<<8) |  ((Value&0xff00)>>8))
#define ENDIAN32(Value) (Value = ((Value&0xff000000)>>24) |  ((Value&0x00ff0000)>>8) | ((Value&0x0000ff00)<<8) |  ((Value&0x000000ff)<<24))
#else
#define ENDIAN16(Value) (Value)
#define ENDIAN32(Value) (Value)
#endif


void WiiFile::InitFileSystem()
{
	if (!fatInitDefault()) 
	{
		ExitPrintf("error: calling fatInitDefault\n");
	}
}

FILE* WiiFile::FileOpenForRead(const char* const pFileName)
{
	FILE* pFile(fopen(pFileName,"rb"));
	if (pFile == NULL)
	{
		ExitPrintf("file not found '%s'\n",pFileName);
	}
	//fseek(pFile,0,SEEK_SET);

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
	// first byte read gives the length of string including null
	u8 size( WiiFile::ReadInt8(pFile) ); 
	char* str( new char[size] );

	if ( fread ( str, 1, size, pFile ) != size )
		exit(1);

	string Result = str;
	delete [] str;

	return Result;
}
