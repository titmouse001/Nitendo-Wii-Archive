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
#include "SpriteManager.h"
#include "InputDeviceManager.h"
#include "MapManager.h"
#include "debug.h"

#define DEBUGCONSOLESTATE	( eDebugConsoleOn )
#define COLOUR_FOR_WIPE		( COLOR_GREEN )

WiiManager::WiiManager() :	m_pGXRMode(NULL), m_gp_fifo(NULL), 
							m_uScreenBufferId(eFrontScreenBuffer), 
							m_ImageManager(NULL), 
							m_MapManager(NULL), 
							m_SpriteManager(NULL),
							m_ViewportX(0),
							m_ViewportY(0)
{ 
	m_pFrameBuffer[0] = NULL;
	m_pFrameBuffer[1] = NULL;
	m_ImageManager			= new ImageManager;
	m_FontManager			= new FontManager;
	m_InputDeviceManager	= new InputDeviceManager;
	m_MapManager			= new MapManager;
	m_SpriteManager			= new SpriteManager;

	WiiFile::InitFileSystem();

	InitScreen();

	InitInputDevices();  // maybe do this first? but has dependancy on screen size
}

WiiManager::~WiiManager()
{
	WPAD_Shutdown();

	UnInitScreen();

	// note: The following manager's do internal housekeeping
	delete m_ImageManager;		
	delete m_FontManager;	
	delete m_InputDeviceManager;
	delete m_MapManager;	
	delete m_SpriteManager;		
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

//	SetUp2DProjection();
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

	GX_SetCopyClear((GXColor){0, 0, 0, 0xff}, GX_MAX_Z24);   // set EFB to clear frames with this colour

	GX_SetDispCopyGamma(GX_GM_1_0);  // Darkest setting - looks ok to me
 
	VIDEO_Flush(); // Apply hardware changes

	GXRModeObj* pMode( GetGXRMode() );
	// Set using the defaults, this part will be called again later with the correct screen details
	GX_SetViewport(0,0,pMode->fbWidth,pMode->efbHeight,0,1);  
	GX_SetScissor(0,0,pMode->fbWidth,pMode->efbHeight);

	VIDEO_Flush(); // Apply hardware changes

	GX_SetDispCopySrc(0,0,pMode->fbWidth,pMode->efbHeight);
	//	The stride of the XFB is set using GX_SetDispCopyDst()
	f32 YScaleFactor (GX_GetYScaleFactor(pMode->efbHeight,(float)pMode->xfbHeight));
	//ExitPrintf("   %f",YScaleFactor);
	u32 xfbHeight (GX_SetDispCopyYScale(YScaleFactor));  // GX_SetDispCopySrc must be called first
	//ExitPrintf("    %d",xfbHeight);
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
#warning *** DONT FORGET TO REMOVE THIS IN THE FINAL BUILDS ***
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
}

//void WiiManager::SetBackGroundClearColour(u8 Red , u8 Green, u8 Blue, u8 Alpha)
//{
//	GX_SetCopyClear((GXColor){Red, Green, Blue, Alpha}, GX_MAX_Z24);// Sets color and Z value to clear the EFB to
//}

void WiiManager::InitInputDevices() const
{
	WPAD_Init(); // all attached InputDeviceManagerers
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);  // use everthing
	if (m_pGXRMode==NULL)
		ExitPrintf("m_pGXRMode is null, did you forget to initialise screen first");
	else
		WPAD_SetVRes(WPAD_CHAN_ALL, GetGXRMode()->fbWidth, GetGXRMode()->xfbHeight);  // resolution of IR
}

s16	vertices[] ATTRIBUTE_ALIGN(32) = {
	0, 15, 0,
	-15, -15, 0,
	15,	-15, 0};

u8 colors[]	ATTRIBUTE_ALIGN(32)	= {
	255, 0,	0, 255,		// red
	0, 255,	0, 255,		// green
	0, 0, 255, 255};	// blue

Mtx	projection;

// Matrix tutorial:- http://www.gamedev.net/reference/articles/article877.asp
void	WiiManager::SetUp3DProjection()
{
	GXRModeObj* pMode( GetGXRMode() );
	f32 w = pMode->viWidth;
	f32 h = pMode->viHeight;
	Mtx perspective;
	guPerspective(perspective, 45*2, (f32)w/h, 0.1F, 800.0F);
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
    GX_SetVtxAttrFmt (GX_VTXFMT3, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);

	//GX_VTXFMT4 used for tex with normals
    GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);   // Each call to a vertex attribute must match the order: Position, normal, color, texcoord. 
	GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
//	GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);



	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_TRUE);

    GX_SetNumChans(1);
    GX_SetNumTexGens(1);

    GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	guMtxIdentity(m_GXmodelView2D);  // reset
	//cam height set again later...!!!
    guMtxTransApply (m_GXmodelView2D, m_GXmodelView2D, 0.0F, 0.0F, 200.0F);  // move camera above ground
    GX_LoadPosMtxImm(m_GXmodelView2D, GX_PNMTX0);   // any point doing this now??

    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);  // does this have any effect?
    GX_SetCullMode(GX_CULL_NONE);
