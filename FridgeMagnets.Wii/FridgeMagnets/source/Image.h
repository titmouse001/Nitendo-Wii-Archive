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


	void DrawImage(u8 Alpha = 0xff , u8 r = 0xff, u8 g = 0xff, u8 b = 0xff );

	void DrawImageWithColourScaled(f32 xpos, f32 ypos , u8 Red, u8 Green, u8 Blue, float fScale = 1.0, u8 Alpha=0xff );


void DrawImageForDEBUGTEXT(f32 xpos, f32 ypos ,u8 Alpha )
{	
	GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   

	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE); 
	
	GX_ClearVtxDesc();
	GX_SetVtxDesc (GX_VA_POS,GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_Begin(GX_QUADS, GX_VTXFMT1,4);		// format: GX_S16#GX_RGBA8#GX_F32
 
	GX_Position3s16(xpos, ypos,0);			// GX_S16
	GX_Color4u8(0,0,0,Alpha);        
	GX_TexCoord2f32(0, 0);					// GX_F32

	GX_Position3s16(xpos+(m_Width), ypos,0);
	GX_Color4u8(0,0,0,Alpha);            
	GX_TexCoord2f32(1, 0);           

	GX_Position3s16(xpos+(m_Width), ypos+(m_Height),0);         
	GX_Color4u8(0,0,0,Alpha);        
	GX_TexCoord2f32(1, 1);            

	GX_Position3s16(xpos, ypos+(m_Height),0);
	GX_Color4u8(0,0,0,Alpha);              
	GX_TexCoord2f32(0, 1);         
	
	GX_End();

	GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);         
	GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);  
} 

	void DrawImageWithColour(f32 xpos, f32 ypos , u8 Red, u8 Green, u8 Blue,u8 Alpha=0xff);

    u32* AllocateMemConvertToRGBA8(u8* rgbaBuffer, u16 bufferWidth, u16 bufferHeight);

	u32* GetImageData() const { return m_ImageData; }

	u16 GetWidth() { return m_Width; }
	u16 GetHeight() { return m_Height; }
	s16 GetOriginX() { return m_OriginX; }
	s16 GetOriginY() { return m_OriginY; }

//	void DrawImageScaled(f32 xpos, f32 ypos, u8 Alpha, float fScale );

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


private:

	u16		m_Width;
	u16		m_Height;
	s16		m_OriginX;
	s16		m_OriginY;
	u32*	m_ImageData;

	GXTexObj	m_HardwareTextureInfo;
};



#endif
