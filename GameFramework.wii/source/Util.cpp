#include "Util.h"


void Util::StringToLower(std::string& StringValue)
{
	for(int i=0; i<(int)StringValue.size();++i)
		StringValue[i] = tolower(StringValue[i]);
}

GXColor Util::Colour(u8 r,u8 g,u8 b,u8 a)
{
	GXColor Colour;
	Colour.a=a;
	Colour.b=b;
	Colour.g=g;
	Colour.r=r;

	return Colour;
}
