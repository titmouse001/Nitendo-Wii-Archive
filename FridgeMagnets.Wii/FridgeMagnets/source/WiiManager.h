#ifndef WiiManager_H
#define WiiManager_H

#include "Singleton.h"
#include "font.h"
//#include "MapManager.h"
#include "Camera.h"
#include <stdlib.h>
#include <string>
#include "FridgeMagnetManager.h"
#include "SoundManager.h"
#include "Debug.h"

using namespace std;

class ImageManager;
class FontManager;
class InputDeviceManager;
class SpriteManager;
class Camera;
class FridgeMagnetManager;
class Menu;

#define FM_CAMZ (2000.0f) 

enum SelectionMode
{
	PickFromCircleLettersSelectionMode,
	CompletedSelectionMode
};

enum  ESounds
{
	eAMMOCLCK_WAV,
	eCLICK_WAV,
	eG10_WAV,
	eWHIP_WAV,
	eG18_WAV,
	eTAP_WAV,
	eFOLLOWPOINTER_WAV,
	eCAMERA_WAV
};

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
	void FinaliseInputDevices() const;

	void InitialiseVideo(GXRModeObj* pScreenMode);

	void InitDebugConsole(int ScreenOriginX = 20, int ScreenOriginY = 20);

//	void DrawScene(MapLayer* pMapLayers);

	u32* GetCurrentFrame() const;
	u32 GetScreenBufferId() const;
	void SwapScreen(); 

	enum EScreenBuffer { eFrontScreenBuffer,eBackScreenBuffer, eMaxScreenBuffers };
	u32 GetMaxScreenBuffers() const  {return eMaxScreenBuffers;}
	GXRModeObj* GetGXRMode() const	
	{ 
		if  (m_pGXRMode!=NULL) 
			return m_pGXRMode; 

		ExitPrintf("Has 'InitScreen' been called?"); 
		return NULL;
	}
	void SetGXRMode(GXRModeObj* pMode) 	{ m_pGXRMode = pMode; }

	int GetScreenWidth() { return GetGXRMode()->fbWidth; }
	int GetScreenHeight() { return GetGXRMode()->efbHeight; }
	

	//Mtx m_GXmodelView2D;

	// Managers
	ImageManager*		 GetImageManager()		  { return m_ImageManager; }
	FontManager*		 GetFontManager()		  { return m_FontManager; }
	InputDeviceManager*	 GetInputDeviceManager()  { return m_InputDeviceManager; }
	//MapManager*			 GetMapManager()		  { return m_MapManager; }  
//	SpriteManager*		 GetSpriteManager()		  { return m_SpriteManager; }
	Camera*				 GetCamera()	const		  { return m_Camera; }
	FridgeMagnetManager* GetFridgeMagnetManager() { return m_FridgeMagnetManager; }
	SoundManager*		GetSoundManager()		{ return m_SoundManager; }

	s16 GetViewportX() const { return m_ViewportX; }
	s16 GetViewportY() const { return m_ViewportY; }

	void InitGX(u32 GXFifoBufferSize = DEFAULT_GX_FIFO_SIZE);
	//void SetUp2DProjection();
	void SetUp3DProjection();

	SelectionMode GetSelectionMode() const {return m_SelectionMode;}
	void SetSelectionMode(SelectionMode Mode) {m_SelectionMode = Mode;}

	int m_CameraIcon;
	bool m_bTakeScreenShot;
//	bool m_UseScreenShotIcon;
//	bool m_HeighLightScreenShotIcon;


	void AddMenu(u32 ItemValue, int x, int y,std::string Name);
	void AddMenu(Image* pImage, int x, int y, int w, int h,std::string Name);
	void DrawAllMenus();
	void DoMenuLogic();
	void MenuAction();
	void SetEnableForAllMenus(bool bState);
	void SetActiveForAllMenus(bool bState);
	void SetEnableAndActivateForMenuItem(bool bState, std::string Name);

	bool m_bHideAllMenuItems;

private:

	void MenuLogic(int WiiControl);

	void SetViewport(s16 x, s16 y) { m_ViewportX = x;m_ViewportY=y; }

	u32* 					m_pFrameBuffer[2];
	GXRModeObj*				m_pGXRMode;
	u32*					m_gp_fifo;
	u32						m_uScreenBufferId;
	ImageManager*			m_ImageManager;
	FontManager*			m_FontManager;
	InputDeviceManager*		m_InputDeviceManager;
	FridgeMagnetManager*    m_FridgeMagnetManager;
	s16						m_ViewportX;
	s16						m_ViewportY;
	Camera*					m_Camera;
	SoundManager*			m_SoundManager;
	SelectionMode			m_SelectionMode;


	std::vector<Menu*>		m_MenuContainer;

};	



#endif
