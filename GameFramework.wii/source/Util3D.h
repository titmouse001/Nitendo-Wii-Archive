#ifndef Util3D_H
#define Util3D_H

#include "GCTypes.h"
#include <gccore.h>


namespace Util3D
{
	void Trans(f32 xpos, f32 ypos);
	void Trans(f32 xpos, f32 ypos,f32 zpos);
	void TransRot(f32 xpos, f32 ypos, f32 z , f32 rad);
	void TransRot(f32 xpos, f32 ypos, f32 rad);
	void Identity();  // as camera
}


#endif