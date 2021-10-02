#ifndef Image_H
#define Image_H

#include "GCTypes.h"
#include "ogc/gx.h"
#include <malloc.h>
#include <string.h>
#include <gccore.h>
#include <string>
using namespace std;

class CharInfo;

class Image
{
public:

	Image();
	Image(string pFileName);
	Image(const char* pFileName);
	Image(u32 Width, u32 Height);
	Image(/*Font::*/CharInfo* pChar, u8* pAlphaData);

	Image(u8* pTgaData, int Width , int Height);

	void DrawImage(f32 xpos, f32 ypos, u8 Alpha = 0xff);
	void DrawImageNoAlpha(f32 xpos, f32 ypos);

	void DrawImageNoAlphaFor3D(s32 xpos, s32 ypos);
	void DrawImageFor3D(f32 xpos, f32 ypos, u8 Alpha = 0xff);

	//void Add_GX_TriStrip(f32 xpos, f32 ypos);

    u32* AllocateMemConvertToRGBA8(u8* rgbaBuffer, u16 bufferWidth, u16 bufferHeight);

	u32* GetImageData() const { return m_ImageData; }

//	void Set(u32* pImage) 
//	{ 
////		m_Width = pImage->m_Width;
////		m_Height = pImage->m_Height;
////		m_OriginX = pImage->m_OriginX;
////		m_OriginY = pImage->m_OriginY;
////		m_HardwareTextureInfo = pImage->m_HardwareTextureInfo;
//		m_ImageData = pImage; 
//	}


	//void Set(Image* pImage) 
	//{ 
	//	m_Width = pImage->m_Width;
	//	m_Height = pImage->m_Height;
	//	m_OriginX = pImage->m_OriginX;
	//	m_OriginY = pImage->m_OriginY;
	//	m_HardwareTextureInfo = pImage->m_HardwareTextureInfo;
	//	m_ImageData = pImage->m_ImageData; 
	//}


	u16 GetWidth() const { return m_Width; }
	u16 GetHeight() const { return m_Height; }

	void DrawImageScaled(f32 xpos, f32 ypos, u8 Alpha, float fScale );

	void Refresh();

	void SetPixelTo4x4RGBA(int x, int y, GXColor color) 
	{
		u8 *truc( (u8*)GetImageData() );
		u32 block( (((y >> 2)<<4) * GetWidth() ) + ((x >> 2)<<6) + (((y%4 << 2) + x%4 ) <<1) );

		*(truc+block   )=color.a;
		*(truc+block+1 )=color.r;
		*(truc+block+32)=color.g;
		*(truc+block+33)=color.b;
	}

	void Remove()
	{
		if (m_ImageData!=NULL)
		{
			m_Width=0;
			m_Height=0;
			m_OriginX=0;
			m_OriginY=0;
			memset(&m_HardwareTextureInfo,0,sizeof(m_HardwareTextureInfo));
			free(m_ImageData);  // cleanup from memalign 
			m_ImageData = NULL;
		}
	}

	GXTexObj	m_HardwareTextureInfo;
private:

	u16		m_Width;
	u16		m_Height;
	s16		m_OriginX;
	s16		m_OriginY;
	u32*	m_ImageData;

	//GXTexObj	m_HardwareTextureInfo;
};



#endif
