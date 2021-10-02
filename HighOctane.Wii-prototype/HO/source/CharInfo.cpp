#include "CharInfo.h"

#include "Image.h"


void CharInfo::Draw(u32 uXpos, u32 uYpos, u32 uAlpha) const
{ 
	m_pImage->DrawImageFor3D(uXpos, uYpos, uAlpha); 
}