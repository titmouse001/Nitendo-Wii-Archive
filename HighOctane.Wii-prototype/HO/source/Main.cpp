#include "Singleton.h"
#include "WiiManager.h"
#include "Image.h"
#include "debug.h"
#include "ImageManager.h"
#include "InputDeviceManager.h"
#include "FontManager.h"
#include "WiiFile.h"
#include "MapManager.h"
#include "SpriteManager.h"
#include "vehicle.h"
#include "EFB.h"
#include "LWOSource/lwo2.h"
#include "Terrain.h"

#include "Utilities.h"

#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
//-------------------------------------------------
// High Octane for the wii - Paul Overy - 2009/2010
//-------------------------------------------------

int  PowerOffMode(SYS_POWEROFF);  
bool bEnablePowerOffMode(false); //don't use "-1" as a 'PowerOffMode' state - you never know what may change, just incase
void Trigger_Reset_Callback(void)			{ PowerOffMode = SYS_RESTART;  bEnablePowerOffMode=true; } 
void Trigger_Power_Callback(void)			{ PowerOffMode = SYS_POWEROFF; bEnablePowerOffMode=true; }  
void Trigger_PadPower_Callback( s32 chan )	{ PowerOffMode = SYS_POWEROFF; bEnablePowerOffMode=true; } 

void DEBUG_Text(Vehicle& vehicle1);

//void DoOnExit() { }


double diff(1);  //debug
u32 Frames;


float calculate_distance (float x1,float y1,float x2 ,float y2)
{
	float distance_x = x1-x2;
	float distance_y = y1- y2; 
	return sqrt( (distance_x * distance_x) + (distance_y * distance_y));
}



int main(int argc, char **argv) 
{	
	//atexit (DoOnExit);

	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );



	SYS_SetResetCallback(Trigger_Reset_Callback); 
    SYS_SetPowerCallback(Trigger_Power_Callback); 
	WPAD_SetPowerButtonCallback( Trigger_PadPower_Callback );
 

	printf("   Waiting for wiiMote");
	while ( Wii.GetInputDeviceManager()->IsWiiMoteReady(0) )
	{
		VIDEO_WaitVSync();
	};

	//======================================================
	//Wii.GetFontManager()->LoadFont("High_Octane/fonts/Arial-latin");
	Wii.GetFontManager()->LoadFont(WiiFile::GetGamePath() + "fonts/Bauhaus93-latin");	
	//======================================================

	ImageManager* pImageManager(Wii.GetImageManager());
	pImageManager->AddImage(WiiFile::GetGamePath() + "Blocks.tga",0,0,16,16);

	//======================================================
	Wii.GetMapManager()->GetMap(WiiFile::GetGamePath() + "map4.map");
	Wii.GetMapManager()->SetCurrentWorkingMapLayer(WiiFile::GetGamePath() +  "map4.map");
	//======================================================

	SpriteManager* pSpriteManager(Wii.GetSpriteManager());

	if (pSpriteManager->BeginGraphicsFile(WiiFile::GetGamePath() + "carfrms1.tga"))
	{
		pSpriteManager->AddImage(0,0,32,32,64);
		pSpriteManager->EndGraphicsFile();
	}

	pSpriteManager->AddImage(WiiFile::GetGamePath() + "WHEELMAR.tga",0,0,32,32,64);

	//======================================================
	MapLayers* pMapLayers( Wii.GetMapManager()->GetCurrentWorkingMapLayer() );
	//======================================================

	Vehicle	vehicle1(444.0f,444.0f,0.0f);
	vehicle1.SetDirection(M_PI);

	time_t TimeStart;
	time_t Time;
	time ( &TimeStart );

	u32 LastPad(0);
	float turnAmount(0);

