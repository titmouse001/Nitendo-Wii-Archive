//
// Image Support - non transparent images
//

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "WiiManager.h"
#include "Image.h"
#include "debug.h"
#include "ImageManager.h"
#include "FontManager.h"
#include "tga.h"
void ImageManager::Line(s16 x1, s16 y1, s16 x2, s16 y2, u32 RGBAStart, u32 RGBAEnd  )
{
	GX_SetLineWidth(10,GX_TO_ONE);

	GX_Begin(GX_LINES, GX_VTXFMT0, 2);
	GX_Position3s16(x1,y1,0);
	GX_Color1u32(RGBAStart);
	GX_Position3s16(x2,y2,0);
	GX_Color1u32(RGBAEnd);
	GX_End();
}
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
	BeginGraphicsFile(FullName.c_str());

	AddImage(StartX, StartY,CutSizeWidth, CutSizeHeight, uTotal );

	EndGraphicsFile();
};

void ImageManager::AddImage( u32 StartX, u32 StartY, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal )
{
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
				return;	
		}
	}
};

bool ImageManager::BeginGraphicsFile(string pFullFileName)
{
	m_pTgaData = Tga::LoadTGA(pFullFileName.c_str(), m_TgaHeader) ; 
	return (m_pTgaData!=NULL);
}

void ImageManager::EndGraphicsFile()
{
	if (m_pTgaData != NULL)
	{
		Tga::UnLoadTGA( m_pTgaData );
	}
}


//
//// Cut this into 2 parts , remove all file logic... just return list things to load
//void ImageManager::ReadDirForDataFiles(string Path, string LookFor)
//{
//	DIR* pdir(opendir( Path.c_str() ));
//	if (!pdir)
//	{
//		printf ("error: opendir\n");
//		exit(0);
//	}
//
//	struct dirent *pent;
//	struct stat statbuf;
//
//	while ((pent=readdir(pdir))!=NULL) 
//	{
//	    stat(pent->d_name,&statbuf);
//	    if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)
//	        continue;
//
//	    if(!(S_ISDIR(statbuf.st_mode)))
//		{
//			int n = strlen(pent->d_name);
//			if (n>4)
//			{
//				char* str = (pent->d_name + n) - 4 ;
//				if (stricmp( str , LookFor.c_str() ) == 0)
//				{
//					std::string FullName(pent->d_name);
//					FullName = Path + FullName;
//
//					AddImage(FullName);
//				}
//			}
//		}
//	}
//	closedir(pdir);
//}

//std::vector<Image*>::iterator ImageManager::ScanImagesFromSDCard()
//{
//	ReadDirForDataFiles("sd:/" , ".tga");
//
//	if (m_ImageContainer.empty())
//	{
//		AddImage(32,32); 
//	}
//	
//	return GetImageDataBegin();
//}


