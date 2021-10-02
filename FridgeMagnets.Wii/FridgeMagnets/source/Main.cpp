// Fridge Magnets for the wii - Paul Overy - 2010

// This code is one big fudge - I've frigged some code together from another game I'm working on, removing parts & hacking the rest.
// It's to be noted that the display side of things is far more frigged than the rest, as the code was taken from a 
// game in progress that was never intended for this kind of thing.

#include <gccore.h>
#include <string.h>
#include "Singleton.h"
#include "WiiManager.h"
#include "Image.h"
#include "debug.h"
#include "ImageManager.h"
#include "InputDeviceManager.h"
#include "FontManager.h"
#include "WiiFile.h"
#include "EFB.h"
#include "Util.h"
#include "FridgeMagnetManager.h"
#include "Config.h"

//#include "libpng/pngu/pngu.h"


int  PowerOffMode(SYS_POWEROFF);  
bool bEnablePowerOffMode(false); 
void Trigger_Reset_Callback(void)			{ PowerOffMode = SYS_RESTART;  bEnablePowerOffMode=true; } 
void Trigger_Power_Callback(void)			{ PowerOffMode = SYS_POWEROFF; bEnablePowerOffMode=true; }  
void Trigger_PadPower_Callback( s32 chan )	{ PowerOffMode = SYS_POWEROFF; bEnablePowerOffMode=true; } 


void DoIntro();
void DoMain();

Image* pImage=NULL;

int main(int argc, char **argv) 
{	
	
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );  // backbone stuff

	srand ( time(NULL) ); // we don't want the same random sequences each time we start.


	SYS_SetResetCallback(Trigger_Reset_Callback); 
    SYS_SetPowerCallback(Trigger_Power_Callback); 
	WPAD_SetPowerButtonCallback( Trigger_PadPower_Callback );
 
	//======================================================
	Wii.GetFontManager()->LoadFont("fonts/CenturyGothic-latin"); 
	//======================================================

#ifdef BUILD_FINAL_RELEASE
	static const std::string Path("Sounds/");  
#else
	static const std::string Path("sd://apps/FridgeMagnets/Sounds/");  
#endif	

	Wii.GetSoundManager()->LoadSound(Path+"AMMOCLCK.WAV"); //0
	Wii.GetSoundManager()->LoadSound(Path+"click.wav");
	Wii.GetSoundManager()->LoadSound(Path+"G10.wav");
	Wii.GetSoundManager()->LoadSound(Path+"Whip.WAV");
	Wii.GetSoundManager()->LoadSound(Path+"G18.wav");
	Wii.GetSoundManager()->LoadSound(Path+"tap.wav");
	Wii.GetSoundManager()->LoadSound(Path+"FollowPointer.wav");  //6
	Wii.GetSoundManager()->LoadSound(Path+"Camera.wav");  //7

#ifdef BUILD_FINAL_RELEASE
	static std::string GfxPath("gfx/");  
#else
	static std::string GfxPath("sd://apps/FridgeMagnets/gfx/");  
#endif	

	ImageManager* pImageManager( Wii.GetImageManager() );


// code behind this is getting to complex - need to refactor to share common ground


