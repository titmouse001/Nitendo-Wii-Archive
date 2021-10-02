#ifndef Char_H_
#define Char_H_

#include "GCTypes.h"
class Image;

class CharInfo
{
private:

	Image*		m_pImage;

public:
	u16			m_uWidth;
	u16			m_uHeight;
	s16			m_iWidthA;
	u16			m_uWidthB;
	s16			m_iWidthC;
	s16			m_iLeftOffset;
	s16			m_iTopOffset;

	void		SetImage(Image* pImage) {m_pImage = pImage;}
	Image*		GetImage() const {return m_pImage;} 

	void		Draw(u32 uXpos, u32 uYpos, u32 uAlpha = 255,float Scaled=1.0) const;

	//this works with text, as it uses correct texture placing, NOT putting things down centered
	void		DrawDEBUGTEXT(u32 uXpos, u32 uYpos, u8 uAlpha = 255) const;
};

#endif