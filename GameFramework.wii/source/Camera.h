#ifndef Camera_H
#define Camera_H

#include "ogc/gx.h"
using namespace std;

class Camera
{

public:
	
	Camera() : m_FieldOfView( 45.0f ) {;}

	void InitialiseCamera(float ZoomFactor = 1.0f);

	void SetCameraView(float x, float y);
	void SetCameraView(float x, float y, float z);

	float GetCamX() const { return m_camera.x; }
	float GetCamY() const { return m_camera.y; }
	float GetCamZ() const {  return m_camera.z; }

	void SetCamX(float Value) {  m_camera.x = Value; }
	void SetCamY(float Value) {  m_camera.y = Value; }
	void SetCamZ(float Value) {  m_camera.z = Value; }
	
	void AddCamX(float Value) {  m_camera.x += Value; }
	void AddCamY(float Value) {  m_camera.y += Value; }
	void AddCamZ(float Value) {  m_camera.z += Value; }
	
	Mtx&  GetcameraMatrix() { return m_cameraMatrix; }

	float GetFOV() const { return m_FieldOfView; }
	void SetFOV(float fValue) { m_FieldOfView = fValue; }

	void SetLightOn(float x = 250000.0f, float y = 250000.0f, float z = -1000000.0f);
	void SetLightOn2();
	void SetLightOff();

private:

	void SetCameraView();

	Mtx			m_cameraMatrix;
	guVector	m_camera;
	guVector	m_up;
	guVector	m_look;
	float		m_FieldOfView;
};


#endif