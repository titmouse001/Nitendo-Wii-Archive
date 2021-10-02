#ifndef WiiManager_H
#define WiiManager_H

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>

#include "Singleton.h"
#include "font.h"
#include "Camera.h"
#include "HashLabel.h"

using namespace std;

class ImageManager;
class FontManager;
class Camera;

struct FontInfo
{
	string FileName;
	string LogicName;
};

class WiiManager : private SingletonClient
{
public:
	
	#define ONE_KILOBYTE (1024)
	#define DEFAULT_GX_FIFO_SIZE ( 256 * ONE_KILOBYTE)   // GX hardware, increase if drawing complex items.
	enum EDebugConsole {eDebugConsoleOn, eDebugConsoleOff} ;

	WiiManager();
	~WiiManager();

	// Init
	void InitWii();
	void UnInitWii();
	void ProgramStartUp(std::string FullFileName);
	void InitDebugConsole(int ScreenOriginX = 20, int ScreenOriginY = 20);

	// Display support
	u32* GetCurrentFrame() const;
	u32 GetScreenBufferId() const;
	void SwapScreen(); 
	enum EScreenBuffer { eFrontScreenBuffer,eBackScreenBuffer, eMaxScreenBuffers };
	u32 GetMaxScreenBuffers() const  {return eMaxScreenBuffers;}
	GXRModeObj* GetGXRMode() const	
	{ 
		if  (m_pGXRMode==NULL) exit(1); 
		return m_pGXRMode; 
	}
	void SetGXRMode(GXRModeObj* pMode) 	{ m_pGXRMode = pMode; }
	int GetScreenWidth() const { return GetGXRMode()->fbWidth; }
	int GetScreenHeight() const { return GetGXRMode()->efbHeight; }
	
	// Managers
	ImageManager*		GetImageManager()		{ return m_ImageManager; }
	FontManager*		GetFontManager()		{ return m_FontManager; }
	Camera*				GetCamera()	const		{ return m_Camera; }
	
	// XML supprt
	int GetXmlVariable(HashLabel Name) { return m_VariablesContainer[Name]; }
	int GetXmlVariable(std::string Name) { return m_VariablesContainer[(HashLabel)Name]; }  // try not to use this one
		
	// Font support
	vector<FontInfo>::iterator GetFontInfoBegin() { return m_FontinfoContainer.begin(); }
	vector<FontInfo>::iterator GetFontInfoEnd() { return m_FontinfoContainer.end(); }
	//bool IsFontinfoContainerEmpty() const { return m_FontinfoContainer.empty(); }

	// Debug
	void Printf(int x, int y, const char* pFormat, ...);


private:
	void InitGX(u32 GXFifoBufferSize = DEFAULT_GX_FIFO_SIZE);
	void SetUp3DProjection();
	void CreateSettingsFromXmlConfiguration(std::string FileName);
	void FinaliseInputDevices();
	void InitialiseVideo();

	GXRModeObj* GetBestVideoMode();

	u32* 					m_pFrameBuffer[2];
	GXRModeObj*				m_pGXRMode;
	u32*					m_gp_fifo;
	u32						m_uScreenBufferId;
	ImageManager*			m_ImageManager;
	FontManager*			m_FontManager;
	Camera*					m_Camera;

	vector<FontInfo> m_FontinfoContainer;
	map<HashLabel,float> m_VariablesContainer;
};	



#endif
