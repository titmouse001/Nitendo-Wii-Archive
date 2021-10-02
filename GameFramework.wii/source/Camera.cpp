#include "Camera.h"
#include "WiiManager.h"
#include "debug.h"
#include <math.h>


void Camera::InitialiseCamera(float ZoomFactor)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );  // backbone stuff
	static const guVector UP = {0.0F, -1.0F, 0.0F}; 
	m_up = UP; 

	//f32 w = Wii.GetScreenWidth();
	f32 h = Wii.GetScreenHeight();
	float Fov = 45.0f;
	float triangle = 90.0f - (Fov * 0.5f); 
	float rads = triangle * (M_PI/180.0f);
	float CameraHeight = tan(rads) * (h * 0.5); 

	//printf("%f",CameraHeight);
	SetCameraView( (Wii.GetScreenWidth()/2), (Wii.GetScreenHeight()/2), -(CameraHeight));
}

void Camera::SetCameraView()
{
	m_look	= m_camera; 
	m_look.z = 0.0f;
	guLookAt(m_cameraMatrix, &m_camera,	&m_up, &m_look);
}

void Camera::SetCameraView(float x, float y)  
{ 
	m_camera.x = x; 
	m_camera.y = y;  
	SetCameraView();
}

void Camera::SetCameraView(float x, float y, float z)  
{ 
	m_camera.x = x; 
	m_camera.y = y; 
	m_camera.z = z; 
	SetCameraView();
}

void Camera::SetLightOn2()
{
    static const GXColor LightColour  = { 0xff, 0xff, 0xff, 0xFF };
	static const GXColor AmbientColour  = { 0x0f, 0x0f, 0x0f, 0xFF };
    static const GXColor MaterialColour = { 0xff, 0xff, 0xff, 0xFF };

	guVector light_pos,light_look;
	guVector look, pos, light_dir;
	GXLightObj LightObject;

	light_pos.x =  1000;
	light_pos.y =  0;
	light_pos.z =  -1000;

	light_look.x = 0;
	light_look.y = 0;
	light_look.z = 0;

	guVecMultiply (GetcameraMatrix(), &light_pos, &pos); 
	guVecMultiply (GetcameraMatrix(), &light_look, &look); 

	
	guVecSub (&look, &pos, &light_dir);
	GX_InitSpecularDirv (&LightObject, &light_dir) ;
	GX_InitLightColor (&LightObject, LightColour ) ;
	GX_InitLightShininess (&LightObject, 0.5f);  // sets a relatively open angle 


	GX_LoadLightObj(&LightObject,GX_LIGHT0);
	GX_SetNumChans(1);

	GX_SetChanCtrl(GX_COLOR0A0,GX_ENABLE,GX_SRC_REG,GX_SRC_REG,GX_LIGHT0,GX_DF_CLAMP,GX_AF_SPEC);
    GX_SetChanAmbColor(GX_COLOR0A0,AmbientColour);
    GX_SetChanMatColor(GX_COLOR0A0,MaterialColour);

}

void Camera::SetLightOn(float x, float y, float z)
{
    static const GXColor LightColour  = { 0xff, 0xff, 0xff, 0xFF };
	static const GXColor AmbientColour  = { 0x0f, 0x0f, 0x0f, 0xFF };
    static const GXColor MaterialColour = { 0xff, 0xff, 0xff, 0xFF };

	guVector LightPos;
	LightPos.x = x;//   250000.0f;
	LightPos.y = y;//   250000.0f;
	LightPos.z = z;// -1000000.0f;

	guVecMultiply(GetcameraMatrix(), &LightPos, &LightPos);

	GXLightObj LightObject;
	GX_InitLightPos(&LightObject,LightPos.x,LightPos.y,LightPos.z);
	GX_InitLightColor(&LightObject,LightColour);
	GX_InitLightSpot(&LightObject, 0.0f, GX_SP_OFF);
	GX_InitLightDistAttn(&LightObject, 1.0f, 1.0f, GX_DA_OFF);
	
	GX_LoadLightObj(&LightObject,GX_LIGHT0);
	GX_SetNumChans(1);

	GX_SetChanCtrl(GX_COLOR0A0,GX_ENABLE,GX_SRC_REG,GX_SRC_REG,GX_LIGHT0,GX_DF_CLAMP,GX_AF_SPOT);
    GX_SetChanAmbColor(GX_COLOR0A0,AmbientColour);
    GX_SetChanMatColor(GX_COLOR0A0,MaterialColour);
}

void Camera::SetLightOff() 
{
    GX_SetNumTevStages(1);
 //   GX_SetTevOp  (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetNumChans(1);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
}