//	if (pImageManager->BeginGraphicsFile(GfxPath + "AllHandsGrey.png"))
//	{
//		pImageManager->AddImage(0,0,64,96,3);
	//	pImageManager->EndGraphicsFile();
	//}

	pImageManager->AddImage(GfxPath + "AllHandsGrey.png",0,0,64,96,3);

	pImageManager->AddImage(GfxPath+"CameraIcon.png");
	Wii.m_CameraIcon =	Wii.GetImageManager()->GetImageCount() - 1;
	pImageManager->AddImage(GfxPath+"Wipe.png");
	pImageManager->AddImage(GfxPath+"Letters.png");

	//Wii.GetImageManager()->CutImageIntoParts(GfxPath+"AllHandsGrey.png",64,96,3);
	//Wii.GetImageManager()->StoreImage(GfxPath+"CameraIcon.png");
	//Wii.m_CameraIcon =	Wii.GetImageManager()->GetImageCount() - 1;
	//Wii.GetImageManager()->StoreImage(GfxPath+"Wipe.png");
	//Wii.GetImageManager()->StoreImage(GfxPath+"Letters.png");
	//================================================================================
	// Setup Fridge magnets and 3d view
	Wii.GetCamera()->InitialiseCamera();

	// Something to keep track of all the letters
	FridgeMagnetManager& Manager =  *(Wii.GetFridgeMagnetManager());

	std::vector<GXColor> ColoursAvailable;
	ColoursAvailable.push_back(  Util::Colour(219,6,165) ); //pink
	ColoursAvailable.push_back(  Util::Colour( 65,180,70 ) ); //dark green
	ColoursAvailable.push_back(  Util::Colour(41,161,240) ); //light blue
	ColoursAvailable.push_back(  Util::Colour(147,27,250) ); //purple
	ColoursAvailable.push_back(  Util::Colour(246,28,18 ) ); //red
	ColoursAvailable.push_back(  Util::Colour( 25,235,35 ) ); //light green
	ColoursAvailable.push_back(  Util::Colour(243,155,17 ) ); //orange
	ColoursAvailable.push_back(  Util::Colour(40,40,244 ) ); //dark blue
	ColoursAvailable.push_back(  Util::Colour(64,250,250 ) ); //
	ColoursAvailable.push_back(  Util::Colour(245,245,33) ); //yellow


	// Pick some colours from the abover list for these letters
	int i(0);
	// help the title screen use better sequence of colours (we can repeat character data - it's still only set once)
	static const std::string ProduceColoursForTheFollowing = "WelcomeToFridgeMagnetsbyPaulOveryetaoinshrdflcumwfgypbvkjxqzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-/=%?!£";
	for (string::const_iterator iter(ProduceColoursForTheFollowing.begin()); iter != ProduceColoursForTheFollowing.end(); ++iter )
	{
		if ( Manager.LetterColourContainer.find(*iter) == Manager.LetterColourContainer.end())
		{
			Manager.LetterColourContainer[*iter] = ColoursAvailable[  ++i % ColoursAvailable.size() ];
		}
	}

	//---------------------------------------------------------------
	// show intro welcome
#ifndef BUILD_FINAL_RELEASE
	Manager.FridgeMagnetText(-200,-180,"Debug Build",0.0f);
