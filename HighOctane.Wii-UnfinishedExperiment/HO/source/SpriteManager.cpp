//
// Sprite Support - transparent images, inherits functionality from 'ImageManager'
//

#include "ImageManager.h"
#include "SpriteManager.h"
#include "Image.h"
#include "Tga.h"
#include "Debug.h"

// function supports pink to alpha
void SpriteManager::AddImage(u32 StartX, u32 StartY, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal )
{
	Tga::ApplyAlphaToTGAFromPink( GetTgaHeader(),  GetTgaData());

	ImageManager::AddImage(StartX, StartY, CutSizeWidth, CutSizeHeight , uTotal);
};

// function supports pink to alpha
void SpriteManager::AddImage(string FullName, u32 StartX, u32 StartY, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal )
{
	BeginGraphicsFile(FullName.c_str());
	
	Tga::ApplyAlphaToTGAFromPink( GetTgaHeader(),  GetTgaData());

	ImageManager::AddImage(StartX, StartY,CutSizeWidth, CutSizeHeight, uTotal );

	EndGraphicsFile();
}