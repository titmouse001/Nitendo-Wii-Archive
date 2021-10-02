#ifndef _UTIL_H
#define _UTIL_H

#include "GCTypes.h"
#include "ogc/gx.h"

#include <string>

namespace Util
{
	void DoResetSystemCheck();
	void SetUpPowerButtonTrigger();

	void StringToLower(std::string& StringValue);

	GXColor Colour(u8 r,u8 g,u8 b,u8 a=0xff);

	u64 TicksToMicrosecs(u64 Value);

	////bool InsideRadius(float center_x, float center_y, float radius, float x, float y);
	u8 CalculateFrameRate(bool bReadOnly  = false);

	void	SleepForMilisec(unsigned long milisec); //TEMP

	bool IsPowerOff();

	std::string NumberToString(int Number);
	std::string NumberToString(int Number, int DigitWidth);


	u16 ENDIAN16(u16 Value);
	u32 ENDIAN32(u32 Value);

	u64 timer_gettime();

	void Replace(std::string& str,const std::string& from,const std::string& to);

	std::string urlDecode(std::string Text);
}


#endif