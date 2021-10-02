#ifndef _UTIL_H
#define _UTIL_H

#include "GCTypes.h"
#include "ogc/gx.h"

#include <string>

namespace Util
{
	GXColor Colour(u8 r,u8 g,u8 b,u8 a=0xff);
	void	Translate(float x, float y, float Scale=1.0f, float rot=0.0f);
	void	TakeIncrementalScreenshot();
	std::string GetIncrementalScreenshotFileName(bool DontIncrement=false);

	void StringToLower(std::string& StringValue);
}


#endif