#endif
	Manager.FridgeMagnetText(-225,-120,	"Welcome",0.4f,2.0f);
	Manager.FridgeMagnetText( -15,-50,	"To",0.15f);
	Manager.FridgeMagnetText(-150,20,    "Fridge",0.45f,1.5f);
	Manager.FridgeMagnetText(-165,80,	"Magnets",0.45f,1.5f);
	Manager.FridgeMagnetText(0,135,	"by",0.15f,0.65f);
	Manager.FridgeMagnetText(40,165,	"Paul Overy",0.15f,0.65f);

	Wii.GetCamera()->InitialiseCamera();
	Wii.GetCamera()->SetCam(0,0,-240);
	Wii.GetCamera()->SetCameraView();

	DoIntro();

	////----------------------------------------------------------------
	// Main Part

	static const float CameraFactor (1.25f); // zoom out a bit for this main part
	Wii.GetCamera()->InitialiseCamera(CameraFactor);

	WPAD_SetVRes(WPAD_CHAN_ALL, Wii.GetGXRMode()->fbWidth * CameraFactor, Wii.GetGXRMode()->xfbHeight * CameraFactor );  // resolution of IR

	Manager.RemoveAllFridgeMagnets(); // clear all onscreen letters
	
	// add the new starting letters in random places.
	static const std::string Characters = "eeeetttaaaoooiinnsshhrrddffllccuummwwffggyyppbbvvkkjxqz";
	for (string::const_iterator iter(Characters.begin()); iter != Characters.end(); /*nop*/  )
	{
		u8 red = Manager.LetterColourContainer[ *iter ].r;
		u8 green = Manager.LetterColourContainer[ *iter ].g;
		u8 blue = Manager.LetterColourContainer[ *iter ].b;
		
		float rx( (40*CameraFactor) + rand()%(int)((Wii.GetScreenWidth()-80) * CameraFactor));
		float ry( (40*CameraFactor) + rand()%(int)((Wii.GetScreenHeight()-80) * CameraFactor));

		if (Manager.HitsFridgeMagnet(rx,ry) == NULL)  // avoid overlapping letters
		{
			Manager.AddNewFridgeMagnet(rx, ry, *iter)
				->SetColour(red,green,blue,0xff)
				->SetAngle( ( (float)rand()/RAND_MAX ) - 0.5f)
				->SetZ(-250 - (std::distance(Characters.begin(),iter)*25) );

			++iter;  // found an empty spot - now to the next letter
		}
	}



	////{
	////	float rx(34);//  + (Wii.GetScreenWidth() * CameraFactor));
	////	float ry(54) ;//+ (Wii.GetScreenHeight() * CameraFactor));
	////static const std::string Characters = "0123456789-+=";
	////for (string::const_iterator iter(Characters.begin()); iter != Characters.end(); /*nop*/  )
	////{
	////	u8 red = Manager.LetterColourContainer[ *iter ].r;
	////	u8 green = Manager.LetterColourContainer[ *iter ].g;
	////	u8 blue = Manager.LetterColourContainer[ *iter ].b;
	////	
	////		Manager.AddNewFridgeMagnet(rx,ry,&(*iter))
	////			->SetColour(red,green,blue,0xff)
	////			->SetAngle( ( (float)rand()/RAND_MAX ) - 0.5f)
	////			->SetZ(-1000-ry*2 );
	////		++iter; 
	////		ry+=31;
	////}
	////}

	//----------------------------------------------------------------
	

	DoMain();
	//----------------------------------------------------------------


	//---------------------------------------------------------------
	// show end message
	Wii.GetCamera()->InitialiseCamera();
	Wii.GetCamera()->SetCam(0,0,-320);
	Wii.GetCamera()->SetCameraView();
	Manager.RemoveAllFridgeMagnets(); // clear all onscreen letters and add the new starting letters in random places.
	for (int i(0); i<3200; ++i)
	{
		char a = (rand()%26) + 'a';
		std::string letter(1,a);
		Manager.FridgeMagnetText((1000-rand()%2000)*0.45,(500-(rand()%1000))*0.60, letter ,M_PI);
	}
	u32 Alpha=0;

	int Frames=60*5;
	for (std::vector<FridgeMagnet*>::iterator iter(Manager.GetBegin()); iter!=Manager.GetEnd(); ++iter)
	{
		FridgeMagnet* pFridgeMagnet(*iter);	
		pFridgeMagnet->SetZ( -( 305 + (rand()%1200)) );
		pFridgeMagnet->m_Velocity.z = (rand()%400)*0.01;
	}
	do // really need to do this with a timer to get round the 50, 60 fps thing
	{
		Wii.GetInputDeviceManager()->Store(); // can store movement history	if needed

		Manager.MotionLogic(0.045, 0.0f);
		Manager.DoMainDisplay();
			
		if (Frames<134)
		{
			Wii.GetImageManager()->Fade(Alpha); 
			Alpha+=2;
			if (Alpha>255) Alpha = 255;
		}

		Wii.SwapScreen();
		--Frames;
	} while( (Frames>0) && ((WPAD_ButtonsUp(0) | WPAD_ButtonsUp(1) | WPAD_ButtonsUp(2) | WPAD_ButtonsUp(3)) & WPAD_BUTTON_HOME) == 0 );

	//do // really need to do this with a timer to get round the 50, 60 fps thing
	//{
	//	Manager.DoMainDisplay();
	//	Wii.GetImageManager()->Fade(Alpha); 
	//	Wii.SwapScreen();
	//	Alpha+=3;
	//} while (Alpha<255);

	Wii.GetImageManager()->Fade(0xff); 
	Wii.SwapScreen();
	Wii.GetImageManager()->Fade(0xff); 
	Wii.SwapScreen();

	VIDEO_WaitVSync();
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();  

	//---------------------------------------------------------------

	return 0;
}

