#ifndef _UTIL_H
#define _UTIL_H

//#include "GCTypes.h"
#include "ogc/gx.h"

#include <string>

namespace Util
{
	void StringToLower(std::string& StringValue);

	GXColor Colour(u8 r,u8 g,u8 b,u8 a=0xff);

}

#endif