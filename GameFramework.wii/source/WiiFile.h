#ifndef FILE_H_
#define FILE_H_

#include "stdio.h"
#include "GCTypes.h"
#include <string>
using namespace std;

namespace WiiFile 
{

	std::string GetGamePath();
	
	void	SleepForMilisec(unsigned long milisec); //TEMP

	void	InitFileSystem();

	FILE*	FileOpenForRead(const char* const pFileName);
	bool	CheckFileExist(const char* FileName);

	int		GetFileSize(FILE* pFile);

	u32		ReadInt32(FILE* pFile);
	u16		ReadInt16(FILE* pFile);
	u8		ReadInt8(FILE* pFile);
	string	ReadString(FILE* pFile);
	
	void		WriteInt32( s32 val,FILE* pFile);
	void		WriteInt16( s16 val,FILE* pFile);

};

#endif

