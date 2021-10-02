//
// WiiManager - Singleton class
//
#include <wiiuse/wpad.h>
#include <malloc.h>
#include <stdarg.h>   // needed for Printf()
#include <math.h>

#include "WiiManager.h"
#include "font.h"
#include "ImageManager.h"
#include "fontManager.h"
#include "debug.h"
#include "Util3D.h"
#include "Util.h"
#include "TinyXML/TinyXML.h"
#include "FontManager.h"
#include "HashLabel.h"
#include "HashString.h"
#include "WiiFile.h"
#include "config.h"

#define DEBUGCONSOLESTATE	( eDebugConsoleOn )  // (for displaying old style 'c' printf's) it's ignored (off) in final release
#define COLOUR_FOR_WIPE		( COLOR_BLACK )  // crash at startup colour


WiiManager::WiiManager() :	m_pGXRMode(NULL), m_gp_fifo(NULL), 
							m_uScreenBufferId(eFrontScreenBuffer), 
							m_ImageManager(NULL), 
							m_Camera(NULL)												
{ 
	m_pFrameBuffer[0] = NULL;
	m_pFrameBuffer[1] = NULL;
	m_ImageManager			= new ImageManager;
	m_FontManager			= new FontManager;
	m_Camera				= new Camera;
}

WiiManager::~WiiManager()
{
	delete m_ImageManager;		
	delete m_FontManager;	
	delete m_Camera;
}

void WiiManager::InitWii()
{
	// wiimote - pre init
	WPAD_Init();
	WPAD_SetIdleTimeout(GetXmlVariable( HashString::WiiMoteIdleTimeoutInSeconds ));   // uses pre calculated hashes... see HashString
	// this method can also be use, but it might store the string multiple times resulting in larger code and its not a good idea inside heavy logic
	// WPAD_SetIdleTimeout(GetXmlVariable( (HashLabel) "WiiMoteIdleTimeoutInSeconds" ));  // aim to avoid this

	// fat
	WiiFile::InitFileSystem();

	//Screen specific
	InitialiseVideo();
	InitGX();
	SetUp3DProjection();

	FinaliseInputDevices();
}


void WiiManager::UnInitWii()
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

	WPAD_Shutdown();
}

