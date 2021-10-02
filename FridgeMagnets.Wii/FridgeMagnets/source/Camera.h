#ifndef Camera_H
#define Camera_H

#include "GCTypes.h"
#include "ogc/gx.h"
#include <gccore.h>

using namespace std;

class Camera
{

private:
	Mtx			m_cameraMatrix;
	guVector	m_camera;
	guVector	m_up;
	guVector	m_look;

	//float		m_CamX;
	//float		m_CamY;
	//float		m_CamZ;
	//guVector	m_Position;

	float		m_CameraFactor;

	//float WiiMotePointerXpos;
	//float WiiMotePointerYpos;
public:

	float GetCamX() const { return m_camera.x; }
	void SetCamX(float Value) {  m_camera.x = Value; }
	void AddCamX(float Value) {  m_camera.x += Value; }
	
	float GetCamY() const { return m_camera.y; }
	void SetCamY(float Value) {  m_camera.y = Value; }
	void AddCamY(float Value) {  m_camera.y += Value; }

	void SetCamZ(float Value) {  m_camera.z = Value; }
	float GetCamZ() const {  return m_camera.z; }

	
	void SetCam(float x, float y, float z)  { m_camera.x = x; m_camera.y = y; m_camera.z = z; }


//	void SetWiiMotePointer(float x, float y) {WiiMotePointerXpos = x; WiiMotePointerYpos = y; }
//	float GetWiiMotePointerX() const { return WiiMotePointerXpos; }
//	float GetWiiMotePointerY() const { return WiiMotePointerYpos; }

	float GetCameraFactor() const { return m_CameraFactor; }
	void  SetCameraFactor(float Value) { m_CameraFactor = Value; }

	Mtx&  GetcameraMatrix() { return m_cameraMatrix; }

	void InitialiseCamera(float ZoomFactor = 1.0f);
	void SetCameraView();
};


#endif