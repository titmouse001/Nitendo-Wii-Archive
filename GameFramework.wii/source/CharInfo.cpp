#include "CharInfo.h"
#include "Image.h"

void CharInfo::Draw(int uXpos, int uYpos, u32 uAlpha) const
{ 
	m_pImage->DrawImageTL(uXpos,uYpos,uAlpha); // Top left
}