#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "WiiManager.h"
#include "Image.h"
#include "debug.h"
#include "util.h"
#include "ImageManager.h"
#include "FontManager.h"
#include "tga.h"
#include "libpng/pngu/pngu.h"


// THIS CODE IS AHEAD of the BoldThrower code.

ImageManager::~ImageManager()
{
	RemoveImages();
}

void ImageManager::RemoveImages()
{
	for (vector<Image*>::iterator Iter(GetImageDataBegin()); Iter!=GetImageDataEnd(); /* NOP */)
	{
		if ( (*Iter) != NULL )
		{
			(*Iter)->Remove(); // cleanup & remove from list
			Iter = m_ImageContainer.erase( Iter );
		}
		else
		{
			++Iter;
		}
	}
}

void ImageManager::RefreshImages()
{
	for (vector<Image*>::iterator Iter(GetImageDataBegin()); Iter!=GetImageDataEnd(); ++Iter)
	{
		if ( (*Iter) != NULL )
		{
			(*Iter)->Refresh();
		}
	}
}

Image* ImageManager::GetImage(u32 ImageID)
{
	if ( ImageID < m_ImageContainer.size())
		return  m_ImageContainer[ImageID] ;
	else
		return  m_ImageContainer[0] ;
}

void ImageManager::AddImage(string FullName)
{
	m_ImageContainer.push_back( new Image( FullName.c_str() ) );
};

void ImageManager::AddImage(Image* pImage)
{
	m_ImageContainer.push_back( pImage );
};

void ImageManager::AddImage(u32 uWidth, u32 uHeight)
{
	m_ImageContainer.push_back( new Image( uWidth, uHeight ) );
};

void ImageManager::AddImage( u8* pTgaData, u32 uWidth, u32 uHeight)
{
	m_ImageContainer.push_back( new Image( pTgaData, uWidth, uHeight ) );
};

void ImageManager::AddImage(string FullName, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal )
{
	ImageManager::AddImage(FullName, 0,0, CutSizeWidth, CutSizeHeight, uTotal );
}

void ImageManager::AddImage(string FullName, u32 StartX, u32 StartY, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal )
{
//	Tga::TGA_HEADER TgaHeader;
//	u8* pTgaData(Tga::LoadTGA(FullName.c_str(),TgaHeader)); 

	BeginGraphicsFile(FullName.c_str());

	AddImage(/*pTgaData, TgaHeader,*/ StartX, StartY,CutSizeWidth, CutSizeHeight, uTotal );

	EndGraphicsFile();

//	Tga::UnLoadTGA(pTgaData);
};

int ImageManager::AddImage( /* u8* pTgaData,  Tga::TGA_HEADER& TgaHeader,*/ u32 StartX, u32 StartY, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal )
{
	// just a missing data thing.... this is a temp work arround for missing alpha data in texuters
	//	Tga::ApplyAlphaToTGAFromPink(TgaHeader, (Tga::PIXEL*) TgaData);

	int StoreCount = GetImageCount();

	if (CutSizeWidth==0)
	{
		CutSizeWidth = m_TgaHeader.width;
	}

	int uCount(uTotal);
	if (uTotal==0)
	{
		uCount = (m_TgaHeader.width / CutSizeWidth) * (m_TgaHeader.height / CutSizeHeight);
	}

	for (u32 y(StartY); y<(u32)m_TgaHeader.height; y+=CutSizeHeight)
	{
		for (u32 x(StartX); x<(u32)m_TgaHeader.width; x+=CutSizeWidth )
		{
			u8* Data( Tga::CreateDataFromTGA(m_TgaHeader, m_pTgaData, x, y, CutSizeWidth, CutSizeHeight) );
			AddImage(Data,CutSizeWidth,CutSizeHeight);
			delete [] Data;  
		
			--uCount;

			if (uCount<=0)
				return StoreCount;	

		}
	}

	return StoreCount;
};


bool ImageManager::BeginGraphicsFile(string FullFileName)
{
	Util::StringToLower(FullFileName);

	if (FullFileName.substr(FullFileName.find_last_of(".") + 1) == "png")
	{		
		PNGUPROP imgProp;
		IMGCTX ctx;
		ctx = PNGU_SelectImageFromDevice (FullFileName.c_str()); 
		PNGU_GetImageProperties (ctx, &imgProp);
		u32 bufferSizeBytes = ( ((imgProp.imgWidth+3)/4*4) * ((imgProp.imgHeight+3)/4*4) ) * 4 ; 

		m_pTgaData = new u8 [bufferSizeBytes]; 

		m_TgaHeader.height = imgProp.imgHeight;
		m_TgaHeader.width = imgProp.imgWidth;

		PNGU_DecodeToRGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, m_pTgaData,0, 255);
		PNGU_ReleaseImageContext (ctx);
	}
	else if (FullFileName.substr(FullFileName.find_last_of(".") + 1) == "tga")
	{
		m_pTgaData = Tga::LoadTGA(FullFileName.c_str(), m_TgaHeader) ; 
	}
	else
	{
		ExitPrintf("Unkown file type for '%s'", FullFileName.c_str() );
	}

	return (m_pTgaData!=NULL);
}

void ImageManager::EndGraphicsFile()
{
	if (m_pTgaData != NULL)
	{
		Tga::UnLoadTGA( m_pTgaData );
	}
}

void ImageManager::Fade(u8 Alpha)
{	
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 

//	Mtx FinalMatrix, TransMatrix; 
//	guMtxIdentity(TransMatrix); 
//	guMtxRotRad(TransMatrix,'Z',pFridgeMagnet->GetAngle());  // Rotage
//	guMtxTransApply(TransMatrix,TransMatrix,pFridgeMagnet->GetX(), pFridgeMagnet->GetY(),pFridgeMagnet->GetZ());	// Position
//	guMtxConcat(Wii.GetCamera()->GetcameraMatrix(),TransMatrix,FinalMatrix);
	GX_LoadPosMtxImm (Wii.GetCamera()->GetcameraMatrix(), GX_PNMTX0);  


	//GX_LoadTexObj(&m_HardwareTextureInfo, GX_TEXMAP0);   

	GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR); 
	//GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);

	GX_Begin(GX_QUADS, GX_VTXFMT3,4);		// format: GX_S16#GX_RGBA8#GX_F32

	float Zoom = Wii.GetCamera()->GetCameraFactor();
	GX_Position3s16(-Wii.GetScreenWidth()*Zoom, -Wii.GetScreenHeight()*Zoom, 0);			// GX_S16
	GX_Color4u8(0,0,0,Alpha);     
	GX_Position3s16(Wii.GetScreenWidth()*Zoom, -Wii.GetScreenHeight() *Zoom,0);
	GX_Color4u8(0,0,0,Alpha);     
	GX_Position3s16(Wii.GetScreenWidth()*Zoom, Wii.GetScreenHeight()*Zoom,0);         
	GX_Color4u8(0,0,0,Alpha);     
	GX_Position3s16(-Wii.GetScreenWidth()*Zoom,Wii.GetScreenHeight()*Zoom, 0);
	GX_Color4u8(0,0,0,Alpha);     

     
	GX_End();

//..	GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);         
//..	GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);  
};
