#include <vector>
#include <string.h>
#include <stdlib.h>
#include <gccore.h>

#include "WiiFile.h"
#include "Font.h"
#include "Image.h"
#include "Debug.h"


using namespace WiiFile;


//string TempGamePath = "sd://";

Font::Font(const string FileBase)
{
	CreateFontFromFile(FileBase);
}

void Font::CreateFontFromFile(const string& FileBase)
{
	//TODO join files
	string sTabFileName = FileBase + ".ftab";
	FILE* pTabFile( FileOpenForRead(sTabFileName.c_str()) );

	string sRawFileName = FileBase + ".fraw";
	FILE* pRawFile( FileOpenForRead(sRawFileName.c_str()) );

	char Header[4];
	u32	uCharBytesPerPixel(0);

	if ( fread ( Header, 1, 4, pTabFile ) != 4 )
		ExitPrintf("failed to read ftab header");
	if ( strncmp ( Header, "FNT1", 4 ) == 0 )
		uCharBytesPerPixel = 1;
	else
		ExitPrintf("Font not supported");

	SetFirstChar( ReadInt32(pTabFile) );
	SetLastChar ( ReadInt32(pTabFile) );
	SetHeight ( ReadInt16(pTabFile) );
	SetBaseLine ( ReadInt16(pTabFile) );
	SetLeading ( ReadInt16(pTabFile) );
	SetLineSpacing ( ReadInt16(pTabFile) );
	SetUnderline ( ReadInt16(pTabFile) );
	SetStrikeout ( ReadInt16(pTabFile) );
	/* ignore value = */ ReadInt32(pTabFile);
	/* ignore value = */ ReadInt32(pTabFile);

	if ( GetLastChar() < GetFirstChar() )
		ExitPrintf("last char < first char");

	u32	uNumChars( GetLastChar() - GetFirstChar() + 1);
	for ( u32 i(0); i < uNumChars; i++ )
	{
		CharInfo charinfo;
		charinfo.m_uWidth = ReadInt16(pTabFile);
		charinfo.m_uHeight = ReadInt16(pTabFile);
		charinfo.m_iWidthA = ReadInt16(pTabFile);
		charinfo.m_uWidthB = ReadInt16(pTabFile);
		charinfo.m_iWidthC = ReadInt16(pTabFile);
		charinfo.m_iLeftOffset = 0;
		charinfo.m_iTopOffset = ReadInt16(pTabFile);
		/* ignore value = */ ReadInt32(pTabFile);

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



void Font::DisplayText(const string& Text, u32 uXpos, u32 uYpos)
{
	s32 x(uXpos);
	for (string::const_iterator Iter(Text.begin()); Iter!=Text.end(); ++Iter )
	{
		const CharInfo* const  pChar ( GetChar( *Iter ) );

		x += pChar->m_iWidthA;
		if (pChar->GetImage() != NULL)
		{
			pChar->Draw(x, uYpos + pChar->m_iTopOffset);
		}
		x += pChar->m_uWidthB; 
		x += pChar->m_iWidthC;
	}
}