void DoMain()
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 
	FridgeMagnetManager& MagnetManager =  *(Wii.GetFridgeMagnetManager());

	Wii.SetSelectionMode( CompletedSelectionMode );  // reset as it may of been used in the intro

	Wii.AddMenu(Wii.m_CameraIcon+2,270,246-(64*3),"Letters"); 
	Wii.AddMenu(Wii.m_CameraIcon+0,270,246-(64*2),"ScreenShot"); 
	Wii.AddMenu(Wii.m_CameraIcon+1,270,246-(64*1),"Wipe"); 

	int timer=0;
	do
	{
		if ( bEnablePowerOffMode )
		{
			SYS_ResetSystem( PowerOffMode, 0, 0 );  
		}

		Wii.GetInputDeviceManager()->Store(); // can store movement history	if needed

		Wii.m_bHideAllMenuItems = false;  // each SelectLogic will set true if needed
		MagnetManager.SelectLogic(WPAD_CHAN_0);
		MagnetManager.SelectLogic(WPAD_CHAN_1);
		MagnetManager.SelectLogic(WPAD_CHAN_2);
		MagnetManager.SelectLogic(WPAD_CHAN_3);

		Wii.DoMenuLogic();

		MagnetManager.MotionLogic(0.22, 0.0f);

		Wii.GetCamera()->SetCameraView();  // aim the camera at target		

		MagnetManager.DoMainDisplay();

		Wii.DrawAllMenus();


		if (Wii.m_bTakeScreenShot)
		{
			Wii.SwapScreen();
			Wii.m_bTakeScreenShot = false;
			Util::TakeIncrementalScreenshot();
			Wii.SetEnableForAllMenus(true);
			timer=60*3;
		}

		if (timer>0)
		{
			--timer;
		
			Util::Translate(-Wii.GetScreenWidth()/2 + Wii.GetCamera()->GetCamX() + 15,
							-Wii.GetScreenHeight()/2 + Wii.GetCamera()->GetCamY()+ 15,0.8f,M_PI/2.0f);
			Wii.GetFontManager()->GetFont()->DisplayTextDebug( Util::GetIncrementalScreenshotFileName(true).c_str() ,0,0,220);
		}


		MagnetManager.DoDisplayAllControllers();

		Wii.SwapScreen();

	} while( ((WPAD_ButtonsUp(0) | WPAD_ButtonsUp(1) | WPAD_ButtonsUp(2) | WPAD_ButtonsUp(3)) & WPAD_BUTTON_HOME) == 0 );
}

void DoIntro()
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 
	FridgeMagnetManager& Manager =  *(Wii.GetFridgeMagnetManager());

	int ShowIntro=0;
	int Alpha=0xff;
	do
	{
		if ( bEnablePowerOffMode )
		{
			SYS_ResetSystem( PowerOffMode, 0, 0 );  
		}

		if ((WPAD_ButtonsDown(0)|WPAD_ButtonsDown(1)|WPAD_ButtonsDown(2)|WPAD_ButtonsDown(3)) & WPAD_BUTTON_A)
		{
			if ( ShowIntro < (60*2) )
				ShowIntro = (60*2)+1;
		}

		Wii.GetInputDeviceManager()->Store(); // can store movement history	if needed

		Wii.GetCamera()->SetCameraView();  // aim the camera at target		

		Wii.SetSelectionMode( CompletedSelectionMode );  // intro - force it

		Manager.SelectLogic(WPAD_CHAN_0);
		Manager.SelectLogic(WPAD_CHAN_1);
		Manager.SelectLogic(WPAD_CHAN_2);
		Manager.SelectLogic(WPAD_CHAN_3);

		if (ShowIntro > 60*2)
			Manager.MotionLogic(0.50f,FM_CAMZ);
		else
			Manager.MotionLogic(0.0f,FM_CAMZ);
		
		Manager.DoMainDisplay();

		Manager.DoDisplayAllControllers();


		Wii.GetImageManager()->Fade(Alpha); 
		Alpha-=5;
		if (Alpha<0) Alpha = 0; 

		Wii.SwapScreen();


		ShowIntro+=1;
	} while( ShowIntro<200);

}













//#include <mii.h>

// --- Mii info test


