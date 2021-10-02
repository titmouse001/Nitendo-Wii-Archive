//
// WiiManager - Singleton class
//
#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <string.h>
#include <malloc.h>
#include "WiiManager.h"
#include <string.h>
#include <vector>

//#include "fat.h"
#include "font.h"
#include "Image.h"
#include "ImageManager.h"
#include "fontManager.h"
//#include "SpriteManager.h"
#include "InputDeviceManager.h"
//#include "MapManager.h"
#include "debug.h"
#include "Camera.h"
#include "WiiFile.h"
//#include "oggplayer.h"
//#include <asndlib.h>
#include "menu.h"

#include "util.h"

#define DEBUGCONSOLESTATE	( eDebugConsoleOn )
#define COLOUR_FOR_WIPE		( COLOR_BLACK )  // startup colour

static const GXColor WIPE_COLOUR = (GXColor){245, 245, 245, 0xff};

WiiManager::WiiManager() :	m_bTakeScreenShot(false),
							m_bHideAllMenuItems(false),
							m_pGXRMode(NULL), 
							m_gp_fifo(NULL), 
							m_uScreenBufferId(eFrontScreenBuffer), 
							m_ImageManager(NULL),
							m_FontManager(NULL),
							m_InputDeviceManager(NULL),
							m_FridgeMagnetManager(NULL),
							m_ViewportX(0),
							m_ViewportY(0),
							m_Camera(NULL),
							m_SoundManager(NULL),
							m_SelectionMode(CompletedSelectionMode)

{ 
	WiiFile::InitFileSystem();
	Debug::StartDebugLogging();

	m_pFrameBuffer[0] = NULL;
	m_pFrameBuffer[1] = NULL;
	m_ImageManager			= new ImageManager;
	m_FontManager			= new FontManager;
	m_InputDeviceManager	= new InputDeviceManager;
	m_Camera				= new Camera;
	m_FridgeMagnetManager   = new FridgeMagnetManager;
	m_SoundManager			= new SoundManager;

	WPAD_Init();

	InitScreen();

	FinaliseInputDevices();  

	////	ASND_Init();
}

WiiManager::~WiiManager()
{
	//WPAD_Shutdown();

	UnInitScreen();

	delete m_ImageManager;		
	delete m_FontManager;	
	delete m_InputDeviceManager;
	delete m_Camera;
	delete m_FridgeMagnetManager;

	Debug::StopDebugLogging();
}


void WiiManager::UnInitScreen()
{
	// check we are using a valid GX buffer
	if (m_gp_fifo!=NULL)
	{
	    GX_Flush();
	    GX_AbortFrame();

		free(m_gp_fifo);
		m_gp_fifo = NULL;
	}

	// Each screen Buffer used 'MEM_K0_TO_K1' to allocate - now use the reverse when freeing
	if (m_pFrameBuffer[0] != NULL)
	{
		free(MEM_K1_TO_K0(m_pFrameBuffer[0]));
		m_pFrameBuffer[0] = NULL;
	}

	if (m_pFrameBuffer[1] != NULL)
	{
		free(MEM_K1_TO_K0(m_pFrameBuffer[1]));
		m_pFrameBuffer[1] = NULL;
	}
}

void WiiManager::InitScreen()
{
	SetViewport(0.0f,0.0f);

	InitialiseVideo( VIDEO_GetPreferredMode(NULL) );

	InitGX();

	SetUp3DProjection();
}


