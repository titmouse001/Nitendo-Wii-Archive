#include "Image.h"

#include "WiiManager.h"
#include "debug.h"
#include "tga.h"
#include "ImageManager.h"

#include <gccore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>  // memset
#include <malloc.h>
#include <dirent.h>
#include "libpng/pngu/pngu.h"


// need to look... think this is missing free memory stuff???

Image::Image() : m_Width(0) , m_Height(0), m_OriginX(0), m_OriginY(0)
{
}

Image::Image(string pFileName)
{
	Image( pFileName.c_str() );
}

Image::Image(const char* pFileName)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 
	ImageManager* pImageManager( Wii.GetImageManager() );

	pImageManager->BeginGraphicsFile(pFileName);

	m_Width = pImageManager->GetTgaHeader().width;
	m_Height = pImageManager->GetTgaHeader().height;
	m_OriginX = m_Width/2;
	m_OriginY = m_Height/2;
	m_ImageData = AllocateMemConvertToRGBA8(pImageManager->GetTgaData(),m_Width,m_Height);

	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);

	pImageManager->EndGraphicsFile();

	//std::string Name(pFileName);

	//for(int i=0; i<(int)Name.size();++i)
	//	Name[i] = tolower(Name[i]);

	//if(Name.substr(Name.find_last_of(".") + 1) == "png")
	//{
	//	PNGUPROP imgProp;
	//	IMGCTX ctx;
	//	
	//	ctx = PNGU_SelectImageFromDevice (pFileName);
	//	PNGU_GetImageProperties (ctx, &imgProp);

	//	u32 bufferSizeBytes = ( ((imgProp.imgWidth+3)/4*4) * ((imgProp.imgHeight+3)/4*4) ) * 4 ; //sizeof(GXColor) ;
	//	m_ImageData = (u32 *)memalign(32, bufferSizeBytes);

	//	m_Width = imgProp.imgWidth;
	//	m_Height = imgProp.imgHeight;
	//	m_OriginX = m_Width/2;
	//	m_OriginY = m_Height/2;

	//	PNGU_DecodeTo4x4RGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, m_ImageData, 255);
	//	DCFlushRange (m_ImageData, imgProp.imgWidth * imgProp.imgHeight * 4);
	//	PNGU_ReleaseImageContext (ctx);
	//}
	//else if (Name.substr(Name.find_last_of(".") + 1) == "tga")
	//{
	//	Tga::TGA_HEADER TgaHeader;
	//	u8* TgaData(Tga::LoadTGA(pFileName,TgaHeader)); 

	//	m_Width = TgaHeader.width;
	//	m_Height = TgaHeader.height;
	//	m_OriginX = m_Width/2;
	//	m_OriginY = m_Height/2;
	//	m_ImageData = AllocateMemConvertToRGBA8(TgaData,m_Width,m_Height);

	//	Tga::UnLoadTGA((u8*)TgaData);
	//}
	//
	//GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);  
}

Image::Image(u8* pTgaData, int Width, int Height)
{
	m_Width = Width;
	m_Height = Height;
	m_OriginX = m_Width/2;
	m_OriginY = m_Height/2;
	m_ImageData = AllocateMemConvertToRGBA8(pTgaData,m_Width,m_Height);
	
	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);  
}

Image::Image(u32 Width, u32 Height)
{
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
	m_ImageData = AllocateMemConvertToRGBA8((u8*)TgaData,m_Width,m_Height);

	delete [] TgaData;

	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);  

}