//	WPAD_ControlSpeaker(WPAD_CHAN_0, GX_TRUE);

	do 
	{
		//  ...  NEED To UPDATE EVERTHING TO USE 3d cam - i.e pre calc the whole map as a 3d grid and move camera arround it.
		// this will also cut down on points used (tri strips & things)- so maybe the whole thing can just be used without needing to removed unseen stuff.
		// probably easy to put just the needed bits of the map into the GX.  Maybe break up into sections - even if its just on on axis?

		if ( bEnablePowerOffMode )
		{
			SYS_ResetSystem( PowerOffMode, 0, 0 );
		}

		Wii.GetInputDeviceManager()->Store();
			
		vehicle1.Drive();

		u32 Pad = Wii.GetInputDeviceManager()->GetPadButton();
		
		if (Pad & WPAD_BUTTON_UP) 
		{
			static float amountleft = -(M_PI*2.0f)/64.0f;
			vehicle1.TurnDrivingDirection( amountleft * 0.02 );
		}
		if (Pad & WPAD_BUTTON_DOWN)
		{
			static float amountright = (M_PI*2.0f)/64.0f;
			vehicle1.TurnDrivingDirection(  amountright * 0.02 );
		}

		if (Pad & WPAD_BUTTON_2)
		{
			vehicle1.AddSpeed(0.05);
		}

		static Vector lastpos;
		Vector v = vehicle1.GetPosition();
		float diffx = calculate_distance(v.x,v.y,lastpos.x,lastpos.y);

		static float counter (0);
		counter += diffx;
		lastpos = vehicle1.GetPosition();

		static bool presseddown=false;
		if (Pad & WPAD_BUTTON_1)
		{	 
			if ((counter>8) || (counter<-8))
			{
				Utilities::TireMark(vehicle1);
				counter=0;
			}

			if (vehicle1.GetSpeed()>0)
			{
				
				vehicle1.AddSpeed(-0.1);
				if (vehicle1.GetSpeed()<0)
					vehicle1.SetSpeed(0);

				presseddown=true;
			}
			else if (!presseddown)
			{
				vehicle1.AddSpeed(-0.025);
			}
		}
		else
		{
			presseddown=false;
		}


		//if (WPAD_ButtonsUp(0) & WPAD_BUTTON_LEFT)//  && once == false)
		//{
		//	Utilities::TireMark(vehicle1);
		//}

		// === CAMERA VIEW
		Map* pMapSolid(pMapLayers->GetSolidLayer());

		Mtx cameraMatrix, FinalMatrix,TransMatrix; 
		guMtxIdentity(TransMatrix);  

		// FOV set at 90 , giving a full left & right view. 
		// As the camera height is known the other 45deg length must be the same.
		// Note: Camera looks up to flip the view so it will work the same way as the 2d view.

		f32 CamX =  pMapSolid->GetViewXpos();
		if (CamX<Wii.GetScreenWidth()/2) CamX=Wii.GetScreenWidth()/2;

		f32 CamY =  pMapSolid->GetViewYpos();
		if (CamY<Wii.GetScreenHeight()/2) CamY=Wii.GetScreenHeight()/2;


		guVector camera = { CamX, CamY, -Wii.GetScreenHeight()/2 }; 
		guVector up =	{0.0F, -1.0F, 0.0F};  
		guVector look	= {CamX, CamY, 0.0F};
		guLookAt(cameraMatrix, &camera,	&up, &look);

		guMtxTransApply(TransMatrix,TransMatrix,0, 0,0);	// origin
		guMtxConcat(cameraMatrix,TransMatrix,FinalMatrix);
		GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0);     
		//===



		LastPad = Pad;

		//-----------------------
		// scroll working layers
		for (vector<Map*>::iterator pMap(pMapLayers->GetBegin()); pMap != pMapLayers->GetEnd(); ++pMap)
		{
			if ((*pMap)->GetMapType() != eMapIsHit)
			{
				// Do this only if the layer supports scrolling 
				(*pMap)->SetView(vehicle1.GetPosition().x, vehicle1.GetPosition().y);
			}
		}

		//----------------------------------------------
		// background & layers
		Wii.DrawScene( pMapLayers );
		//----------------------------------------------
		// test car
		vehicle1.Draw();
		//----------------------------------------------
		// debug section only
		DEBUG_Text(vehicle1);
		//----------------------------------------------

		Wii.SwapScreen();  // to clear zbuffer keep GX_SetZMode on until after this call 
		GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);

		time ( &Time );
		diff = difftime (Time,TimeStart);
		if (diff<1.0f) diff=1.0f;
		++Frames;

	} while( (WPAD_ButtonsUp(0) & WPAD_BUTTON_HOME)== 0 );

	
	//------------------------------------------------------
	Wii.ResetSwapScreen();
	time_t TimeEnd;
	time ( &TimeEnd );
	diff = difftime (TimeEnd,TimeStart);
	if (diff<1.0f) diff=1.0f;
	printf ( "\n\t\t%s", asctime (localtime ( &TimeEnd )) );
	ExitPrintf("Resolution:%dx%d",Wii.GetScreenWidth(),Wii.GetScreenHeight());
	//------------------------------------------------------

	return 0;
}