// this function may be called more than once
void WiiManager::InitGX(u32 GXFifoBufferSize)
{
	// allocate & clear the GX queue, 
	if (m_gp_fifo==NULL)
	{
		m_gp_fifo = (u32*)memalign(32, GXFifoBufferSize);
		memset(m_gp_fifo, 0, GXFifoBufferSize);
		GX_Init(m_gp_fifo, GXFifoBufferSize);
	}

	VIDEO_Flush(); // Apply hardware changes

	GX_SetCopyClear(WIPE_COLOUR, GX_MAX_Z24);   // set EFB to clear frames with this colour


	GX_SetDispCopyGamma(GX_GM_1_0);  // Darkest setting - looks ok to me
 
	VIDEO_Flush(); // Apply hardware changes

	GXRModeObj* pMode( GetGXRMode() );
	
	GX_SetViewport(0,0,pMode->fbWidth,pMode->efbHeight,0,1);  
	GX_SetScissor(0,0,pMode->fbWidth,pMode->efbHeight);

	VIDEO_Flush(); // Apply hardware changes

	GX_SetDispCopySrc(0,0,pMode->fbWidth,pMode->efbHeight);

	//	The stride of the XFB is set using GX_SetDispCopyDst()
	f32 YScaleFactor (GX_GetYScaleFactor(pMode->efbHeight,pMode->xfbHeight));
	u32 xfbHeight (GX_SetDispCopyYScale(YScaleFactor));  // GX_SetDispCopySrc must be called first

	GX_SetDispCopyDst(pMode->fbWidth,xfbHeight);

	VIDEO_Flush(); // Apply hardware changes

	GX_SetCopyFilter(pMode->aa,pMode->sample_pattern,GX_TRUE,pMode->vfilter);
	
	VIDEO_Flush(); // Apply hardware changes

	if ( pMode->viHeight == (2*pMode->xfbHeight) )
		GX_SetFieldMode(pMode->field_rendering,GX_ENABLE );
	else
		GX_SetFieldMode(pMode->field_rendering,GX_DISABLE);

	VIDEO_Flush(); // Apply hardware changes

	if (pMode->aa)
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	else
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

	VIDEO_Flush(); // Apply hardware changes
}


//private
void WiiManager::InitialiseVideo(GXRModeObj* pScreenMode)
{
	if (pScreenMode == NULL)
		exit(1);

	// Initialise the video system
	VIDEO_Init();

	SetGXRMode( pScreenMode );	// use wii settings

	//1st frame buffer
	if (m_pFrameBuffer[0]!=NULL) free(MEM_K1_TO_K0(m_pFrameBuffer[0]));  // allows the function to be called more than once
	m_pFrameBuffer[0] = static_cast<u32*>(MEM_K0_TO_K1(SYS_AllocateFramebuffer(m_pGXRMode)));
	VIDEO_ClearFrameBuffer (GetGXRMode(), m_pFrameBuffer[0], COLOUR_FOR_WIPE);   

	if (DEBUGCONSOLESTATE == eDebugConsoleOn)
	{
		InitDebugConsole();
	}


	VIDEO_Configure(m_pGXRMode);				// Setup the video registers with the chosen mode

	VIDEO_WaitVSync();												// for VI retrace
	VIDEO_SetNextFramebuffer(m_pFrameBuffer[m_uScreenBufferId]);	// Give H/W a starting point.
	VIDEO_SetBlack(FALSE);											// signal output - show our frame buffer
	VIDEO_Flush();  												// Apply the changes

	// 2nd frame buffer - doing this here should reduce screen starting hickups, if the first buffer is cleared & ready before the first VI
	if (m_pFrameBuffer[1]!=NULL) free(MEM_K1_TO_K0(m_pFrameBuffer[1]));
	m_pFrameBuffer[1] = static_cast<u32*>(MEM_K0_TO_K1(SYS_AllocateFramebuffer(m_pGXRMode)));
	VIDEO_ClearFrameBuffer (GetGXRMode(), m_pFrameBuffer[1], COLOUR_FOR_WIPE );

	if ((GetGXRMode()->viTVMode) & VI_NON_INTERLACE)  
		VIDEO_WaitVSync(); // wait for 2nd interlace to finnish.. is this really needed?
}

