#include <vector>
#include <string.h>
#include <stdlib.h>
#include <gccore.h>

#include "WiiFile.h"
#include "Font.h"
#include "Image.h"
#include "Debug.h"
#include "Config.h"

using namespace WiiFile;

#ifdef BUILD_FINAL_RELEASE

//Final build
string TempGamePath = ""; //final version.  use path relative to the executable

#else

//debug only
#warning *** DONT FORGET TO REMOVE THIS IN THE FINAL BUILDS ***
string TempGamePath = "sd://apps/FridgeMagnets/"; 

#endif


Font::Font(const string& FileBase)
{
	CreateFontFromFile(FileBase);
}

void Font::CreateFontFromFile(const string& FileBase)
{
	//TODO join files
	string sTabFileName = TempGamePath + FileBase + ".ftab";
	FILE* pTabFile( FileOpenForRead(sTabFileName.c_str()) );

	string sRawFileName = TempGamePath + FileBase + ".fraw";
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



void Font::DisplayTextDebug(const string& Text, u32 uXpos, u32 uYpos, u8 Alpha) const
{
	s32 x(uXpos);
	for (string::const_iterator Iter(Text.begin()); Iter!=Text.end(); ++Iter )
	{
		const CharInfo* const  pChar ( GetChar( *Iter ) );

		x += pChar->m_iWidthA;
		if (pChar->GetImage() != NULL)
		{
			pChar->DrawDEBUGTEXT(x, uYpos + pChar->m_iTopOffset,Alpha);
		}
		x += pChar->m_uWidthB; 
		x += pChar->m_iWidthC;
	}
}

void Font::DisplayTextScaled(const string& Text, u32 uXpos, u32 uYpos,float Scale, u8 Alpha) const
{

//This is a hacked version of the above 'DisplayText' to allow for 'Fridge magnet' letters

// So all support functions called by this have been fudged for this to work

// It maybe better to do one of the following things instead of working this way:-
//	1) Add real images for each fridge magnet letter - so we don't fake it with this font, I'm sure it would look far better
//  2) Add a 3D object for each fridge magnet letter - that would be super, but pre-done 3D stuff is hard to get hold of


	string::const_iterator Iter(Text.begin());
	const CharInfo* const  pChar ( GetChar( *Iter ) );

	if (pChar->GetImage()!=NULL)   // note to myself: maybe just allocate empty memory for things like spaces - saves doing this step
		pChar->Draw(uXpos, uYpos, Alpha,Scale);

}