void DEBUG_Text(Vehicle& vehicle1 )
{

	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );

	Vector Vehicle = vehicle1.GetPosition();

//	if (pTempImage3x3!=NULL)
//	pTempImage3x3->DrawImage(111,111);

	//	int Size2 = GX_GetTexBufferSize(64, 64, GX_TF_RGBA8, GX_FALSE, 0);
//	char text[128];
//	sprintf(text,"%2.1f",  ((vehicle1.GetDirection() / (M_PI*2.0f) )*64 ) );
//	Wii.GetFontManager()->GetFont()->DisplayText(text,Vehicle.x,Vehicle.y-20);


	//// kind of a CPU/GPU amount used test
	//char text[128];
	//sprintf(text,"%d",VIDEO_GetCurrentLine() );
	//Wii.GetFontManager()->GetFont()->DisplayText(text,Vehicle.x-250,Vehicle.y+50);

	//if (diff!=0)
	//{
	//	char text[128];
	//	sprintf(text,"%2.1ffps", 60.00) ;//)(float)(Frames/60.0f) / (diff/60.0f));
	//	Wii.GetFontManager()->GetFont()->DisplayText(text,Vehicle.x-250,Vehicle.y-50);
	//}

		//for (vector<Map*>::iterator Iter(pMapLayers->GetBegin()); Iter != pMapLayers->GetEnd(); ++Iter)
		//{
		//	if ((*Iter)->GetMapType() != eMapIsHit)
		//	{
		//		u32 OverBlockID = ( (*Iter)->GetValueFromOrigin(vehicle1.GetPosition().x,vehicle1.GetPosition().y));
		//		char text[128];
		//		sprintf(text,"%s %04d %d",(*Iter)->GetMapName(), OverBlockID, pMapLayers->GetHitMap()->GetValueByIndex(OverBlockID) ); //(*Iter)->GetValueByIndex( OverBlockID ) );
		//		Wii.GetFontManager()->GetFont()->DisplayText(text,20,300 + (std::distance(pMapLayers->GetBegin(),Iter)*28) );
		//	}
		//}


}




// works ... change resolutions

#if 0
			if (once == true)
			{

				// turn off video
				VIDEO_SetBlack(TRUE);
				VIDEO_Flush();

				// clear screeen
				VIDEO_ClearFrameBuffer (Wii.GetGXRMode(), Wii.GetCurrentFrame(), COLOR_BLACK);  
				VIDEO_WaitVSync();
				VIDEO_SetNextFramebuffer( Wii.GetCurrentFrame() );	
				VIDEO_Flush();  
				VIDEO_WaitVSync();

				// setup new resolution & initialise all 
				GXRModeObj* pMode = Wii.GetGXRMode();
				if (pMode == &TVEurgb60Hz240DsAa)
					Wii.InitialiseVideo( VIDEO_GetPreferredMode(NULL) );
				else
					Wii.InitialiseVideo( &TVEurgb60Hz240DsAa );	// reinit the new screen format

				Wii.InitGX();							// reinit

				//Wii.SetUp2DProjection();				// GX wiped, reinit the lot
				Wii.SetUp3DProjection();

				Wii.GetImageManager()->RefreshImages();	// not really needed 

				VIDEO_SetBlack(FALSE);
				VIDEO_Flush();
	
				once = false;
			}
