#ifndef ImageManager_H_
#define ImageManager_H_

#include "GCTypes.h"
#include "ogc/gx.h"
#include "Tga.h"
#include <string>
#include <vector>

using namespace std;

class Image;


class ImageManager
{
public:

	ImageManager() : m_pTgaData(NULL) { ; }
	~ImageManager();

	Image* GetImage(u32 ImageID);
	void AddImage(string FullName);
	void AddImage(u32 uWidth, u32 uHeight);
	void AddImage(Image* pImage);
	virtual void AddImage(u8* pTgaData, u32 Width , u32 Height);
	
	void AddImage(string FullName, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal = 0);
	void AddImage(string FullName, u32 StartX, u32 StartY, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal = 0 );
	void AddImage(u32 StartX, u32 StartY, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal = 0 );

	std::vector<Image*>::iterator GetImageDataBegin() { return m_ImageContainer.begin(); }
	std::vector<Image*>::iterator GetImageDataEnd() { return m_ImageContainer.end(); }
	int GetImageCount() const { return m_ImageContainer.size(); }

	void RemoveImages();
	void RefreshImages();

	bool BeginGraphicsFile(string pFullFileName);
	void EndGraphicsFile();

	Tga::TGA_HEADER	GetTgaHeader()  { return m_TgaHeader; };
	Tga::PIXEL* 	GetTgaData() const { return (Tga::PIXEL*)m_pTgaData; };

	void Line(s16 x1, s16 y1, s16 x2, s16 y2, u32 RGBAStart, u32 RGBAEnd  );

private:

	vector<Image*> m_ImageContainer;

	Tga::TGA_HEADER m_TgaHeader;
	u8*				m_pTgaData;
};

#endif


////////#ifndef ImageManager_H_
////////#define ImageManager_H_
////////
////////#include "GCTypes.h"
////////#include "ogc/gx.h"
////////#include <string>
////////#include <vector>
////////
////////using namespace std;
////////
////////class Image;
////////
////////class ImageManager
////////{
////////public:
////////
////////	~ImageManager();
////////
////////	std::vector<Image*>::iterator ScanImagesFromSDCard();
////////
////////	Image* GetImage(u32 ImageID);
////////
////////	void DrawLineStrip(std::vector<Vtx> Container, u32 RGBAStart);
////////	void ReadDirForDataFiles(string Path, string LookFor);
////////
////////	void StoreImage(string FullName);
////////	void StoreImage(u32 uWidth, u32 uHeight);
////////	void StoreImage(Image* pImage);
////////	virtual void StoreImage(u8* pTgaData, u32 Width , u32 Height);
////////	
////////	void CutImageIntoParts(string FullName, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal = 0);
////////
////////	vector<Image*>::iterator GetImageDataBegin();
////////	vector<Image*>::iterator GetImageDataEnd();
////////
////////	//void Line(s16 x1, s16 y1, s16 x2, s16 y2, u32 RGBAStart, u32 RGBAEnd  );
////////
////////	//void DrawFromImageData(u32* pImageData, f32 xpos, f32 ypos, u8 Alpha );
////////	int GetImageCount() const;
////////
////////	void RemoveImages();
////////	void RemoveImages2();
////////	void RefreshImages();
////////
////////	//Image* StoreImage(u8* pTgaData, u32 x, u32 y,u32 uWidth, u32 uHeight)
////////	//{
////////	//	Image* pImage = new Image(pTgaData,x,y,uWidth,uHeight);
////////	//	m_ImageContainer.push_back( pImage );
////////	//	return pImage;
////////	//};
////////
////////private:
////////
////////	vector<Image*> m_ImageContainer;
////////};
////////
////////#endif
