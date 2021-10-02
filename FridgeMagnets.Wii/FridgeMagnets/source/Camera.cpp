#include "Camera.h"
#include "WiiManager.h"
#include "debug.h"
//#include <gccore.h>

// FOV set at 90 , giving a full left & right view. 
// As the camera height is known the other 45deg length must be the same.
// Note: Camera looks up to flip the view so it will work the same way as the 2d view.
//	CameraFactor, this affects the camera height, the real 3D world width/height of the screen

void Camera::InitialiseCamera(float ZoomFactor)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );  // backbone stuff

	m_CameraFactor = ZoomFactor;

	//default view
	//m_Position.x = (Wii.GetScreenWidth()/2) * m_CameraFactor;
	//m_Position.y = (Wii.GetScreenHeight()/2) * m_CameraFactor;
	//m_Position.z = -(Wii.GetScreenHeight()/2) * m_CameraFactor;

	SetCam( (Wii.GetScreenWidth()/2) * m_CameraFactor,
			(Wii.GetScreenHeight()/2) * m_CameraFactor,
			-(Wii.GetScreenHeight()/2) * m_CameraFactor );

	static const guVector UP = {0.0F, -1.0F, 0.0F}; 
	//m_camera = m_Position;//  { m_CamX, m_CamY, m_CamZ}; 
	m_up = UP; 
	m_look	=  m_camera; //m_Position; //{m_CamX, m_CamY, 0.0F};
	m_look.z = 0.0f;
	SetCameraView();
}

void Camera::SetCameraView()
{
	static const guVector UP = {0.0F, -1.0F, 0.0F}; 
	//m_camera = m_Position; //{ m_CamX, m_CamY, m_CamZ}; 
	m_up = UP;  
	m_look	= m_camera; // {m_Position.x, m_Position.y, 0.0F};
	m_look.z = 0.0f;
	guLookAt(m_cameraMatrix, &m_camera,	&m_up, &m_look);
}