void WiiManager::InitDebugConsole(int ScreenOriginX, int ScreenOriginY)
{	
#ifndef BUILD_FINAL_RELEASE

#warning *** InitDebugConsole DONT FORGET TO REMOVE '#define BUILD_FINAL_RELEASE' in config.h from the final build ***

	//noticed 'InitDebugConsole' clears the given buffer with black

	// Initialise the console, required for printf (hopefully this can be called more than once, can't find any uninit?)
	console_init(	m_pFrameBuffer[0],
					ScreenOriginX,
					ScreenOriginY,
					m_pGXRMode->fbWidth,
					m_pGXRMode->xfbHeight,
					m_pGXRMode->fbWidth * VI_DISPLAY_PIX_SZ);

	VIDEO_ClearFrameBuffer (m_pGXRMode, m_pFrameBuffer[0], COLOUR_FOR_WIPE );  // put back the orginal wipe

	//CON_InitEx(GetGXRMode(), ScreenOriginX,ScreenOriginY,m_pGXRMode->fbWidth,m_pGXRMode->xfbHeight);

	printf("\n\n");
#endif
}

//void WiiManager::SetBackGroundClearColour(u8 Red , u8 Green, u8 Blue, u8 Alpha)
//{
//	GX_SetCopyClear((GXColor){Red, Green, Blue, Alpha}, GX_MAX_Z24);// Sets color and Z value to clear the EFB to
//}

void WiiManager::FinaliseInputDevices() const
{
	float CameraFactor(1);// (GetCamera()->GetCameraFactor());

//	WPAD_Init(); // all attached InputDeviceManagerers
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);  // use everthing
	if (m_pGXRMode==NULL)
		ExitPrintf("m_pGXRMode is null, did you forget to initialise screen first");
	else
		WPAD_SetVRes(WPAD_CHAN_ALL, GetGXRMode()->fbWidth * CameraFactor, GetGXRMode()->xfbHeight * CameraFactor);  // resolution of IR
}

// Matrix tutorial:- http://www.gamedev.net/reference/articles/article877.asp
void	WiiManager::SetUp3DProjection()
{
	Mtx44 perspective;
	guPerspective(perspective, 45.0f*2.0f, (f32)GetGXRMode()->viWidth / (f32)GetGXRMode()->viHeight , 0.1f, FM_CAMZ); 
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);   


    GX_InvalidateTexAll();
	GX_InvVtxCache ();

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
//	GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
//    GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);

 //
	//GX_SetChanCtrl(GX_COLOR0A0,GX_ENABLE,GX_SRC_REG,GX_SRC_REG,GX_LIGHT0,GX_DF_CLAMP,GX_AF_NONE);      
 //   GX_SetChanAmbColor(GX_COLOR0A0, (GXColor){120, 120, 120, 255});
 //   GX_SetChanMatColor(GX_COLOR0A0, (GXColor){80, 80, 80, 255});



	//GX_VTXFMT0 used for lines
    GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);   // Each call to a vertex attribute must match the order: Position, normal, color, texcoord. 
	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

  //  GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
//    GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
 
	//GX_VTXFMT1 used for ploy textures
	GX_SetVtxAttrFmt (GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	//GX_VTXFMT2 used for ploy textures NO ALPHA
	GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    
	GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_CLR0, GX_CLR_RGB, GX_RGB565, 0);
//	GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
//    GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_TEX0, GX_TEX_ST, GX_U16, 0);

	//GX_VTXFMT3 used for test colour polys
	GX_SetVtxAttrFmt (GX_VTXFMT3, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT3, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

	//GX_VTXFMT4 used for solid with normals
    GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);   // Each call to a vertex attribute must match the order: Position, normal, color, texcoord. 
	GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    GX_SetNumChans(1);
    GX_SetNumTexGens(1);

    GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);  
    GX_SetCullMode(GX_CULL_NONE);
	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_TRUE);
}

// Matrix tutorial:- http://www.gamedev.net/reference/articles/article877.asp


//------------------------------