//////////////#include <gccore.h>
//////////////#include <stdio.h>
//////////////#include <stdlib.h>
//////////////#include <string.h>
//////////////#include <dirent.h>
//////////////#include "WiiManager.h"
//////////////#include "Image.h"
//////////////#include "debug.h"
//////////////#include "ImageManager.h"
//////////////#include "FontManager.h"
//////////////#include "tga.h"
//////////////
//////////////ImageManager::~ImageManager()
//////////////{
//////////////	RemoveImages();
//////////////}
//////////////
//////////////void ImageManager::RemoveImages()
//////////////{
//////////////	for (vector<Image*>::iterator Iter(GetImageDataBegin()); Iter!=GetImageDataEnd(); /* NOP */)
//////////////	{
//////////////		if ( (*Iter) != NULL )
//////////////		{
//////////////			// cleanup & remove from list
//////////////			(*Iter)->Remove();
//////////////			Iter = m_ImageContainer.erase( Iter );
//////////////		}
//////////////		else
//////////////		{
//////////////			++Iter;
//////////////		}
//////////////	}
//////////////}
//////////////
//////////////void ImageManager::RefreshImages()
//////////////{
//////////////	for (vector<Image*>::iterator Iter(GetImageDataBegin()); Iter!=GetImageDataEnd(); ++Iter)
//////////////	{
//////////////		if ( (*Iter) != NULL )
//////////////		{
//////////////			(*Iter)->Refresh();
//////////////		}
//////////////	}
//////////////}
//////////////
//////////////Image* ImageManager::GetImage(u32 ImageID)
//////////////{
//////////////	if ( ImageID < m_ImageContainer.size())
//////////////		return  m_ImageContainer[ImageID] ;
//////////////	else
//////////////		return  m_ImageContainer[0] ;
//////////////}
//////////////
//////////////std::vector<Image*>::iterator ImageManager::ScanImagesFromSDCard()
//////////////{
//////////////	ReadDirForDataFiles("sd:/" , ".tga");
//////////////
//////////////	if (m_ImageContainer.empty())
//////////////	{
//////////////		StoreImage(32,32); 
//////////////	}
//////////////	
//////////////	return GetImageDataBegin();
//////////////}
//////////////
//////////////void ImageManager::DrawLineStrip(std::vector<Vtx> Container, u32 RGBAStart)
//////////////{
//////////////	//if (Container.size()<3)
//////////////	//	return;
//////////////
//////////////	//GX_Begin(GX_TRIANGLESTRIP, GX_VTXFMT0, 2*Container.size());
//////////////
//////////////	//bool bToggle(true);
//////////////	//for (std::vector<Vtx>::iterator iter(Container.begin()); iter!=Container.end(); iter++)
//////////////	//{
//////////////	//	GX_Position3s16(iter->x,iter->y-5, -240 + (iter->z)*70.0f ); 
//////////////	//	GX_Color1u32(RGBAStart);
//////////////
//////////////	//	GX_Position3s16(iter->x,iter->y+5, -240 + (iter->z)*70.0f);
//////////////	//	GX_Color1u32(RGBAStart);
//////////////
//////////////	//	bToggle = !bToggle;
//////////////	//		alpha += 2.005;
//////////////	//	RGBAStart += 0x03050402;
//////////////	//}
//////////////	//GX_End();
//////////////}
////////////////
////////////void ImageManager::Line(s16 x1, s16 y1, s16 x2, s16 y2, u32 RGBAStart, u32 RGBAEnd  )
////////////{
////////////	GX_SetLineWidth(10,GX_TO_ONE);
////////////
////////////	GX_Begin(GX_LINES, GX_VTXFMT0, 2);
////////////	GX_Position3s16(x1,y1,0);
////////////	GX_Color1u32(RGBAStart);
////////////	GX_Position3s16(x2,y2,0);
////////////	GX_Color1u32(RGBAEnd);
////////////	GX_End();
////////////}
////////////////
//////////////
//////////////
//////////////// Cut this into 2 parts , remove all file logic... just return list things to load
//////////////void ImageManager::ReadDirForDataFiles(string Path, string LookFor)
//////////////{
//////////////	DIR* pdir(opendir( Path.c_str() ));
//////////////	if (!pdir)
//////////////	{
//////////////		printf ("error: opendir\n");
//////////////		exit(0);
//////////////	}
//////////////
//////////////	struct dirent *pent;
//////////////	struct stat statbuf;
//////////////
//////////////	while ((pent=readdir(pdir))!=NULL) 
//////////////	{
//////////////	    stat(pent->d_name,&statbuf);
//////////////	    if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)
//////////////	        continue;
//////////////
//////////////	    if(!(S_ISDIR(statbuf.st_mode)))
//////////////		{
//////////////			int n = strlen(pent->d_name);
//////////////			if (n>4)
//////////////			{
//////////////				char* str = (pent->d_name + n) - 4 ;
//////////////				if (stricmp( str , LookFor.c_str() ) == 0)
//////////////				{
//////////////					std::string FullName(pent->d_name);
//////////////					FullName = Path + FullName;
//////////////
//////////////					StoreImage(FullName);
//////////////				}
//////////////			}
//////////////		}
//////////////	}
//////////////	closedir(pdir);
//////////////}
//////////////
//////////////void ImageManager::CutImageIntoParts(string FullName, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal )
//////////////{
//////////////	Tga::TGA_HEADER TgaHeader;
//////////////	u8* TgaData(Tga::LoadTGA(FullName.c_str(),TgaHeader)); 
//////////////
//////////////// just a missing data thing.... this is a temp work arround for missing alpha data in texuters
////////////////	Tga::ApplyAlphaToTGAFromPink(TgaHeader, (Tga::PIXEL*) TgaData);
//////////////
//////////////
//////////////	if (CutSizeWidth==0)
//////////////	{
//////////////		CutSizeWidth = TgaHeader.width;
//////////////	}
//////////////
//////////////	u32 uCount(uTotal);
//////////////	if (uTotal==0)
//////////////	{
//////////////		uCount = (TgaHeader.width / CutSizeWidth) * (TgaHeader.height / CutSizeHeight);
//////////////	}
//////////////
//////////////	for (u32 y(0); y<(u32)TgaHeader.height ; y+=CutSizeHeight)
//////////////	{
//////////////		for (u32 x(0); x<(u32)TgaHeader.width; x+=CutSizeWidth )
//////////////		{
//////////////			if (uCount==0)
//////////////				break;	
//////////////
//////////////			u8* Data( Tga::GetDataFromTGA(TgaHeader, TgaData, x, y, CutSizeWidth, CutSizeHeight) );
//////////////
//////////////			StoreImage(Data,CutSizeWidth,CutSizeHeight);
//////////////			delete [] Data;  
//////////////
//////////////			--uCount;
//////////////		}
//////////////	}
//////////////
//////////////	Tga::UnLoadTGA(TgaData);
//////////////};
//////////////
//////////////void ImageManager::StoreImage(Image* pImage)
//////////////{
//////////////	m_ImageContainer.push_back( pImage );
//////////////};
//////////////
//////////////void ImageManager::StoreImage(u8* pTgaData, u32 uWidth, u32 uHeight)
//////////////{
//////////////	Image* pImage = new Image( pTgaData, uWidth, uHeight );
//////////////	m_ImageContainer.push_back( pImage );
//////////////
////////////////		return pImage;
//////////////};
//////////////
//////////////void ImageManager::StoreImage(string FullName)
//////////////{
//////////////	m_ImageContainer.push_back( new Image( FullName.c_str() ) );
//////////////};
//////////////
//////////////void ImageManager::StoreImage(u32 uWidth, u32 uHeight)
//////////////{
//////////////	Image* pImage = new Image( uWidth, uHeight );
//////////////	m_ImageContainer.push_back( pImage );
//////////////
////////////////	return pImage;
//////////////};
//////////////
//////////////std::vector<Image*>::iterator ImageManager::GetImageDataBegin()
//////////////{
//////////////	return m_ImageContainer.begin();
//////////////};
//////////////
//////////////std::vector<Image*>::iterator ImageManager::GetImageDataEnd()
//////////////{
//////////////	return m_ImageContainer.end();
//////////////};
//////////////
//////////////int ImageManager::GetImageCount() const
//////////////{
//////////////	return m_ImageContainer.size();
//////////////};
////////////////
////////////////void ImageManager::DrawFromImageData(u32* pImageData, f32 xpos, f32 ypos, u8 Alpha )
////////////////{
////////////////	u16 width  = 16*3;
////////////////	u16 height = 16*3;
////////////////	s16 OriginX = width/2;
////////////////	s16 OriginY = height/2;
////////////////	GXTexObj	TempHardwareTextureInfo;
////////////////	GX_InitTexObj(&TempHardwareTextureInfo, pImageData, width, height, GX_TF_RGBA8,GX_CLAMP, GX_CLAMP,GX_FALSE);  
////////////////	GX_LoadTexObj(&TempHardwareTextureInfo, GX_TEXMAP0);   
////////////////	GX_SetTevOp (GX_TEVSTAGE0, GX_REPLACE); // GX_MODULATE);
////////////////	GX_SetVtxDesc (GX_VA_TEX0,GX_DIRECT);
////////////////	
////////////////	Mtx TransMatrix, FinalMatrix; 
////////////////	guMtxIdentity(TransMatrix);   
////////////////
////////////////	guMtxCopy(Singleton<WiiManager>::GetInstanceByRef().m_GXmodelView2D, FinalMatrix);	
////////////////
////////////////	guMtxTransApply(TransMatrix,FinalMatrix, xpos + OriginX, ypos + OriginY, 0);	// origin at centre     
////////////////
////////////////	GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0);       
////////////////
////////////////	GX_Begin(GX_QUADS, GX_VTXFMT1,4); // format: G X_S16#GX_RGBA8#GX_F32
//////////////// 
////////////////	GX_Position2s16(-OriginX, -OriginY);	// GX_S16
////////////////	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);			// GX_RGBA8
////////////////	GX_TexCoord2f32(0, 0);						// GX_F32
////////////////
////////////////	GX_Position2s16(OriginX, -OriginY);
////////////////	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);         
////////////////	GX_TexCoord2f32(1, 0);           
////////////////
////////////////	GX_Position2s16(OriginX, OriginY);         
////////////////	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);
////////////////	GX_TexCoord2f32(1, 1);            
////////////////
////////////////	GX_Position2s16(-OriginX, OriginY);
////////////////	GX_Color4u8(0xFF,0xFF,0xFF,Alpha);         
////////////////	GX_TexCoord2f32(0, 1);         
////////////////	
////////////////	GX_End();
////////////////
////////////////
////////////////	// todo don't think this func should be responsible for reseting ... i.e calling 'GX_LoadPosMtxImm' 
//////////////////	GX_LoadPosMtxImm(WiiManager::m_GXmodelView2D, GX_PNMTX0);
////////////////
////////////////
////////////////
/////////////////////////////	GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);         
/////////////////////////////	GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);  
////////////////
////////////////
////////////////} 
////////////////  
//////////////