Image::Image(/*Font::*/CharInfo* pChar, u8* pAlphaData)
{
	m_Width = pChar->m_uWidth;
	m_Height = pChar->m_uHeight;
	
	Tga::PIXEL* TgaData(NULL);
	if (m_Width==1)
	{
		// can't have a 1 pixel width - size will be increased.
		u32 OriginalWidth(m_Width);
		m_Width=2;
		TgaData = new Tga::PIXEL [m_Width * m_Height]; 
		memset(TgaData,0,m_Width * m_Height * sizeof(TgaData));
		for (u16 y(0); y<m_Height; ++y)
		{
			for (u16 x(0); x<OriginalWidth; ++x)
			{
				TgaData[x + (m_Width*y)].r = 255;// pAlphaData[x + (OriginalWidth*y)];
				TgaData[x + (m_Width*y)].g = 255;// pAlphaData[x + (OriginalWidth*y)];
				TgaData[x + (m_Width*y)].b = 255;// pAlphaData[x + (OriginalWidth*y)];
				TgaData[x + (m_Width*y)].a = pAlphaData[x + (OriginalWidth*y)]; // 0;
			//	if (pAlphaData[x + (OriginalWidth*y)]!=0)
			//		TgaData[x + (m_Width*y)].a = 255;
			}
		}
	}
	else
	{
		TgaData = new Tga::PIXEL [m_Width * m_Height]; 
		for (u16 i(0); i<m_Width * m_Height; ++i)
		{
			TgaData[i].r = 255;//pAlphaData[i];
			TgaData[i].g = 255;//pAlphaData[i];
			TgaData[i].b = 255;//pAlphaData[i];
			TgaData[i].a = pAlphaData[i];
			//if (pAlphaData[i]!=0)
			//	TgaData[i].a = 255;
		}
	}

	m_OriginX = m_Width/2;
	m_OriginY = m_Height/2;
	m_ImageData = AllocateMemConvertToRGBA8((u8*)TgaData,m_Width,m_Height);

	delete [] TgaData;

	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);  

}


void Image::DrawImageWithColourScaled(f32 xpos, f32 ypos , u8 Red, u8 Green, u8 Blue, float fScale, u8 Alpha)
{  
	s16 KeepX(m_OriginX);
	s16 KeepY(m_OriginY);
	m_OriginX=(float)m_OriginX*fScale;
	m_OriginY=(float)m_OriginY*fScale;

	DrawImageWithColour(xpos, ypos,Red,Green,Blue,Alpha );

	m_OriginX = KeepX;
	m_OriginY = KeepY;
}

void Image::DrawImageWithColour(f32 xpos, f32 ypos , u8 Red, u8 Green, u8 Blue,u8 Alpha)
{	
//	u8 Alpha = 0xff;
	GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   

	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE); 
	GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);

	GX_Begin(GX_QUADS, GX_VTXFMT1,4);		// format: GX_S16#GX_RGBA8#GX_F32
 
	GX_Position3s16(xpos-(m_OriginX), ypos-(m_OriginY),0);			// GX_S16
	GX_Color4u8(Red,Green,Blue,Alpha);     
	GX_TexCoord2f32(0, 0);					// GX_F32

	GX_Position3s16(xpos+(m_OriginX), ypos-(m_OriginY),0);
	GX_Color4u8(Red,Green,Blue,Alpha);     
	GX_TexCoord2f32(1, 0);           

	GX_Position3s16(xpos+(m_OriginX), ypos+(m_OriginY),0);         
	GX_Color4u8(Red,Green,Blue,Alpha);     
	GX_TexCoord2f32(1, 1);            

	GX_Position3s16(xpos-(m_OriginX), ypos+(m_OriginY),0);
	GX_Color4u8(Red,Green,Blue,Alpha);     
	GX_TexCoord2f32(0, 1);         
	
	GX_End();

	GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);         
	GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);  
} 

// does not translate
void Image::DrawImage(u8 Alpha , u8 r, u8 g, u8 b )
{
	GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   

	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE); 
	GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);

	GX_Begin(GX_QUADS, GX_VTXFMT1,4);		// format: GX_S16#GX_RGBA8#GX_F32
 
	GX_Position3s16(-m_OriginX, -m_OriginY,0);	// GX_S16
	GX_Color4u8(r,g,b,Alpha);        
	GX_TexCoord2f32(0, 0);					// GX_F32

	GX_Position3s16(m_OriginX, -m_OriginY,0);
	GX_Color4u8(r,g,b,Alpha);            
	GX_TexCoord2f32(1, 0);           

	GX_Position3s16(m_OriginX, m_OriginY,0);         
	GX_Color4u8(r,g,b,Alpha);         
	GX_TexCoord2f32(1, 1);            

	GX_Position3s16(-m_OriginX, m_OriginY,0);
	GX_Color4u8(r,g,b,Alpha);                
	GX_TexCoord2f32(0, 1);         

	GX_End();

	GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);         
	GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);  
} 