#endif




//static void	copy_buffers(u32 unused);
//VIDEO_SetPostRetraceCallback(copy_buffers);

//static void	copy_buffers(u32 count __attribute__ ((unused)))
//{
//	if (readyForCopy==GX_TRUE) {
//		GX_SetZMode(GX_TRUE, GX_LEQUAL,	GX_TRUE);
//		GX_SetColorUpdate(GX_TRUE);
//		GX_CopyDisp(frameBuffer,GX_TRUE);
//		GX_Flush();
//		readyForCopy = GX_FALSE;
//	}
//}




// http://gxr.brickmii.com/Tutorial/Setup/
//
//
//
//
//
//Setup
//
//--------------------------------------------------------------------------------
//
//Framebuffers: 
//
//Embeded Frame Buffer (efb)- internal memory used to draw graphics 
//External Frame Buffer (xfb) - the memory that your display will show the users 
//
//
//--------------------------------------------------------------------------------
//VIDEO_Init(); //set up basics of video properties 
//rmode = VIDEO_GetPreferredMode(NULL); //store preferred settings
//VIDEO_Configure(rmode); //give the video the settings
//frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode)); //frames to draw on
//frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode)); //...
//VIDEO_SetNextFramebuffer(frameBuffer[0]); //say you will draw on this frame during next draw step
//VIDEO_SetBlack(FALSE); //stop showing black, and show what we draw
//VIDEO_Flush(); //accept the changes we have said (set black, configure, ect)
//
//There are two main parts to setting up a GX application.:
//The first of these is to set allocate room for and set up the memory and video setting with the height and width of the display you will be drawing on. This usually entails getting the preffered configuration from the system, and making a call to Video Initialization. 
//
//One of the instructions you will need to use is the VideoBlack(False) setting. It took me a bit to exactly what this did, but it is the control that actually turns the display to the TV off and on. If it is set to True, the tv will black out and own't be told to display anything at all, and set to False allows you to draw. You may see uses for this in the future if you want to do lots of stuff when the applicaiton first starts up. 
//
//
//--------------------------------------------------------------------------------
//void *gp_fifo = NULL; //pointer to the buffer 
//gp_fifo = memalign(32,DEFAULT_FIFO_SIZE); //Make a buffer to hold GX instructions, 32bit aligned
//memset(gp_fifo,0,DEFAULT_FIFO_SIZE); //Make sure it is all empty (fill it with 0's)
//GX_Init(gp_fifo,DEFAULT_FIFO_SIZE); //Call GX_Init to set defaults for everything and extra setup stuff 
//
//Info from YAGCD 5.11
//The second operation is setting up GX. This requires allocating another section of meemory for something called GX FIFO. This is a section of memory that will store the information you are passing to the GX chip, such as vertex positions, colors, and other information. Later, I will show you how you can have fun with this by capturing the FIFO data in display lists and replaying the input to make things go faster (reduce CPU time to make models and such to almost 0 by sending large packs of raw vertex/color/ect info). You will then call GX Initialization, which will set additional default settings, and will set GX registers to default values. This will turn off vertex descriptions, set TEV operations to normals (no special color tricks), and in general turn off anything it can possibly find. 
//
//Since everything is turned off, you need to turn on everything you need. Two primary things you will need to configure are: the display buffers you made(part 1) and the FIFO settings for rendering on these(part 2). You can think of these as: You've made a canvas, and now must configure how you will be painting on it. 
//
//
//--------------------------------------------------------------------------------
//GXColor background = {0, 0, 0, 0xff}; //a color for the background (solid black with full alpha (can't see through it)
//GX_SetCopyClear(background, 0x00ffffff); //every copy to the screen, set color to 'background' and zbuffer to 0x00ffffff
//
//One of these settings is what should happen when you first start using the buffer, what should it look like? Since a lot of 3d things change every frame, it is often good to clear the buffer. This line says what it should be cleared to. I have not tried turning this off yet... it might be just setting the alpha to 0. I'd rather use a texture to store previous screens instead.
//The default is; 
//GX_SetCopyClear((GXColor){64,64,64,255},0x00ffffff); in setup, so you wouldn't have full black
//These are GX_DEFAULT_BG and GX_MAX_Z24 respectively. 
//
//
//--------------------------------------------------------------------------------
//GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1); 
//What area should GX be writing to on it's canvas? yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight); 
//if the GX canvas is different in height than what you are rendering to the TV xfbHeight = GX_SetDispCopyYScale(yscale); 
//make is so that the TV looks like the canvas (useful if not 480p or anything) GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight); 
//What area should be rendered to on the frame? You can use this to do some tricks, like split screen and such 
//There are actually three video related paths that you might want to keep track of. There is GX->efb, where GX will write to it's semi-personal framebuffer. A second path is efb->xfb, where you tell the system to copy the GX's buffer to a more public buffer, and finally there is xfb->TV, a process handled by the System library. The code above handles a couple parts of this. The viewport deals with GX->efb, saying where the top/left, and width/height (near/far) parts of the view are. I have not messed with the near/far, as you can manipulate the projection settings (later) to do something in GX that achieves better results. 
//
//Scaling is needed since your efb might be larger than your destination xfb. This can happen for a couple reasons, the main one I've thought is the true height of the xfb made with the config was 240 and the efb was 480, but this line seems to be pretty stable. Should you find other settings than this that suit your needs, feel free to use those. The scissors are a subset of the viewport. Viewport says how big your canvas is, scissors tell how much of that canvas you wish to draw on at the time. You might want to render a tiny patch of another MapManager on top of your current one (picture in picture), or do split screen, or something of this nature. In thos cases, at the end you would set the scissors to the area that you want to draw on, draw the stuff, and then set them back to normal (or another area) for the next frame. 
//The default is:
//GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
//GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
//GX_SetDispCopyYScale(1.0);
//
//
//
//--------------------------------------------------------------------------------
//GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
//GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
//GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
//GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));
//
//This section is setting the way that you copy from the GX canvas to the frame buffer for drawing. Once again, this is usually setup to handle different sized framebuffers than the size of the efb. Because of this, the display copy scale from above is used to set the height. The field rendering mode is set to make sure that you are drawing either every other, or every line depending on if interlace or not is enabled. Most people draw 480 when screens can be 240, so the screen will draw every other line to emulate a full 480 in some cases. Copy filter is allowing for anti-alias or other features which may make the screen look better (but sometimes you may want to turn off).
//default is:
//GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
//GX_SetDispCopyDst(rmode->fbWidth,rmode->efbHeight);
//GX_SetCopyFilter(GX_FALSE,NULL,GX_FALSE,NULL);
//GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE)) 
//
//
//--------------------------------------------------------------------------------
//GX_SetCullMode(GX_CULL_NONE); //Which polygons should be removed? 
//GX_CopyDisp(frameBuffer[fb],GX_TRUE); //Draw the first frame from efb->xfb,clearing efb
//GX_SetDispCopyGamma(GX_GM_1_0); //How much gamma should be done?
//
//The setup is almost complete. These lines introduce a couple tweaks. The act of culling is done to help reduce GX's need to draw so much. This becomes more practical and useful as you start loading models, doing complicated MapManagers, and doing more 3d tricks on the system. The options that you have are GX_CULL_BACK, GX_CULL_NONE, GX_CULL_FRONT, and GX_CULL_ALL. Culling back means that polygons facing backwards aren't drawn. Culling front means that polygons facing forwards aren't drawn. All means that no polygons are drawn (I forgot why this is ever used, maybe for creating zbuffer entries). If you have a modeling program, it should allow you to made the models draw faces in the correct order to make them visible. If you don't care about 3d models, set this to NONE for now. I think it defaults to BACK.
//
//CopyDisplay will be called whenever you want to copy efb->xfb and make it displayable. The second parameter says whether or not to clear the efb when the copy is done. This is sometimes done for either the non-active buffer or both buffers during setup to make sure they are empty (since GX does not clear a buffer when it starts drawing). If you are doing a loop, this will be 1/30th of a second they were dirty, so if you forget this step, don't worry. I'm not sure about the copy gamma, but if you want to try the other settings, they are GX_GM_1_7, and GX_GM_2_2. This right now is the default, so if you don't call it, GX_Init will set it to GX_GM_1_0 
//
//
//--------------------------------------------------------------------------------
//f32 w = rmode->viWidth; 
//f32 h = rmode->viHeight;
//guPerspective(perspective, 45, (f32)w/h, 0.1F, 300.0F);
//GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);




