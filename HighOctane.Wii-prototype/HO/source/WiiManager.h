#ifndef WiiManager_H
#define WiiManager_H

#include "Singleton.h"
#include "font.h"
#include "MapManager.h"
#include <stdlib.h>
#include <string>
using namespace std;

class ImageManager;
class FontManager;
class InputDeviceManager;
class SpriteManager;

class WiiManager : private SingletonClient
{
public:
	
	#define ONE_KILOBYTE (1024)
	#define DEFAULT_GX_FIFO_SIZE ( (256) * ONE_KILOBYTE)
	enum EDebugConsole {eDebugConsoleOn, eDebugConsoleOff} ;

	WiiManager();
	~WiiManager();

	void InitScreen();
	void UnInitScreen();
	void InitInputDevices() const;

	void InitialiseVideo(GXRModeObj* pScreenMode);

	void InitDebugConsole(int ScreenOriginX = 20, int ScreenOriginY = 20);

	void DrawScene(MapLayers* pMapLayers);

	u32* GetCurrentFrame() const;
	u32 GetScreenBufferId() const;
	void SwapScreen(); 
	void ResetSwapScreen();

	enum EScreenBuffer { eFrontScreenBuffer,eBackScreenBuffer, eMaxScreenBuffers };
	u32 GetMaxScreenBuffers() const  {return eMaxScreenBuffers;}
	GXRModeObj* GetGXRMode() const	
	{ 
		if  (m_pGXRMode==NULL) exit(1); 
		return m_pGXRMode; 
	}
	void SetGXRMode(GXRModeObj* pMode) 	{ m_pGXRMode = pMode; }

	int GetScreenWidth() { return GetGXRMode()->fbWidth; }
	int GetScreenHeight() { return GetGXRMode()->efbHeight; }
	

	Mtx m_GXmodelView2D;

	// Managers
	ImageManager*		GetImageManager()		{ return m_ImageManager; }
	FontManager*		GetFontManager()		{ return m_FontManager; }
	InputDeviceManager*	GetInputDeviceManager()	{ return m_InputDeviceManager; }
	MapManager*			GetMapManager()			{ return m_MapManager; }
	SpriteManager*		GetSpriteManager()		{ return m_SpriteManager; }

	s16 GetViewportX() const { return m_ViewportX; }
	s16 GetViewportY() const { return m_ViewportY; }

	void InitGX(u32 GXFifoBufferSize = DEFAULT_GX_FIFO_SIZE);
	void SetUp2DProjection();
	void SetUp3DProjection();


private:


	void SetViewport(s16 x, s16 y) { m_ViewportX = x;m_ViewportY=y; }

	u32* 					m_pFrameBuffer[2];
	GXRModeObj*				m_pGXRMode;
	u32*					m_gp_fifo;
	u32						m_uScreenBufferId;
	ImageManager*			m_ImageManager;
	FontManager*			m_FontManager;
	InputDeviceManager*		m_InputDeviceManager;
	MapManager*				m_MapManager;
	SpriteManager*			m_SpriteManager;

	s16						m_ViewportX;
	s16						m_ViewportY;
};	



#endif
