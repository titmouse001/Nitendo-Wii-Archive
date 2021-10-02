#include <gccore.h>
#include <stdio.h>
#include "WiiManager.h"
#include "Image.h"
#include "debug.h"
#include "ImageManager.h"
#include "InputDeviceManager.h"

#include "FontManager.h"

void FontManager::LoadFont(const string FontName)
{
	m_vFontData.push_back ( new Font(FontName)  );
	//m_vFontData.push_back ( new Font(FontName+"-1-sup")  );
}

//Font* FontManager::GetCharactersContainer(u32 uChar)
//{
//	for (TFontDetailsList::iterator Iter(m_vFontData.begin()); Iter< m_vFontData.end(); ++Iter)
//	{
//		if ( uChar >= (*Iter)->GetFirstChar() && uChar <= (*Iter)->GetLastChar() )
//			return (*Iter);
//	}
//	return NULL;
//}

Font* FontManager::GetFont()
{
	return ( *m_vFontData.begin() );
}