//
//void Image::DrawImageNoAlphaFor3D(s32 xpos, s32 ypos )
//{
//	//GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR); //disable alpha 
//
//	GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   
//
//	GX_SetTevOp (GX_TEVSTAGE0, GX_REPLACE); 
//	GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);
//
//	GX_Begin(GX_QUADS, GX_VTXFMT2,4);			// format: GX_S16#GX_RGB8#GX_F32
// 
//	GX_Position3s16(xpos, ypos,0);				// GX_S16
//	//GX_Color3u8(0xFF,0xFF,0xFF);				// GX_RGB8
//	GX_Color1u16(0xFFFF);				// GX_RGB8
//	//GX_TexCoord2f32(0, 0);						// GX_F32
//	GX_TexCoord2u16(0, 0);
//
//	GX_Position3s16(xpos+(m_Width), ypos,0);
//	//GX_Color3u8(0xFF,0xFF,0xFF);      
//	GX_Color1u16(0xFFFF);
//	//GX_TexCoord2f32(1, 0);           
//	GX_TexCoord2u16(1, 0);
//
//	GX_Position3s16(xpos+(m_Width), ypos+(m_Height),0);         
//	//GX_Color3u8(0xFF,0xFF,0xFF);
//	GX_Color1u16(0xFFFF);
//	//GX_TexCoord2f32(1, 1);            
//	GX_TexCoord2u16(1, 1);
//
//	GX_Position3s16(xpos, ypos+(m_Height),0);
//	//GX_Color3u8(0xFF,0xFF,0xFF);         
//	GX_Color1u16(0xFFFF);
//	//GX_TexCoord2f32(0, 1);    
//	GX_TexCoord2u16(0, 1);
//	
//	GX_End();
//
////	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR); // enable alpha
//
//	GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);         
//	GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);  
//} 

//
//void Image::DrawImage(f32 xpos, f32 ypos, u8 Alpha )
//{    
//	GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   
//
//	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE); 
//	GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);
//
//	Mtx ViewMatrix;
//	//guMtxCopy(Singleton<WiiManager>::GetInstanceByRef().m_GXmodelView2D, ViewMatrix);	
//	guMtxIdentity(ViewMatrix); 
//
//	guMtxTrans(ViewMatrix, xpos + m_OriginX, ypos + m_OriginY, 0);	// origin at centre     
//	GX_LoadPosMtxImm (ViewMatrix, GX_PNMTX0);       
//
//	GX_Begin(GX_QUADS, GX_VTXFMT1,4); // format: GX_S16#GX_RGBA8#GX_F32
// 
//	GX_Position2s16(-m_OriginX, -m_OriginY);	// GX_S16
//	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);			// GX_RGBA8
//	GX_TexCoord2f32(0, 0);						// GX_F32
//
//	GX_Position2s16(m_OriginX, -m_OriginY);
//	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);         
//	GX_TexCoord2f32(1, 0);           
//
//	GX_Position2s16(m_OriginX, m_OriginY);         
//	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);
//	GX_TexCoord2f32(1, 1);            
//
//	GX_Position2s16(-m_OriginX, m_OriginY);
//	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);         
//	GX_TexCoord2f32(0, 1);         
//	
//	GX_End();
//
//	GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);      
//	GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);  
//} 

//
//void Image::DrawImageNoAlpha(f32 xpos, f32 ypos )
//{
//	//return;
//	////----------- disable alpha ---------
//	//   GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
//	//DrawImage(xpos,ypos);
//	////----------- enable alpha ---------
//	//GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
//
//	//return ;
//
//    GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);  //disable alpha
//
//	GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   //use texture
//
//	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE); 
//	GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);		// maybe move this to init stage... if its only to be used here
//
//	Mtx ViewMatrix; 
//	//guMtxCopy(Singleton<WiiManager>::GetInstanceByRef().m_GXmodelView2D, ViewMatrix);	
//	guMtxIdentity(ViewMatrix);  // reset
//	guMtxTrans(ViewMatrix, xpos + m_OriginX, ypos + m_OriginY, 0);	// origin at centre     
//	GX_LoadPosMtxImm (ViewMatrix, GX_PNMTX0);       
//
//	GX_Begin(GX_QUADS, GX_VTXFMT2,4); // format: GX_S16#GX_RGB8#GX_F32
// 
//	GX_Position2s16(-m_OriginX, -m_OriginY);	// GX_S16
//	GX_Color3u8(0xFF,0xFF,0xFF);			// GX_RGB8
//	GX_TexCoord2f32(0, 0);						// GX_F32
//
//
//	GX_Position2s16(m_OriginX, -m_OriginY);
//	GX_Color3u8(0xFF,0xFF,0xFF);         
//	GX_TexCoord2f32(1, 0);           
//
//	GX_Position2s16(m_OriginX, m_OriginY);         
//	GX_Color3u8(0xFF,0xFF,0xFF);
//	GX_TexCoord2f32(1, 1);            
//
//	GX_Position2s16(-m_OriginX, m_OriginY);
//	GX_Color3u8(0xFF,0xFF,0xFF);         
//	GX_TexCoord2f32(0, 1);         
//	
//	GX_End();
//
//	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);  //enable alpha
//} 