//	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
}

// Matrix tutorial:- http://www.gamedev.net/reference/articles/article877.asp
void	WiiManager::SetUp2DProjection()
{
    GX_InvalidateTexAll();
	GX_ClearVtxDesc();
	GX_InvVtxCache ();

    GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	//GX_VTXFMT0 used for lines
    GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_S16, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
 
	//GX_VTXFMT1 used for ploy textures
	GX_SetVtxAttrFmt (GX_VTXFMT1, GX_VA_POS, GX_POS_XY, GX_S16, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	//GX_VTXFMT2 used for ploy textures NO ALPHA
	GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_POS, GX_POS_XY, GX_S16, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);


	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_TRUE);

    GX_SetNumChans(1);
    GX_SetNumTexGens(1);
    GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	guMtxIdentity(m_GXmodelView2D);  // reset
	// set some default camera values - don't think these values are important for Ortho view
    guMtxTransApply (m_GXmodelView2D, m_GXmodelView2D, 0.0F, 0.0F, -50.0F);  // move camera above ground
    GX_LoadPosMtxImm(m_GXmodelView2D, GX_PNMTX0);

	GXRModeObj* pMode( GetGXRMode() );
	//Orthographic projection means: "Show this area of the 3D world, and don't taper off 
	// the images as they get farther from the camera."
	Mtx44 perspective;
	guOrtho(perspective, 0, pMode->efbHeight, 0, pMode->fbWidth, 0.0f, 1.0f);
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);


    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);  // does this have any effect?
    GX_SetCullMode(GX_CULL_NONE);
	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
}
//
//void WiiManager::SetViewPort(f32 x,f32 y,f32 w, f32 h)
//{
//	SetViewportX( x );
//	SetViewportY( y );
//    GX_SetViewport(x, y, w, h, 0.0f, 1.0f);
//	GX_SetScissor(x, y, w, h);
//}


//------------------------------

// 'ResetSwapScreen' ensures that the first frame buffer is used next
// usefull for things like bebugging where we have little control over things like 'printf'
void WiiManager::ResetSwapScreen()
{
	if (GetScreenBufferId() == 1) 
	{
		SwapScreen();
	}
}


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

#if 1   //test border
	GetImageManager()->Line(100, 100, GetScreenWidth()-1+100, 100,0xffffffff, 0xffffffff  );
	GetImageManager()->Line(100, 100, 100,100+GetScreenHeight()-1, 0xffffffff, 0xffffffff  );
	GetImageManager()->Line(GetScreenWidth()-1+100, 100, GetScreenWidth()-1+100, 100+GetScreenHeight()-1, 0xffffffff, 0xffffffff  );
	GetImageManager()->Line(100, 100+GetScreenHeight()-1, 100+GetScreenWidth()-1, 100+GetScreenHeight()-1, 0xffffffff, 0xffffffff  );
#endif

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

void WiiManager::DrawScene(MapLayers* pMapLayers)
{
	// The camera view looks at the centre
	// So here we need to take that into acount.

	for (vector<Map*>::iterator pMap(pMapLayers->GetBegin()); pMap != pMapLayers->GetEnd(); ++pMap)
	{
		if ((*pMap)->GetMapType() == eMapIsHit)
			continue;

		int ImageWidth = GetImageManager()->GetImage(0)->GetWidth(); // w,h of any image in that bank will do
		int ImageHeight = GetImageManager()->GetImage(0)->GetHeight();
		int StartX ( ((int)(*pMap)->GetViewXpos() / ImageWidth * ImageWidth ) - GetScreenWidth()/2);
		int StartY ( ((int)(*pMap)->GetViewYpos() / ImageHeight * ImageHeight )- GetScreenHeight()/2);

		StartX = (StartX / ImageWidth) * ImageWidth;  
		StartY = (StartY / ImageHeight) * ImageHeight; 

		if (StartX<0) 
			StartX=0;
		if (StartY<0) 
			StartY=0;

//		u32 UsableMapWidthInPixels( (*pMap)->GetTotalPixelWidth() /2);
//		u32 UsableMapHeightInPixels( (*pMap)->GetTotalPixelHeight()  /2);

		if ((*pMap)->GetMapType() == eMapIsSolidLayer)
			GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR); //disable alpha 
		else
			GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR); // enable alpha

		for (int iy( StartY  ); iy < StartY + GetScreenHeight() + ImageHeight  ; iy +=  ImageHeight ) 
		{
			for (int ix( StartX  ); ix < StartX + GetScreenWidth() + ImageWidth; ix += ImageWidth)
			{
				u32 MapCell( (*pMap)->GetValueFromOrigin( ix, iy ) );
				MapCell &= 0x7fff;
				Image* pImage(GetImageManager()->GetImage( MapCell ));

				if ((*pMap)->GetMapType() == eMapIsTransparentLayer)
				{
					if (MapCell!=0) 
						pImage->DrawImageFor3D( ix, iy );
				}
				else
				{
					pImage->DrawImageNoAlphaFor3D( ix, iy );
				}

			}
		}
	}
}