//#include <stdio.h>
//#include <stdlib.h>
//#include <wiiuse/wpad.h>
//#include <ogc/video.h>
//#include <mii.h>
//
//static void *xfb = NULL;
//static GXRModeObj *rmode = NULL;
//
//int run = 1;
//
//void init(){
//	VIDEO_Init();
//	WPAD_Init();
//	rmode = VIDEO_GetPreferredMode(NULL);
//	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
//	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
//	VIDEO_Configure(rmode);
//	VIDEO_SetNextFramebuffer(xfb);
//	VIDEO_SetBlack(FALSE);
//	VIDEO_Flush();
//	VIDEO_WaitVSync();
//	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
//	printf("\x1b[2;0H");
//}
//
//
//void showMii(Mii mii){
//	printf("Press Left and Right on the Wii mote to navigate through your Miis\n\n");
//
//	printf("Name: %s\n", mii.name);
//	printf("By: %s\n", mii.creator);
//
//	if (mii.female) printf("Gender: female\n");
//	else printf("Gender: male\n");
//
//	printf("Weight: %i\n", mii.weight);
//	printf("Height: %i\n", mii.height);
//
//	if (mii.favorite) printf("Mii is a Favorite\n");
//
//	if (mii.month>0 && mii.day>0)
//		printf("Birthday: %i/%i\n", mii.month, mii.day);
//
//	if (mii.downloaded) printf("Downloaded\n");
//
//	printf("Favorite Color: %i\n", mii.favColor);
//	
//	printf("Face is shape %i, color %i, with feature %i.\n", mii.faceShape, mii.skinColor, mii.facialFeature);
//	
//	if (mii.hairPart) printf("Hair is type %i, color %i, with a reversed part.\n", mii.hairType, mii.hairColor);
//	else printf("Hair is type %i, color %i, with a normal part.\n", mii.hairType, mii.hairColor);
//	
//	printf("Eyebrows are type %i, color %i, rotated %i, size %i, %i high, with %i spacing.\n", mii.eyebrowType, mii.eyebrowColor, mii.eyebrowRotation, mii.eyebrowSize, mii.eyebrowVertPos, mii.eyebrowHorizSpacing);
//	
//	printf("Eyes are type %i, color %i, rotated %i, size %i, %i high, with %i spacing.\n", mii.eyeType, mii.eyeColor, mii.eyeRotation, mii.eyeSize, mii.eyeVertPos, mii.eyeHorizSpacing);
//	
//	printf("Nose is type %i, size %i, and %i high.\n", mii.noseType, mii.noseSize, mii.noseVertPos);
//	
//	printf("Lips are type %i, color %i, size %i, and %i high.\n", mii.lipType, mii.lipColor, mii.lipSize, mii.lipVertPos);
//	
//	if(mii.glassesType>0) printf("Mii has color %i glasses of %i type and %i size and they are %i high.\n", mii.glassesType, mii.glassesColor, mii.glassesSize, mii.glassesVertPos);
//	
//	if(mii.mustacheType>0) printf("Mii has a type %i mustache that is size %i and is %i high. It is of color %i.\n", mii.mustacheType, mii.mustacheSize, mii.mustacheVertPos, mii.facialHairColor);
//	if(mii.beardType>0) printf("Mii has a type %i beard of %i color.\n", mii.beardType, mii.facialHairColor);
//
//	if (mii.mole) printf("Has mole in position %i, %i and it is of size %i.\n", mii.moleHorizPos, mii.moleVertPos, mii.moleSize);
//
//	printf("\n\nAll the above values are raw data that can be used for simple data display, data comparison, or to build a graphical representation of the mii using sprites of all the body parts!\n");
//}
//
//void clearScreen() {
//	printf("\033[2J");printf("\x1b[2;0H");
//}
//
//int main() {
//	init();
//
//	Mii * miis;
//
//	miis = loadMiis_Wii();
//
//	int n = 0;
//
//	showMii(miis[n]);
//
//	while(run) {
//		WPAD_ScanPads();
//		u32 pressed = WPAD_ButtonsDown(0);
//		if ((pressed & WPAD_BUTTON_RIGHT || pressed & WPAD_BUTTON_2 || 
//			pressed & WPAD_BUTTON_PLUS || pressed & WPAD_BUTTON_DOWN ||
//			pressed & WPAD_BUTTON_A) && n+1<NoOfMiis){
//			clearScreen();
//			n+=1;
//			showMii(miis[n]);
//		} else if ((pressed & WPAD_BUTTON_LEFT || pressed & WPAD_BUTTON_1 || pressed & WPAD_BUTTON_MINUS || pressed & WPAD_BUTTON_UP  || pressed & WPAD_BUTTON_B) && n>0) {
//			clearScreen();
//			n-=1;
//			showMii(miis[n]);
//		} else if (pressed & WPAD_BUTTON_HOME){
//			clearScreen();
//			printf("Goodbye!\n");
//			run = 0;
//		}
//		VIDEO_WaitVSync();
//	}
//	return 0;
//}