//I4 (4 bit intensity, 8x8 tiles) 
//I8 (8 bit intensity, 8x4 tiles) 
//IA4 (4 bit intensity with 4 bit alpha, 8x4 tiles) 
//IA8 (8 bit intensity with 8 bit alpha, 4x4 tiles) 
//RGB565 (4x4 tiles) 
//RGB5A3 (4x4 tiles) 
//RGBA8 (4x4 tiles in two cache lines - first is AR and second is GB) 
//CI4 (4 bit color index, 8x8 tiles) 
//CI8 (8 bit color index, 8x4 tiles) 
//CI14X2 (14 bit color index, 4x4 tiles) 
//CMP (S3TC compressed, 2x2 blocks of 4x4 tiles) 


// convertion to RGBA8 (4x4 tiles in two cache lines - first is AR and second is GB)
u32* Image::AllocateMemConvertToRGBA8(u8* src, u16 bufferWidth, u16 bufferHeight) 
{
	// changed the way size allocation works, should now convert any size of image safely.  
	u32 bufferSizeBytes = ( ((bufferWidth+3)/4*4) * ((bufferHeight+3)/4*4) ) * 4 ; //sizeof(GXColor) ;

	u32* dataBufferRGBA8 = (u32 *)memalign(32, bufferSizeBytes);
	memset(dataBufferRGBA8, 0, bufferSizeBytes);

	u8 *dst((u8*)dataBufferRGBA8);
	for (u16 block(0); block < bufferHeight; block += 4) 
	{  
		for (u16 i(0); i < bufferWidth; i += 4) 
		{  
			for (u16 c(0); c < 4; ++c) 
			{  
				for (u16 AlphaAndRed(0); AlphaAndRed < 4; ++AlphaAndRed) 
				{  
					*dst++ = src[(((i + AlphaAndRed) + ((block + c) * bufferWidth)) * 4) + 3];	//Alpha 
					*dst++ = src[((i + AlphaAndRed) + ((block + c) * bufferWidth)) * 4];		//red
				}  
			}  
			for (u16 c(0); c < 4 ; ++c) 
			{  
				for (u16 GreenAndBluePixels(0); GreenAndBluePixels < 4; ++GreenAndBluePixels) 
				{  
					*dst++ = src[(((i + GreenAndBluePixels) + ((block + c) * bufferWidth)) * 4) + 1];  //green
					*dst++ = src[(((i + GreenAndBluePixels) + ((block + c) * bufferWidth)) * 4) + 2];  //blue
				}  
			}  
		}
	} 

	DCFlushRange(dataBufferRGBA8, bufferSizeBytes);
	return dataBufferRGBA8;
}


void Image::Refresh()
{
	u32 bufferSizeBytes = ( ((m_Width+3)/4*4) * ((m_Height+3)/4*4) ) * 4 ; //sizeof(GXColor) ;


	DCFlushRange(m_ImageData,bufferSizeBytes);
	memset(&m_HardwareTextureInfo,0,sizeof(m_HardwareTextureInfo));
	GX_InitTexObj(&m_HardwareTextureInfo, m_ImageData, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);
	GX_Flush();

	//////////// test hack - debug only, eat's memory
	//////////u32* dataBufferRGBA8 = (u32 *)memalign(32, bufferSizeBytes);
	//////////memcpy(dataBufferRGBA8,m_ImageData,bufferSizeBytes);
	//////////DCFlushRange(dataBufferRGBA8,bufferSizeBytes);
	//////////GX_InitTexObj(&m_HardwareTextureInfo, dataBufferRGBA8, m_Width,m_Height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);
	//////////GX_Flush();
	//////////m_ImageData  = dataBufferRGBA8;
}


//void Image::CopyImage(Image* pImage,u32 x, u32 y,u32 uWidth, u32 uHeight)
//{
//	//temp ...cut needed
//	memcpy(GetImageData(), pImage->GetImageData(), GetWidth()*GetHeight()*4);
//};





