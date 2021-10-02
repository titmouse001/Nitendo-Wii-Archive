#ifndef SpriteManager_H_
#define SpriteManager_H_

#include "GCTypes.h"
#include "Image.h"
#include <string>
#include "imageManager.h"

class SpriteManager : public ImageManager
{
public:

//next to fix naming i.e was using storeimage without nowing it, this child class was not used!
	virtual void AddImage( u32 StartX, u32 StartY, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal );

	virtual void AddImage(string FullName, u32 StartX, u32 StartY, u32 CutSizeWidth, u32 CutSizeHeight, u32 uTotal );

private:

};

#endif