GXRModeObj* WiiManager::GetBestVideoMode()
{
	// TODO... need to check is all this is correct

	GXRModeObj* vmode = VIDEO_GetPreferredMode(NULL); // get default video mode

	bool pal = false;

	if (vmode == &TVPal528IntDf)
	{
		pal = true;
		vmode = &TVPal574IntDfScale;
	}

	if (CONF_GetAspectRatio() == CONF_ASPECT_16_9)
	{
		vmode->viWidth = 678;  // this value has been checked on flat screen and old TV
		//vmode->viWidth = 720;  // Don't like this large value.. just too much on everything I've tried..chops off horz edges
	}
//	else
//	{
//		vmode->viWidth = 678;
//	}


	if (pal)
	{
		vmode->viXOrigin = (VI_MAX_WIDTH_PAL - vmode->viWidth) / 2;
		vmode->viYOrigin = (VI_MAX_HEIGHT_PAL - vmode->viHeight) / 2;
	}
	else
	{
		vmode->viXOrigin = (VI_MAX_WIDTH_NTSC - vmode->viWidth) / 2;
		vmode->viYOrigin = (VI_MAX_HEIGHT_NTSC - vmode->viHeight) / 2;
	}

	s8 hoffset = 0;

	if (CONF_GetDisplayOffsetH(&hoffset) == 0)
		vmode->viXOrigin += hoffset;

	return vmode;
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

	static const GXColor WIPE_COLOUR = (GXColor){0, 0, 0, 0xff};
	GX_SetCopyClear(WIPE_COLOUR, GX_MAX_Z24);   // set EFB to clear frames with this colour


	GX_SetDispCopyGamma(GX_GM_1_0);  // Darkest setting - looks ok to me
 
	VIDEO_Flush(); // Apply hardware changes

	GXRModeObj* pMode( GetGXRMode() );
	GX_SetViewport(0,0,pMode->fbWidth,pMode->efbHeight,0,1);  
	GX_SetScissor(0,0,pMode->fbWidth,pMode->efbHeight);

	VIDEO_Flush(); // Apply hardware changes

	GX_SetDispCopySrc(0,0,pMode->fbWidth,pMode->efbHeight);

	//	The stride of the XFB is set using GX_SetDispCopyDst()
	f32 YScaleFactor (GX_GetYScaleFactor(pMode->efbHeight,(float)pMode->xfbHeight));
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


void WiiManager::InitialiseVideo()
{
//	GXRModeObj* pScreenMode( VIDEO_GetPreferredMode(NULL) );
	GXRModeObj* pScreenMode( GetBestVideoMode() );

	if (pScreenMode != NULL)
	{
		SetGXRMode( pScreenMode );		// keep a working copy - bit nasty as it creates dependances

		// Initialise the video system
		VIDEO_Init();

		// incase InitialiseVideo is called multiple times - it may change to another display region later.
		if (m_pFrameBuffer[0] != NULL) // Has the 1'st frame buffer already been set
		{
			free(MEM_K1_TO_K0(m_pFrameBuffer[0]));
		}
		m_pFrameBuffer[0] = static_cast<u32*>(MEM_K0_TO_K1(SYS_AllocateFramebuffer(m_pGXRMode)));
		VIDEO_ClearFrameBuffer (GetGXRMode(), m_pFrameBuffer[0], COLOUR_FOR_WIPE);   

		if (DEBUGCONSOLESTATE == eDebugConsoleOn)
		{
			InitDebugConsole();
		}

		VIDEO_Configure( GetGXRMode() );	// Setup the video registers with the chosen mode

		VIDEO_WaitVSync();												// for VI retrace
		VIDEO_SetNextFramebuffer(m_pFrameBuffer[m_uScreenBufferId]);	// Give H/W a starting point.
		VIDEO_SetBlack(FALSE);											// signal output - show our frame buffer
		VIDEO_Flush();  												// Apply the changes

		// 2nd frame buffer - doing this workload here should reduce screen starting hickups
		// need to make sure the first buffer is ready before the VI displays it
		if (m_pFrameBuffer[1] != NULL) 
		{
			free(MEM_K1_TO_K0(m_pFrameBuffer[1]));
		}
		m_pFrameBuffer[1] = static_cast<u32*>(MEM_K0_TO_K1(SYS_AllocateFramebuffer(m_pGXRMode)));
		VIDEO_ClearFrameBuffer (GetGXRMode(), m_pFrameBuffer[1], COLOUR_FOR_WIPE );

		if ((GetGXRMode()->viTVMode) & VI_NON_INTERLACE)  
			VIDEO_WaitVSync(); // wait for 2nd interlace to finnish.. is this really needed?

	}
}

void WiiManager::FinaliseInputDevices()
{
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);  // turn on everthing
	
//	if (m_pGXRMode==NULL)
//		ExitPrintf("m_pGXRMode is null, did you forget to initialise screen first");
//	else
//	{
//		WPAD_SetVRes(WPAD_CHAN_ALL, GetScreenWidth()+ BothEdgesX, GetScreenHeight()+ BothEdgesY );  // dynamic resolution - IR
//		//PAL & NTSC - keep in mind PAL height/width can be more than NTSC
//		//we add on +100 to allow a little movment around the edges, the working code later just takes off 50 to give normal values
//	}

	// keep it simple for this framework 
	WPAD_SetVRes(WPAD_CHAN_ALL, GetScreenWidth(), GetScreenHeight() );  // resolution of IR
	WPAD_SetIdleTimeout( 60 * 5 );  // mins

}

