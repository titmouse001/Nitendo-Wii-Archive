// Colour converting stuff

#include "Util.h"
#include "WiiManager.h"

#include <string>
#include <sstream>
#include <iomanip>


#include "libpng/pngu/pngu.h"

static int Value(0);

void Util::StringToLower(string& StringValue)
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


void Util::Translate(float x, float y, float Scale, float Rot)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 

	Mtx FinalMatrix, TransMatrix; 
	//guMtxIdentity(TransMatrix);
	guMtxRotRad(TransMatrix,'Z',Rot); 
	guMtxScaleApply(TransMatrix,TransMatrix,Scale,Scale,0);
	guMtxTransApply(TransMatrix,TransMatrix,x,y,0);	// origin
	guMtxConcat(Wii.GetCamera()->GetcameraMatrix(),TransMatrix,FinalMatrix);
	GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0);  
}


void Util::TakeIncrementalScreenshot()
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 

	IMGCTX ctx;
	//std::string FileName = "FMag";
	//std::ostringstream stream;
	//stream << setfill('0');
	//stream << setw(4) << Value;
	//FileName += stream.str();
	//++Value;
	//std::string FullFileName = "sd:/" + FileName + ".tga";


	std::string FullFileName = GetIncrementalScreenshotFileName(false);

	if ( !(ctx = PNGU_SelectImageFromDevice (FullFileName.c_str())) )
	{
		ExitPrintf ("TakeIncrementalScreenshot device failed");
	}
	else
	{
		if (PNGU_EncodeFromYCbYCr (ctx, Wii.GetScreenWidth(), Wii.GetScreenHeight(), Wii.GetCurrentFrame() /* xfb */, 0) != PNGU_OK)
			ExitPrintf ("TakeIncrementalScreenshot Encode failed");
	}
}


std::string Util::GetIncrementalScreenshotFileName(bool DontIncrement)
{
	if (!DontIncrement)
		++Value;

	std::string FileName = "FMag";
	std::ostringstream stream;
	stream << setfill('0');
	stream << setw(4) << Value;
	FileName += stream.str();


	//time_t TimeEnd;
	//time ( &TimeEnd );

	//stream << asctime(localtime(&TimeEnd));
	//FileName += stream.str();


	std::string FullFileName = "sd:/" + FileName + ".png";

	return FullFileName;
}