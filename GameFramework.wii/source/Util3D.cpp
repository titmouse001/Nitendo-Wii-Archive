#include "Util3D.h"
#include "Singleton.h"
#include "WiiManager.h"
#include <gccore.h>


void Util3D::Trans(f32 xpos, f32 ypos)
{
	Mtx Matrix;
	guMtxIdentity(Matrix);
	guMtxTrans(Matrix,xpos, ypos, 0 );	// Position
	guMtxConcat(Singleton<WiiManager>::GetInstanceByRef().GetCamera()->GetcameraMatrix(),Matrix,Matrix);
	GX_LoadPosMtxImm (Matrix, GX_PNMTX0); 
}


void Util3D::Trans(f32 xpos, f32 ypos, f32 zpos)
{
	Mtx Matrix;
	guMtxIdentity(Matrix);
	guMtxTrans(Matrix,xpos, ypos, zpos );	// Position
	guMtxConcat(Singleton<WiiManager>::GetInstanceByRef().GetCamera()->GetcameraMatrix(),Matrix,Matrix);
	GX_LoadPosMtxImm (Matrix, GX_PNMTX0); 
}

void Util3D::TransRot(f32 xpos, f32 ypos, f32 z , f32 rad)
{
	Mtx FinalMatrix,TransMatrix;
	guMtxRotRad(TransMatrix,'Z',rad);  // Rotage
	guMtxTransApply(TransMatrix,TransMatrix,xpos, ypos, z );	// Position
	guMtxConcat(Singleton<WiiManager>::GetInstanceByRef().GetCamera()->GetcameraMatrix(),TransMatrix,FinalMatrix);
	GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0); 
}

void Util3D::TransRot(f32 xpos, f32 ypos, f32 rad)
{
	Mtx FinalMatrix,TransMatrix;
	guMtxRotRad(TransMatrix,'Z',rad);  // Rotage
	guMtxTransApply(TransMatrix,TransMatrix,xpos, ypos, 0.0f );	// Position
	guMtxConcat(Singleton<WiiManager>::GetInstanceByRef().GetCamera()->GetcameraMatrix(),TransMatrix,FinalMatrix);
	GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0); 
}

void Util3D::Identity()
{
	GX_LoadPosMtxImm (Singleton<WiiManager>::GetInstanceByRef().GetCamera()->GetcameraMatrix(), GX_PNMTX0); 
}