#include <string>
#include <stdlib.h>
#include "WiiFile.h"
#include "Font.h"
#include "Image.h"
#include "Debug.h"
#include "Config.h"


Font::Font(const std::string& FileName)
{
	CreateFontFromFile(FileName);
}

void Font::CreateFontFromFile(const std::string& FileName)
{
	//TODO join files
	string sTabFileName = FileName + ".ftab";
	FILE* pTabFile( WiiFile::FileOpenForRead(sTabFileName.c_str()) );

	string sRawFileName = FileName + ".fraw";
	FILE* pRawFile( WiiFile::FileOpenForRead(sRawFileName.c_str()) );

	char Header[4];
	u32	uCharBytesPerPixel(0);

	if ( fread ( Header, 1, 4, pTabFile ) != 4 )
		ExitPrintf("failed to read ftab header");
	if ( strncmp ( Header, "FNT1", 4 ) == 0 )
		uCharBytesPerPixel = 1;
	else
		ExitPrintf("Font not supported");

	SetFirstChar( WiiFile::ReadInt32(pTabFile) );
	SetLastChar ( WiiFile::ReadInt32(pTabFile) );
	SetHeight ( WiiFile::ReadInt16(pTabFile) );
	SetBaseLine ( WiiFile::ReadInt16(pTabFile) );
	SetLeading ( WiiFile::ReadInt16(pTabFile) );
	SetLineSpacing ( WiiFile::ReadInt16(pTabFile) );
	SetUnderline ( WiiFile::ReadInt16(pTabFile) );
	SetStrikeout ( WiiFile::ReadInt16(pTabFile) );
	/* ignore value = */ WiiFile::ReadInt32(pTabFile);
	/* ignore value = */ WiiFile::ReadInt32(pTabFile);

	if ( GetLastChar() < GetFirstChar() )
		ExitPrintf("last char < first char");

	u32	uNumChars( GetLastChar() - GetFirstChar() + 1);

	for ( u32 i(0); i < uNumChars; i++ )
	{
		CharInfo charinfo;
		charinfo.m_uWidth = WiiFile::ReadInt16(pTabFile);
		charinfo.m_uHeight = WiiFile::ReadInt16(pTabFile);
		charinfo.m_iWidthA = WiiFile::ReadInt16(pTabFile);
		charinfo.m_uWidthB = WiiFile::ReadInt16(pTabFile);
		charinfo.m_iWidthC = WiiFile::ReadInt16(pTabFile);
		charinfo.m_iLeftOffset = 0;
		charinfo.m_iTopOffset = WiiFile::ReadInt16(pTabFile);
		/* ignore value = */ WiiFile::ReadInt32(pTabFile);

		u32 size (charinfo.m_uWidth * charinfo.m_uHeight * uCharBytesPerPixel);
		charinfo.SetImage(NULL); // Blank char's are  NULL as no memory needs to be allocated.
		if ( size > 0 )
		{
			u8* pRawdata( new u8[size] ); //for the raw character bitmap...
			if ( pRawdata == NULL )
				exit (1);
			if ( fread ( pRawdata, 1, size, pRawFile ) != size )
			{
				delete [] pRawdata;
				ExitPrintf("Failed to read raw data %s",sRawFileName.c_str());
			}
			charinfo.SetImage( new Image(&charinfo,pRawdata) );
		}
		StoreCharInfo(charinfo);
	}
	fclose ( pRawFile );
	fclose ( pTabFile );
}

const CharInfo*	const Font::GetChar(u32 uChar) const
{
	if (uChar <= m_uLastChar)
		return &m_vCharInfo[ uChar - m_uFirstChar ]; 
	else
		return &m_vCharInfo[ m_uLastChar ];	// fallback to something that should stand out
}