u32* WiiManager::GetCurrentFrame() const 
{ 
	return m_pFrameBuffer[m_uScreenBufferId]; 
}

u32 WiiManager::GetScreenBufferId() const 
{ 
	return m_uScreenBufferId; 
}

void WiiManager::SwapScreen()
{
	GX_DrawDone();  // flush & wait for the pipline (could use callback here)

	GX_CopyDisp( GetCurrentFrame() , GX_TRUE); // mainly to clear the z buffer (don't think its possible to clear just the Z buffer ? )
	GX_Flush();

	++m_uScreenBufferId; 
	if (m_uScreenBufferId >= GetMaxScreenBuffers()) 
		m_uScreenBufferId=0;
	// calling 'VIDEO_WaitVSync' at the incorrect time can cause screen tearing when the hardware flips
	VIDEO_WaitVSync(); //must wait here for a full VBL before the next frame is set

	VIDEO_SetNextFramebuffer( GetCurrentFrame() );
	VIDEO_Flush();
}


void WiiManager::AddMenu(u32 ItemValue,int x, int y,std::string Name)
{
	Image* pImage( GetImageManager()->GetImage(ItemValue) );
	AddMenu(pImage,x,y, pImage->GetWidth(), pImage->GetHeight(),Name );
}

void WiiManager::AddMenu(Image* pImage,int x, int y, int w, int h,std::string Name)
{
	Menu* pMenu( new Menu(pImage,x,y,w,h,Name) );
	m_MenuContainer.push_back( pMenu );
}

void WiiManager::DoMenuLogic()
{
	for (std::vector<Menu*>::iterator Iter(m_MenuContainer.begin()); Iter!=m_MenuContainer.end(); ++Iter)
	{
		(*Iter)->SetHightLight( false );
		(*Iter)->SetSelected( false );
	}

	MenuLogic(WPAD_CHAN_0);
	MenuLogic(WPAD_CHAN_1);
	MenuLogic(WPAD_CHAN_2);
	MenuLogic(WPAD_CHAN_3);

	MenuAction();
}

void WiiManager::SetEnableForAllMenus(bool bState)
{
	for (std::vector<Menu*>::iterator Iter(m_MenuContainer.begin()); Iter!=m_MenuContainer.end(); ++Iter)
	{
		(*Iter)->SetEnable(bState);
	}
}

void WiiManager::SetActiveForAllMenus(bool bState)
{
	for (std::vector<Menu*>::iterator Iter(m_MenuContainer.begin()); Iter!=m_MenuContainer.end(); ++Iter)
	{
		(*Iter)->SetActive(bState);
	}
}

void WiiManager::SetEnableAndActivateForMenuItem(bool bState, std::string Name)
{
	for (std::vector<Menu*>::iterator Iter(m_MenuContainer.begin()); Iter!=m_MenuContainer.end(); ++Iter)
	{
		if ( Name == (*Iter)->GetName() )
		{
			(*Iter)->SetEnable(bState);
			(*Iter)->SetActive(bState);
			break;
		}
	}
}


void WiiManager::MenuAction()
{
	for (std::vector<Menu*>::iterator Iter(m_MenuContainer.begin()); Iter!=m_MenuContainer.end(); ++Iter)
	{
		Menu* pMenu( *Iter );
		if (pMenu->GetSelected( )==true)
		{
			pMenu->SetSelected(false);

			if (pMenu->GetName() == "Wipe")
			{
				GetFridgeMagnetManager()->RemoveAllFridgeMagnets();
				GetSoundManager()->GetSound(eWHIP_WAV)->Play();
				
				SetEnableAndActivateForMenuItem(true,"Letters");  // stop the letters icon from staying grayed out - maybe a better way??
			}
			else if (pMenu->GetName() == "ScreenShot")
			{
				SetEnableForAllMenus(false);  // hide menus
				m_bTakeScreenShot = true;
				GetSoundManager()->GetSound(eCAMERA_WAV)->Play();
			}
			else if (pMenu->GetName() == "Letters")
			{
				GetSoundManager()->GetSound(eCLICK_WAV)->Play();

				SetSelectionMode( PickFromCircleLettersSelectionMode );
				GetFridgeMagnetManager()->DisableAllFridgeMagnets(true);
				GetFridgeMagnetManager()->AddPickerCircleLetters("abcdefghijklmnopqrstuvwxyz",1.00f,1.00f);
				GetFridgeMagnetManager()->AddPickerCircleLetters("0123456789/=+-",0.55f,1.0f);
		
				pMenu->SetActive(false);
			}
		}
	}
}