void	WiiManager::SetUp3DProjection()
{
	f32 w = GetScreenWidth();
	f32 h = GetScreenHeight();
	Mtx44 perspective;
	float Fov = 45.0f;
	float aspect = w/h;
	guPerspective(perspective, Fov, aspect, 1.0f, 50000.0f);

	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

    GX_InvalidateTexAll();
	GX_InvVtxCache ();

	GX_ClearVtxDesc();							// not really needed here
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	//GX_VTXFMT0 used for lines
    GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);   
	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); 
	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

	//GX_VTXFMT1 used for ploy textures
	GX_SetVtxAttrFmt (GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	//GX_VTXFMT2 used for ploy textures NO ALPHA
	GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_POS, GX_POS_XY, GX_S16, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_CLR0, GX_CLR_RGB, GX_RGB565, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT2, GX_VA_TEX0, GX_TEX_ST, GX_U16, 0);

	//GX_VTXFMT3 used for test colour polys
	GX_SetVtxAttrFmt (GX_VTXFMT3, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT3, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	
	//GX_VTXFMT4 used for tex with normals
    GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);   // Each call to a vertex attribute must match the order: Position, normal, color, texcoord. 
	GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT4, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	//GX_VTXFMT5 used for tex
    GX_SetVtxAttrFmt (GX_VTXFMT5, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt (GX_VTXFMT5, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT5, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	//GX_VTXFMT6 used for test colour polys
	GX_SetVtxAttrFmt (GX_VTXFMT6, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT6, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_TRUE);

    GX_SetNumChans(1);
    GX_SetNumTexGens(1);

    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	
	GX_SetAlphaUpdate(GX_TRUE); 

    GX_SetCullMode(GX_CULL_BACK);//GX_CULL_NONE
	 GX_SetCullMode(GX_CULL_NONE);
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

	++m_uScreenBufferId; // usig this method will allow for future changes i.e triple buffering 
	if (m_uScreenBufferId >= GetMaxScreenBuffers()) 
		m_uScreenBufferId=0;

	// calling 'VIDEO_WaitVSync' at the incorrect time can cause screen tearing when the hardware flips
	VIDEO_WaitVSync(); //must wait here for a full VBL before the next frame is set

	VIDEO_SetNextFramebuffer( GetCurrentFrame() );
	VIDEO_Flush();
}

// for debugging - its a fudge, use it for debugging some amounts of text or variables
void WiiManager::Printf(int x, int y, const char* pFormat, ...) 
{
	static const u32 BufferSize(128);

	va_list tArgs;
	va_start(tArgs, pFormat);
	char Buffer[BufferSize+1];
	vsnprintf(Buffer, BufferSize, pFormat, tArgs);	
	va_end(tArgs);

	Util3D::Trans(-320,-240);  // fudge as is just for debug text, fixed camera view 0,0 needed
	//GetFontManager()->DisplayLargeText(Buffer,x,y);
	GetFontManager()->DisplaySmallText(Buffer,x,y);
}

void WiiManager::CreateSettingsFromXmlConfiguration(std::string FileName)
{
	TiXmlDocument doc( FileName.c_str() );
	if ( doc.LoadFile() )
	{
		//map<string,int> VariablesContainer;

		TiXmlHandle docHandle( &doc );
		TiXmlHandle Data( docHandle.FirstChild( "Data" ) );

		// do we have a valid 'data' root
		if (Data.Element() != NULL)
		{
			// For things like;
			// <Variables><AmountStars>3000</AmountStars> ... </Variables>
			//

			TiXmlElement* pChild =  Data.FirstChild( "Variables" ).FirstChildElement().ToElement();
			for( TiXmlElement* pElem(pChild); pElem!=NULL; pElem=pElem->NextSiblingElement() )
			{
				const char* const pKey(pElem->Value());
				const char* const pText(pElem->GetText());
				if (pKey && pText) 
				{
					printf("%s:%s",pKey,pText ); ///debug
					m_VariablesContainer[(HashLabel)pKey] = atof( pText ) ;
				}
			}

			//// For things like;
			//// <Graphics FileName="new.tga">
			////	 <AddImage Name="ShipFrames"  StartX="0" StartY="0"	ItemWidth="32" ItemHeight="32" NumberItems="4"/>
			////	 ... </Graphics>
			////

			//std::string IconSetFileName = Data.FirstChild( "Graphics" ).ToElement()->Attribute("FileName");

			//TiXmlElement* pGraphics =  Data.FirstChild( "Graphics" ).FirstChildElement().ToElement();
			//for( TiXmlElement* pGraphicsElem(pGraphics); pGraphicsElem!=NULL; pGraphicsElem=pGraphicsElem->NextSiblingElement() )
			//{
			//	string Key(pGraphicsElem->Value());
			//	if (Key=="AddImage") 
			//	{
			//		FrameInfo Info;
			//		string  Name =	pGraphicsElem->Attribute("Name");
			//		pGraphicsElem->Attribute("StartX",&Info.iStartX);
			//		pGraphicsElem->Attribute("StartY",&Info.iStartY);
			//		pGraphicsElem->Attribute("ItemWidth",&Info.iItemWidth);
			//		pGraphicsElem->Attribute("ItemHeight",&Info.iItemHeight);
			//		pGraphicsElem->Attribute("NumberItems",&Info.iNumberItems);
			//		Info.Name = Name;
			//		Info.FileName = IconSetFileName;

			//		m_FrameinfoContainer[ (HashLabel)Name ] = Info;
			//	}
			//}

			//// *** Sounds ***

			//TiXmlElement* pSounds =  Data.FirstChild( "Sounds" ).FirstChildElement().ToElement();
			//for( TiXmlElement* pGraphicsElem(pSounds); pGraphicsElem!=NULL; pGraphicsElem=pGraphicsElem->NextSiblingElement() )
			//{
			//	string Key(pGraphicsElem->Value());
			//	if (Key=="AddSound") 
			//	{
			//		SoundInfo Info;
			//		Info.LogicName = pGraphicsElem->Attribute("Name");
			//		Info.FileName = pGraphicsElem->Attribute("FileName");
			//	//	printf ((Info.Name +"  " +Info.FileName).c_str());
			//		m_SoundinfoContainer.push_back( Info );
			//	}
			//}

			// *** Fonts ***

			TiXmlElement* pFonts =  Data.FirstChild( "Fonts" ).FirstChildElement().ToElement();
			for( TiXmlElement* pFontElem(pFonts); pFontElem!=NULL; pFontElem=pFontElem->NextSiblingElement() )
			{
				string Key(pFontElem->Value());
				if (Key=="AddFont") 
				{
					FontInfo Info;
					Info.LogicName = pFontElem->Attribute("Name");
					Info.FileName = pFontElem->Attribute("FileName");
					printf ((Info.LogicName +"  " +Info.FileName).c_str());
					m_FontinfoContainer.push_back( Info );
				}
			}
		}
		else
		{
			if (docHandle.FirstChildElement().Element() == NULL)
				printf("The root label is missing");
			else
				printf("The root is not labeled <Data>");
		}

		printf("XML Setttings complete");
	}

	// Check the XML for any obvious mistakes
	static const std::string ErrorString = "Misssing %s in the XML";
	if (m_VariablesContainer.empty()) ExitPrintf(ErrorString.c_str(),"variables");
//	if (m_SoundinfoContainer.empty()) ExitPrintf(ErrorString.c_str(),"sound");
	if (m_FontinfoContainer.empty()) ExitPrintf(ErrorString.c_str(),"fonts");

}
	
void WiiManager::InitDebugConsole(int ScreenOriginX, int ScreenOriginY)
{	
#ifndef BUILD_FINAL_RELEASE
	//noticed 'InitDebugConsole' clears the given buffer with black

	// Initialise the console, required to see the output of printf onscreen
	console_init(	m_pFrameBuffer[0],
					ScreenOriginX,
					ScreenOriginY,
					640,480,
					//m_pGXRMode->fbWidth,
					//m_pGXRMode->xfbHeight,
					m_pGXRMode->fbWidth * VI_DISPLAY_PIX_SZ);

	VIDEO_ClearFrameBuffer (m_pGXRMode, m_pFrameBuffer[0], COLOUR_FOR_WIPE );  // put back the orginal wipe
	//////CON_InitEx(GetGXRMode(), ScreenOriginX,ScreenOriginY,m_pGXRMode->fbWidth,m_pGXRMode->xfbHeight);

	//printf("InitDebugConsole\n\n");
#endif
}




void WiiManager::ProgramStartUp(std::string FullFileName)
{
	CreateSettingsFromXmlConfiguration(FullFileName);
	
	for ( vector<FontInfo>::iterator FontInfoIter( GetFontInfoBegin());	FontInfoIter !=  GetFontInfoEnd() ; ++FontInfoIter )
	{
		// Fonts, loads files '.ftab' (dimensions) + '.fraw' (data)
		GetFontManager()->LoadFont(WiiFile::GetGamePath() + FontInfoIter->FileName, FontInfoIter->LogicName);
	}
}