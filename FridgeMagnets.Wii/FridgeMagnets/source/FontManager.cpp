#include <gccore.h>
#include <stdio.h>
#include "WiiManager.h"
#include "Image.h"
#include "debug.h"
#include "ImageManager.h"
#include "InputDeviceManager.h"

#include "FontManager.h"

void FontManager::LoadFont(const string& FontName)
{
	m_vFontData.push_back ( new Font(FontName)  );
}

const Font* const FontManager::GetFont() const
{
	return ( *m_vFontData.begin() );
}