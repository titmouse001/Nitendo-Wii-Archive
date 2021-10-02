#ifndef Util3D_H
#define Util3D_H

#include "GCTypes.h"
#include <gccore.h>

#include <math.h>

namespace Util3D
{
	void Trans(f32 xpos, f32 ypos);
	void Trans(f32 xpos, f32 ypos,f32 zpos);
	void TransRot(f32 xpos, f32 ypos, f32 z , f32 rad);
	void TransRot(f32 xpos, f32 ypos, f32 rad);
	void TransScale(f32 xpos, f32 ypos, f32 zpos , f32 scale);
	void CameraIdentity();  // as camera

	void MatrixRotateZ(Mtx mt,f32 rad);
	void MatrixRotateY(Mtx mt,f32 rad);
	void MatrixRotateX(Mtx mt,f32 rad);

	//void MatrixConcat(Mtx a,Mtx b,Mtx ab); // unsafe - no var checking
	//void TransApply(Mtx m,f32 x,f32 y,f32 z);  // unsafe
	//
	//void ScaleApply(Mtx m,f32 xS,f32 yS,f32 zS);
}

#endif