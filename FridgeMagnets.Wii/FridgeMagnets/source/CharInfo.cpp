#include "CharInfo.h"

#include "Image.h"
#include "WiiManager.h"
#include "FontManager.h"

void CharInfo::Draw(u32 uXpos, u32 uYpos, u32 uAlpha,float Scaled) const
{ 
	WiiManager& Wii = Singleton<WiiManager>::GetInstanceByRef();
	GXColor Col = Wii.GetFontManager()->GetFontColour();

	m_pImage->DrawImageWithColourScaled(uXpos,uYpos, Col.r,Col.g,Col.b,Scaled,uAlpha); 

}

void CharInfo::DrawDEBUGTEXT(u32 uXpos, u32 uYpos, u8 uAlpha) const
{ 
	m_pImage->DrawImageForDEBUGTEXT(uXpos, uYpos, uAlpha); 
}