///****************************************************************************
// * StopGX
// *
// * Stops GX (when exiting)
// ***************************************************************************/
//void StopGX()
//{
//        GX_AbortFrame();
//        GX_Flush();
//
//        VIDEO_SetBlack(TRUE);
//        VIDEO_Flush();
//}
//



////////////static GXColor litcolors[] = {
////////////        { 0xff, 0xff, 0xff, 0xFF }, // Light color 1
////////////        { 0x61, 0x61, 0x61, 0xFF }, // Ambient 1
////////////        { 0x88, 0x88, 0x88, 0xff}  // Material 1
////////////};

////////#include <math.h>
////////
////////void setlight(Mtx view,u32 theta,u32 phi,GXColor litcol, GXColor ambcol,GXColor matcol)
////////{
////////	guVector lpos;
////////	f32 _theta,_phi;
////////	GXLightObj lobj;
////////
////////	_theta = (f32)theta*M_PI/180.0f;
////////	_phi = (f32)phi*M_PI/180.0f;
////////
////////	static float xx(0.1);
////////
////////	lpos.x = 500.0f * cosf(_phi) * sinf(_theta);
////////	lpos.y = 500.0f * sinf(_phi);
////////	lpos.z = 500.0f * cosf(_phi) * cosf(_theta);
////////
////////	guVecMultiply(view,&lpos,&lpos);
////////
////////	GX_InitLightPos(&lobj,lpos.x,lpos.y,lpos.z);
////////	GX_InitLightColor(&lobj,litcol);
////////	GX_LoadLightObj(&lobj,GX_LIGHT0);
////////	
////////	// set number of rasterized color channels
////////	GX_SetNumChans(1);
////////    GX_SetChanCtrl(GX_COLOR0A0,GX_ENABLE,GX_SRC_REG,GX_SRC_REG,GX_LIGHT0,GX_DF_CLAMP,GX_AF_NONE);
////////    GX_SetChanAmbColor(GX_COLOR0A0,ambcol);
////////    GX_SetChanMatColor(GX_COLOR0A0,matcol);
////////}
////////

	//////////
	//////////	
	//////////		guMtxIdentity(TransMatrix);  
	//////////		static float aaa(0);
	//////////		guMtxRotRad(TransMatrix,'x', aaa+=0.05) ;//M_PI/2);
	//////////		if (aaa>M_PI*2) aaa = M_PI*2 - aaa;
	//////////		guMtxTransApply(TransMatrix,TransMatrix, 300, 300,-80);	// origin
	//////////		guMtxConcat(cameraMatrix,TransMatrix,FinalMatrix);
	//////////
	//////////		
	//////////		GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0);     
	//////////
	//////////			setlight(cameraMatrix,2,12,litcolors[0],litcolors[1],litcolors[2]);
	//////////
	////////////===
	//////////
	//////////	GX_ClearVtxDesc();
	//////////	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	//////////	GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
	//////////  //  GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	//////////    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	//////////
	//////////	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE); 
	//////////	GX_LoadTexObj(&(Wii.GetImageManager()->GetImage(222)->m_HardwareTextureInfo), GX_TEXMAP0);   
	//////////	
	//////////
	//////////	GX_SetZMode (GX_TRUE, GX_LEQUAL, GX_TRUE);
	//////////	//GX_SetPointSize(10,GX_TO_ONE);
	//////////	st_lwLayer* pLayer = obj->layer;
	//////////
	//////////	GX_Begin(GX_TRIANGLES, GX_VTXFMT4,	pLayer->polygon.count*3 );
	//////////
	//////////	lwPoint* Points = pLayer->point.pt;
	//////////	
	//////////	for (int i=0; i < pLayer->polygon.count ; i++)
	//////////	{
	//////////		lwPolygon* pPoly = &pLayer->polygon.pol[i];
	//////////
	//////////		lwPoint* p1 = &Points[ pPoly->v[0].index ];
	//////////		lwPoint* p2 = &Points[ pPoly->v[1].index ];
	//////////		lwPoint* p3 = &Points[ pPoly->v[2].index ];
	//////////
	//////////		u8 red = pPoly->surf->color.rgb[0]* 255.0;
	//////////		u8 green = pPoly->surf->color.rgb[1]* 255.0;
	//////////		u8 blue = pPoly->surf->color.rgb[2]* 255.0;
	//////////
	////////////ExitPrintf("%f %f %f",pLayer->polygon.pol->surf->color.rgb[0],pLayer->polygon.pol->surf->color.rgb[1],pLayer->polygon.pol->surf->color.rgb[2]);
	//////////
	//////////		static const float size = 500;
	//////////
	//////////		GX_Position3s16(p1->pos[0]*size,p1->pos[1]*size, p1->pos[2]*size); 
	//////////		GX_Normal3f32(pPoly->v[0].norm[0], pPoly->v[0].norm[1], pPoly->v[0].norm[2]);
	//////////		//GX_Color4u8(red,green,blue,255);
	//////////		GX_TexCoord2f32(0, 0);
	//////////
	//////////		GX_Position3s16(p2->pos[0]*size,p2->pos[1]*size, p2->pos[2]*size); 
	//////////		GX_Normal3f32(pPoly->v[1].norm[0], pPoly->v[1].norm[1], pPoly->v[1].norm[2]);
	//////////		//GX_Color4u8(red,green,blue,255);
	//////////		GX_TexCoord2f32(1, 0);
	//////////
	//////////		GX_Position3s16(p3->pos[0]*size,p3->pos[1]*size, p3->pos[2]*size); 
	//////////		GX_Normal3f32(pPoly->v[2].norm[0], pPoly->v[2].norm[1], pPoly->v[2].norm[2]);
	//////////		//GX_Color4u8(red,green,blue,255);
	//////////		GX_TexCoord2f32(1, 1);
	//////////	}
	//////////	GX_End();
	//////////
	//////////
	//////////
	//////////////////////////// DISABLE LIGHT/////////////////////////////
	//////////    GX_SetNumChans(1);
	//////////    GX_SetChanCtrl(
	//////////        GX_COLOR0A0,
	//////////        GX_DISABLE,  // disable channel
	//////////        GX_SRC_VTX,  // amb source
	//////////        GX_SRC_VTX,  // mat source
	//////////        0,           // light mask
	//////////        GX_DF_NONE,  // diffuse function
	//////////        GX_AF_NONE);
	//////////    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	///////////////////////////////////////////////////////////////////////////
	//////////
	//////////	//ExitPrintf("");
	//////////			//Vtx* pVtx(Wii.GetInputDeviceManager()->GetIRPosition());
	//////////		//if (pVtx != NULL)
	//////////		//{
	//////////		//	Wii.GetImageManager()->DrawLineStrip(Wii.GetInputDeviceManager()->GetIRPositionContainer(),0xf000000);
	//////////		//}



// WIIMOTE SOUND!!!
			//////u8 buff[]	ATTRIBUTE_ALIGN(32)	= {
			//////						1,  // needed?
			//////						0x18, // what about this?
			//////						8<<3,	// amount<<3... but why not silence
			//////						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }; 

			//////	WPAD_SendStreamData(0,&buff,sizeof(buff) / sizeof(buff[0]) );
