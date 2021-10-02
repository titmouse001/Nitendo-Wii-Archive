#include "Image.h"

#include "WiiManager.h"
#include "debug.h"
#include "tga.h"
#include "ImageManager.h"
#include <string.h>  // memset
#include "libpng/pngu/pngu.h"
#include "Util3D.h"

Image::Image() : m_Width(0) , m_Height(0), m_OriginX(0), m_OriginY(0)
{
}

// font support
Image::Image(CharInfo* pChar, u8* pAlphaData)
{
	// using fixed white and the alpha channel works well for fonts

	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 
	ImageManager* pImageManager( Wii.GetImageManager() );

	m_Width = pChar->m_uWidth;
	m_Height = pChar->m_uHeight;
	
	Tga::PIXEL* TgaData(NULL);
	if (m_Width==1)
	{
		// can't have a 1 pixel width - size will be increased.
		// (font tool could do this for us by generating chars of a multiple of say 2)
		u32 OriginalWidth(m_Width);
		m_Width=2;
		TgaData = new Tga::PIXEL [m_Width * m_Height]; 
		memset(TgaData,0,m_Width * m_Height * sizeof(TgaData));

		for (u16 y(0); y<m_Height; ++y)
		{
			for (u16 x(0); x<OriginalWidth; ++x)
			{
				TgaData[x + (m_Width*y)].r = 255; 
				TgaData[x + (m_Width*y)].g = 255;
				TgaData[x + (m_Width*y)].b = 255;
				TgaData[x + (m_Width*y)].a = pAlphaData[x + (OriginalWidth*y)];
			}
		}
	}
	else
	{
		TgaData = new Tga::PIXEL [m_Width * m_Height]; 
		for (u16 i(0); i<m_Width * m_Height; ++i)  // 1:1 mapping
		{
			TgaData[i].r = 255;
			TgaData[i].g = 255;
			TgaData[i].b = 255;
			TgaData[i].a = pAlphaData[i];
		}
	}

	m_OriginX = m_Width/2;
	m_OriginY = m_Height/2;
	m_ImageData = pImageManager->AllocateMemConvertToRGBA8((u8*)TgaData,m_Width,m_Height);

	delete [] TgaData;
	
	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);  

}

// support for TGA,PNG,JPG
Image::Image(string pFileName)
{
	ImageLoad(pFileName.c_str());
}

// support for TGA,PNG,JPG
void Image::ImageLoad(const char* pFileName)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 
	ImageManager* pImageManager( Wii.GetImageManager() );

	pImageManager->BeginGraphicsFile(pFileName);

	m_Width = pImageManager->GetTgaHeader().width;
	m_Height = pImageManager->GetTgaHeader().height;
	m_OriginX = m_Width/2;
	m_OriginY = m_Height/2;
	m_ImageData = pImageManager->AllocateMemConvertToRGBA8(pImageManager->GetTgaData(),m_Width,m_Height);

	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);

	pImageManager->EndGraphicsFile();
}

// support for TGA,PNG,JPG
Image::Image(u8* pTgaData, int Width, int Height)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 
	ImageManager* pImageManager( Wii.GetImageManager() );

	m_Width = Width;
	m_Height = Height;
	m_OriginX = m_Width/2;
	m_OriginY = m_Height/2;
	m_ImageData = pImageManager->AllocateMemConvertToRGBA8(pTgaData,m_Width,m_Height);
	
	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);  
}

// Create blank image (currenly pink to spot if somethings not completed)
Image::Image(u32 Width, u32 Height)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 
	ImageManager* pImageManager( Wii.GetImageManager() );

	Tga::PIXEL Col = {255,0,255,128};  // Should never see this, so purple to make it stand out
	Tga::PIXEL* TgaData ( new Tga::PIXEL [Width*Height] ); 
	for (u32 i(0); i<Width*Height; ++i)
	{
		TgaData[i] = Col;
	}

	m_Width = Width;
	m_Height = Height;
	m_OriginX = m_Width/2;
	m_OriginY = m_Height/2;
	m_ImageData = pImageManager->AllocateMemConvertToRGBA8((u8*)TgaData,m_Width,m_Height);

	delete [] TgaData;

	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);  
}