void WiiManager::MenuLogic(int WiiControl)
{
	vec3f_t* pos(GetInputDeviceManager()->GetIRPosition( WiiControl ));
	if (pos==NULL || m_bHideAllMenuItems)
		return;

	u32 uButtonsDown(WPAD_ButtonsDown(WiiControl));
	float WiiMotePointerXpos = pos->x - (GetScreenWidth()/2);
	float WiiMotePointerYpos = pos->y - (GetScreenHeight()/2);

	for (std::vector<Menu*>::iterator Iter(m_MenuContainer.begin()); Iter!=m_MenuContainer.end(); ++Iter)
	{
		Menu* pMenu( *Iter );
		if ( pMenu->GetActive() && pMenu->GetEnable() )
		{
			bool bOver = pMenu->IsOverMenu(WiiMotePointerXpos,WiiMotePointerYpos);

			if ( bOver )
				pMenu->SetHightLight( true );
			
			if ( ( uButtonsDown & (WPAD_BUTTON_A|WPAD_BUTTON_B) ) && bOver )
				pMenu->SetSelected( true );
		}
	}
	
}

void WiiManager::DrawAllMenus()
{
	float LastFactor = GetCamera()->GetCameraFactor();

	//------ correct 3d camera for menu view
	float Factor = 1.0f;
	GetCamera()->SetCameraFactor(Factor);
	GetCamera()->SetCamZ( -(GetScreenHeight()/2) * Factor );
	GetCamera()->SetCameraView(); 
	//------

	int camx( GetCamera()->GetCamX() );
	int camy( GetCamera()->GetCamY() );

	for (std::vector<Menu*>::iterator Iter(m_MenuContainer.begin()); Iter!=m_MenuContainer.end(); ++Iter)
	{
		Menu* pMenu( *Iter );
		if (pMenu->GetEnable())
		{
			guVector& Pos( pMenu->GetPos() );

			if (pMenu->GetActive() && !m_bHideAllMenuItems)
			{
				
				if ( pMenu->GetHightLight() )
				{
					Util::Translate( camx+ Pos.x,camy + Pos.y,1.25f );
					pMenu->GetImage()->DrawImage(pMenu->GetFadeValue(),0xaa,0xaa,0xff);
				}
				else
				{
					Util::Translate( camx+ Pos.x,camy + Pos.y );
					pMenu->GetImage()->DrawImage(pMenu->GetFadeValue(),0xff,0xff,0xff);
				}

				pMenu->AddFadeValue( (255.0f - ( pMenu->GetFadeValue() ))*0.10 );
			}
			else
			{
				Util::Translate( camx+ Pos.x /*+ pMenu->GetImage()->GetWidth()/2*/, camy + Pos.y );
				pMenu->GetImage()->DrawImage(pMenu->GetFadeValue(),0xff,0xff,0xff);

				pMenu->AddFadeValue( (44.0f - ( pMenu->GetFadeValue() ))*0.10 );

			}
		}
	}

	//----- back to normal view
	GetCamera()->SetCameraFactor(LastFactor);
	GetCamera()->SetCamZ( -(GetScreenHeight()/2) * LastFactor );
	GetCamera()->SetCameraView(); 
	//------
}
