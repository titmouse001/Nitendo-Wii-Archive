#ifndef FILE_H_
#define FILE_H_

#include "stdio.h"
#include "GCTypes.h"
#include <string>
using namespace std;

namespace WiiFile 
{
	void	InitFileSystem();

	FILE*	FileOpenForRead(const char* const pFileName);
	bool	CheckFileExist(const char* FileName);

	u32		ReadInt32(FILE* pFile);
	u16		ReadInt16(FILE* pFile);
	u8		ReadInt8(FILE* pFile);
	string	ReadString(FILE* pFile);
};

#endif