// Display Image using 2D coords - Origin centre
void Image::DrawImage(f32 xpos, f32 ypos)
{	
	Util3D::Trans(xpos,ypos);

	GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   

	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE); 
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_Begin(GX_QUADS, GX_VTXFMT2,4);	
	GX_Position2s16(-m_OriginX,-m_OriginY);	
	GX_Color1u16(0xFFFF);		
	GX_TexCoord2u16(0, 0);			
	GX_Position2s16(m_OriginX, -m_OriginY);
	GX_Color1u16(0xFFFF);
	GX_TexCoord2u16(1, 0);           
	GX_Position2s16(m_OriginX, m_OriginY);         
	GX_Color1u16(0xFFFF);
	GX_TexCoord2u16(1, 1);            
	GX_Position2s16(-m_OriginX, m_OriginY);
	GX_Color1u16(0xFFFF);    
	GX_TexCoord2u16(0, 1);      
	GX_End();
} 


// Display Alpha Image using 2D coords - Origin top left
void Image::DrawImageTL(f32 xpos, f32 ypos ,u8 Alpha )
{	
	GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   

	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE); 

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_Begin(GX_QUADS, GX_VTXFMT1,4);
	GX_Position3s16(xpos, ypos,0);	
	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);        
	GX_TexCoord2f32(0, 0);		
	GX_Position3s16(xpos+(m_Width), ypos,0);
	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);            
	GX_TexCoord2f32(1, 0);           
	GX_Position3s16(xpos+(m_Width), ypos+(m_Height),0);         
	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);        
	GX_TexCoord2f32(1, 1);           
	GX_Position3s16(xpos, ypos+(m_Height),0);
	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);              
	GX_TexCoord2f32(0, 1);         
	GX_End();
} 

// Display Alpha/Rotated/Scaled Image using 3D coords - Origin centre
void Image::DrawImageXYZ(f32 xpos, f32 ypos ,f32 z, u8 Alpha, f32 rad, f32 scale )
{
	f32 sx(m_OriginX);
	f32 sy(m_OriginY);	
	m_OriginX*=scale;
	m_OriginY*=scale;	
	DrawImageXYZ(xpos, ypos ,z,Alpha, rad );
	m_OriginX=sx;
	m_OriginY=sy;
}

// Display Alpha/Rotated Image using 3D coords - Origin centre
void Image::DrawImageXYZ(f32 xpos, f32 ypos ,f32 z, u8 Alpha, f32 rad )
{	
	Util3D::TransRot(xpos,ypos,z,rad);
	GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   

	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE); 
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_Begin(GX_QUADS, GX_VTXFMT1,4);	
	GX_Position3s16(-m_OriginX,-m_OriginY,0);	
	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);		
	GX_TexCoord2f32(0, 0);			
	GX_Position3s16(m_OriginX, -m_OriginY,0);
	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);
	GX_TexCoord2f32(1, 0);           
	GX_Position3s16(m_OriginX, m_OriginY,0);         
	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);
	GX_TexCoord2f32(1, 1);            
	GX_Position3s16(-m_OriginX, m_OriginY,0);
	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);    
	GX_TexCoord2f32(0, 1);      
	GX_End(); 
} 

void Image::RemoveImage()
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

//
//void Image::SetImageTo4x4RGBA(int x, int y, GXColor color) 
//{
//	u8 *truc( (u8*)GetImageData() );
//	u32 block( (((y >> 2)<<4) * GetWidth() ) + ((x >> 2)<<6) + (((y%4 << 2) + x%4 ) <<1) );
//
//	*(truc+block   )=color.a;
//	*(truc+block+1 )=color.r;
//	*(truc+block+32)=color.g;
//	*(truc+block+33)=color.b;
//}
//

//
//void Image::RefreshImage()
//{
//	u32 bufferSizeBytes = ( ((m_Width+3)/4*4) * ((m_Height+3)/4*4) ) * 4 ; //sizeof(GXColor) ;
//
//	DCFlushRange(m_ImageData,bufferSizeBytes);
//	memset(&m_HardwareTextureInfo,0,sizeof(m_HardwareTextureInfo));
//	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);
//	GX_Flush();
//}
