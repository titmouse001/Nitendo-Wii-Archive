#include "Singleton.h"
#include <gccore.h>
#include <math.h>
#include "WiiManager.h"
#include "Image.h"
#include "ImageManager.h"
#include "InputDeviceManager.h"
#include "FontManager.h"
#include "Util.h"
#include "Util3D.h"
#include "debug.h"
#include "HashString.h"
#include "Config.h"
#include "Menu.h"
#include "MenuManager.h"
#include "MenuScreens.h"
#include "Timer.h"
#include "GameDisplay.h"
#include "UpdateManager.h"
#include "camera.h"
#include "Render3D.h"
#include "Draw_Util.h"

MenuScreens::MenuScreens() :  m_ZoomAmountForSpaceBackground(3.1f), m_pTimer(NULL)
{
}

void MenuScreens::Init()
{
	m_pWii = Singleton<WiiManager>::GetInstanceByPtr();
	m_pGameLogic = m_pWii->GetGameLogic();
	m_pTimer = new Timer;
}

void MenuScreens::SetTimeOutInSeconds(int Value)
{
	m_pTimer->SetTimerSeconds(Value);
}

bool MenuScreens::HasMenuTimedOut() 
{ 
	return m_pTimer->IsTimerDone(); 
}

void MenuScreens::DoMenuScreen()
{
	Util3D::CameraIdentity();

	WPAD_ScanPads();
	m_pWii->GetInputDeviceManager()->Store();
	Vtx* WiiMote( m_pWii->GetInputDeviceManager()->GetIRPosition() );

	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
	m_ZoomAmountForSpaceBackground+=0.001;
	m_pWii->GetSpaceBackground()->DrawImageXYZ(0,0, 9400,255,0,12.0f + (8.0f+(sin(m_ZoomAmountForSpaceBackground)*6.0f)) );

	static float spin=0.0f;
	spin+=0.01;
	m_pWii->GetSpaceBackground()->DrawImageXYZ(0,0, 9400,50,sin(spin)*0.15,13.0f + (8.0f+(sin(m_ZoomAmountForSpaceBackground)*6.0f)) );

	//=========================
	static float bbb = 0;
	bbb+=0.005f;
	//-------------------
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	m_pWii->GetCamera()->SetLightColour(255);
	m_pWii->GetCamera()->SetMaterialColour(0xF0);
	m_pWii->GetCamera()->SetAmbientLight(0x2F);
	m_pWii->GetCamera()->DoLight(-200.0f, -320.0f, -240.0f);
	//--------------------------

	Mtx Model,mat;
	orient_t Orientation;
	WPAD_Orientation(0, &Orientation);
	Mtx mat2;
	Mtx mat3;

	static float yaw  = 0; 
	static float pitch =0; 
	static float roll  = 0;

	// WiiMote Model
	yaw += (DegToRad(Orientation.yaw) - yaw )*0.15f;
	pitch += (DegToRad(Orientation.pitch) - pitch)*0.15f;
	roll += (DegToRad(Orientation.roll) - roll)*0.15f;
	guMtxRotRad(Model,'y', -M_PI/2 + yaw) ;
	guMtxRotRad(mat2,'x', M_PI + M_PI/16 - pitch );
	guMtxRotRad(mat3,'z', M_PI - roll) ;
	guMtxConcat(mat3,Model,Model);
	guMtxConcat(mat2,Model,Model);
	guMtxScaleApply(Model,Model,0.18f,0.18f,0.18f);
	guMtxTrans(mat, 35,12, -450);
	guMtxConcat(mat,Model,Model);
	guMtxConcat(m_pWii->GetCamera()->GetcameraMatrix(),Model,Model);
	m_pWii->GetRender3D()->RenderModelHardNorms( HashString::WiiMote, Model );
	//--------------------------

	m_pWii->GetCamera()->DoDefaultLight(50000.0f, 10000.0f, -10000.0f);

	m_pWii->GetGameDisplay()->DisplayMoon(HashString::MoonHiRes);


	m_pWii->GetCamera()->SetLightOff();
	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
	Util3D::CameraIdentity();
	Draw_Util::DrawRectangle( -320, -200-4,  640,  4,	 112, 0,0,0 );
	Draw_Util::DrawRectangle( -320, -200,	  640, 42,	  88, 0,0,0 );
	Draw_Util::DrawRectangle( -320, -200+42, 640,  4,	 112, 0,0,0 );

	// "B O L T    T H R O W E R"
	m_pWii->GetFontManager()->DisplayTextCentre( m_pWii->GetText("MainMenuScreenTopTitle") ,0,-180,190); 
	m_pWii->GetFontManager()->DisplayTextCentre( s_ReleaseVersion,280, -180,128, HashString::SmallFont); 

//	extern string MesageHack;
	string BarText = m_pWii->GetUpdateManager()->GetMessageVersionReport();

	HashLabel Name = m_pWii->GetMenuManager()->GetSelectedMenu();
	if (Name == HashString::Options)
		BarText= m_pWii->GetText("OptionsPopUpMessage");
	else if (Name == HashString::Start_Game)
		BarText= m_pWii->GetText("Start_GamePopUpMessage") + " - " + m_pWii->GetDifficulty();
	else if (Name == HashString::Intro)
		BarText= m_pWii->GetText("IntroPopUpMessage");
	else if (Name == HashString::Change_Tune)
		BarText = m_pWii->GetNameOfCurrentMusic();
	else if (Name == HashString::Controls)
		BarText = m_pWii->GetText("ControlsPopMessage");
	else if (Name == HashString::Credits)
		BarText = m_pWii->GetText("CreditsPopMessage");
	else if (Name == HashString::download_extra_music)
		BarText = m_pWii->GetText("DownloadExtraMusicPopUpMessage");
	


	Draw_Util::DrawRectangle( -300, 200, 600, 20, 55, 0,0,0 );
	m_pWii->GetFontManager()->DisplayTextCentre( BarText,0,210,222,HashString::SmallFont); 


	//----------------------------------------------------------
	Util3D::TransRot(320-35,240-55,-3.14f/4.0f);
	char Text[8];
	sprintf(Text,"%0d",m_pTimer->GetTimerSeconds());
	m_pWii->GetFontManager()->DisplayTextCentre(Text,0,0,80,HashString::SmallFont);
	//----------------------------------------------------------

	m_pWii->GetMenuManager()->SetMenuGroup("MainMenu");
	m_pWii->GetMenuManager()->Draw();
	m_pWii->GetMenuManager()->MenuLogic();

	
	if (WiiMote!=NULL)
	{
		float PointerX = m_pWii->GetCamera()->GetCamX() + WiiMote->x - (m_pWii->GetScreenWidth()/2);
		float PointerY = m_pWii->GetCamera()->GetCamY() + WiiMote->y - (m_pWii->GetScreenHeight()/2);

		Util3D::TransRot(PointerX,PointerY,0, roll );
		Draw_Util::DrawRectangle( -640, -3,  640*2,  6,	 32, 0,0,0 );
		Draw_Util::DrawRectangle( -3, -480,  6,  512*2,	 32, 0,0,0 );

		for (int ix=-14 ; ix<14; ++ix)
		{
			if (ix&1)
			{
				Draw_Util::DrawRectangle(  50*ix, -10,  2, 20, 32, 0,0,0 );
				Draw_Util::DrawRectangle( -10, 50*ix, 20, 2, 32, 0,0,0 );
			}
			else
			{
				Draw_Util::DrawRectangle(  50*ix, -15, 2, 30, 32, 0,0,0 );
				Draw_Util::DrawRectangle( -15, 50*ix, 30, 2, 32, 0,0,0 );
			}		
		}
		m_pWii->GetImageManager()->GetImage( m_pWii->m_FrameEndStartConstainer[HashString::AimingPointer32x32].StartFrame )
			->DrawImageXYZ( m_pWii->GetCamera()->GetCamX() + WiiMote->x - (m_pWii->GetScreenWidth()/2), 
			m_pWii->GetCamera()->GetCamY() + WiiMote->y - (m_pWii->GetScreenHeight()/2),
			WiiMote->z, 255, 0) ; //GetPlrVessel()->GetFacingDirection() );
	}

	m_pWii->GetGameDisplay()->DebugInformation();
}

void MenuScreens::DoControlsScreen()
{
	WPAD_ScanPads();
	m_pWii->GetInputDeviceManager()->Store();

	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
	m_ZoomAmountForSpaceBackground+=0.001;
	m_pWii->GetSpaceBackground()->DrawImageXYZ(0,0, 9400,255,0,10.0f + (8.0f+(sin(m_ZoomAmountForSpaceBackground)*6.0f)) );

	static float spin=0.0f;
	spin+=0.01;
	m_pWii->GetSpaceBackground()->DrawImageXYZ(0,0, 9400,50,sin(spin)*0.15,11.0f + (8.0f+(sin(m_ZoomAmountForSpaceBackground)*6.0f)) );

	//=========================

	static float bbb = 0;
	bbb+=0.025f;
		//-------------------
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

	m_pWii->GetCamera()->DoDefaultLight(200.0f, 100.0f, -1000.0f);

	//--------------------------
	Mtx Model,mat;
	//--------------------------
	guMtxRotRad(Model,'x', bbb);
	guMtxRotRad(mat,'y',M_PI/8 + sin(bbb) * 0.25f) ;
	guMtxConcat(mat,Model,Model);
	guMtxRotRad(mat,'z',  M_PI/2 + (cos(bbb) * 0.25f)) ;
	guMtxConcat(mat,Model,Model);
	guMtxTrans(mat, -100, 0, 0);
	guMtxConcat(mat,Model,Model);
	guMtxConcat(m_pWii->GetCamera()->GetcameraMatrix(),Model,Model);

	m_pWii->GetRender3D()->RenderModelHardNorms(HashString::WiiMote, Model);

	//--------------------------

	m_pWii->GetCamera()->SetLightOff();
	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
	//=====================
	Util3D::CameraIdentity();
	Draw_Util::DrawRectangle( -320, -200-4,  640,  4,	 112, 0,0,0 );
	Draw_Util::DrawRectangle( -320, -200,	  640, 42,	  88, 0,0,0 );
	Draw_Util::DrawRectangle( -320, -200+42, 640,  4,	 112, 0,0,0 );

	// "C O N T R O L S"
	m_pWii->GetFontManager()->DisplayTextCentre( m_pWii->GetText("ControlsMenuScreenTopTitle"),0,-180,190,HashString::LargeFont);


	ImageManager* pImageManager = m_pWii->GetImageManager();

	Util3D::CameraIdentity();
	Draw_Util::DrawRectangle(  0, -120,  42, 272,	 128, 255,255,255 );
	Draw_Util::DrawRectangle( 42, -120, 250, 272,	 64, 0,0,0 );
	
	int y= -145;
	int x= 21;
	int step = 45;

	pImageManager->GetImage(HashString::WiiMoteButtonA)->DrawImage(x,y+=step);
	m_pWii->GetFontManager()->DisplayTextVertCentre(m_pWii->GetText("WiiMoteButtonA"),32,0,200,HashString::SmallFont); //"Fire Missile"

	pImageManager->GetImage(HashString::WiiMoteButtonB)->DrawImage(x,y+=step);
	m_pWii->GetFontManager()->DisplayTextVertCentre(m_pWii->GetText("WiiMoteButtonB"),32,0,200,HashString::SmallFont);  // "Thrusters"

	pImageManager->GetImage(HashString::WiiMoteDirectionDownMarkedRed)->DrawImage(x,y+=step);
	m_pWii->GetFontManager()->DisplayTextVertCentre(m_pWii->GetText("WiiMoteDirectionDown"),32,0,200,HashString::SmallFont); // "Drop Mine"

	pImageManager->GetImage(HashString::WiiMoteDirectionUpMarkedRed)->DrawImage(x,y+=step);
	m_pWii->GetFontManager()->DisplayTextVertCentre(m_pWii->GetText("WiiMoteDirectionUp"),32,0,200,HashString::SmallFont); // "defence Mine"

	pImageManager->GetImage(HashString::WiiMoteInfraRedPointer)->DrawImage(x,y+=step);
	m_pWii->GetFontManager()->DisplayTextVertCentre(m_pWii->GetText("WiiMoteInfraRedPointer"),32,0,200,HashString::SmallFont); //"Aim"

	pImageManager->GetImage(HashString::WiiMoteButtonHome)->DrawImage(x,y+=step);
	m_pWii->GetFontManager()->DisplayTextVertCentre(m_pWii->GetText("WiiMoteButtonHome"),32,0,200,HashString::SmallFont);  //"Quit"

	Util3D::CameraIdentity();
	{
		static float wobble	(0);
		wobble+=0.015;
		// "PRESS A TO CONTINUE" 
		m_pWii->GetFontManager()->DisplayTextCentre(m_pWii->GetText("PressButtonAToContinueMessage"),0,200.0f,50 + fabs(cos(wobble)*60.0f),HashString::LargeFont);
	}

	m_pWii->SwapScreen();  
}


void MenuScreens::DoCreditsScreen()
{
	WPAD_ScanPads();
	m_pWii->GetInputDeviceManager()->Store();
	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);

	m_ZoomAmountForSpaceBackground+=0.001;
	m_pWii->GetSpaceBackground()->DrawImageXYZ(0,0, 9400,255,0,10.0f + (8.0f+(sin(m_ZoomAmountForSpaceBackground)*6.0f)) );

	static float spin=0.0f;
	spin+=0.01;
	m_pWii->GetSpaceBackground()->DrawImageXYZ(0,0, 9400,50,sin(spin)*0.15,11.0f + (8.0f+(sin(m_ZoomAmountForSpaceBackground)*6.0f)) );

	//=========================
	static float bbb = 0;
	bbb+=0.025f;
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

	m_pWii->GetCamera()->DoDefaultLight(250.0f, 1000.0f, -1000.0f);

	Mtx Model,mat;
	guMtxRotRad(Model,'x',M_PI );
	guMtxRotRad(mat,'z', sin(bbb)*0.55);
	guMtxConcat(mat,Model,Model);
	guMtxRotRad(mat,'y', M_PI + bbb*0.3f);
	guMtxConcat(mat,Model,Model);
	guMtxTrans(mat, 0,0, 0);
	guMtxConcat(mat,Model,Model);
	guMtxConcat(m_pWii->GetCamera()->GetcameraMatrix(),Model,Model);
	m_pWii->GetRender3D()->RenderModelHardNorms(HashString::Viper, Model);
	m_pWii->GetCamera()->SetLightOff();
	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
	//=====================

	Util3D::CameraIdentity();
	Draw_Util::DrawRectangle( -320, -200-4,  640,  4,	 112, 0,0,0 );
	Draw_Util::DrawRectangle( -320, -200,	  640, 42,	  88, 0,0,0 );
	Draw_Util::DrawRectangle( -320, -200+42, 640,  4,	 112, 0,0,0 );

	//"C R E D I T S"
	m_pWii->GetFontManager()->DisplayTextCentre(m_pWii->GetText("CreditsMenuScreenTopTitle"),0,-180,190,HashString::LargeFont);


	Util3D::TransRot(0,-164,0,0);
	int y(0);

	static float ScrollText=1.0f;
	ScrollText += 0.5f; //speed
	int i = ((int)ScrollText)/19;  // height of font plus needed padding
	int scroll = ( (int)ScrollText) % 19;

	//printf("%d",scroll);

	GX_SetScissor(0,86,m_pWii->GetScreenWidth(),m_pWii->GetScreenHeight()-230);

	static const int AMOUNT(16); // lines of text
	string Message;
	int CountItems=0;
	while ( (++CountItems) < AMOUNT) {
		// 'GetText' - all text comes from the GameConfiguration.xml file, i.e get text labeled 'Credits01'
		Message = m_pWii->GetText( "Credits" + Util::NumberToString(i++,2));
		if (Message == "TAG-END")
		{
			ScrollText=1.0f;
			break;
		}
		Util::Replace(Message,"%%RELEASE_VERSION%%", s_ReleaseVersion);
		Util::Replace(Message,"%%DATE_OF_BUILT%%", s_DateOfRelease);
		//int alpha = min(127+(int)(( (y/2) / (float)AMOUNT) * (128/(AMOUNT-1)) ) , 255);
		int alpha = min( y - scroll , 255);
	
		m_pWii->GetFontManager()->SetFontColour(0,0,0,255);
		m_pWii->GetFontManager()->DisplayTextCentre( Message,0-1, y - scroll, alpha ,HashString::SmallFont);	

		m_pWii->GetFontManager()->SetFontColour(255,255,255,255);
		m_pWii->GetFontManager()->DisplayTextCentre( Message,0, y - scroll, alpha ,HashString::SmallFont);	
		y+=19;
	};

	GX_SetScissor(0,0,m_pWii->GetScreenWidth(),m_pWii->GetScreenHeight());

	{
		static float wobble	(0);
		wobble+=0.05;
		m_pWii->GetFontManager()->DisplayTextCentre(m_pWii->GetText("PressButtonAToContinueMessage"),0,exp(sin(wobble)*2.8f)+340.0f,128,HashString::LargeFont);
	}

	m_pWii->SwapScreen(); 
}

void MenuScreens::DoOptionsScreen()
{
	WPAD_ScanPads();
	m_pWii->GetInputDeviceManager()->Store();

	Vtx* WiiMote( m_pWii->GetInputDeviceManager()->GetIRPosition() );

	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
	m_ZoomAmountForSpaceBackground+=0.001;
	m_pWii->GetSpaceBackground()->DrawImageXYZ(0,0, 9400,255,0,10.0f + (8.0f+(sin(m_ZoomAmountForSpaceBackground)*6.0f)) );
	static float spin=0.0f;
	spin+=0.01;
	m_pWii->GetSpaceBackground()->DrawImageXYZ(0,0, 9400,50,sin(spin)*0.15,11.0f + (8.0f+(sin(m_ZoomAmountForSpaceBackground)*6.0f)) );
	
	//=========================
	static float bbb = 0;
	bbb+=0.025f;
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

	m_pWii->GetCamera()->DoDefaultLight(250.0f, 40.0f, -600.0f);

	Mtx Model,mat;
	guMtxRotRad(Model,'x',cos(bbb*0.5));
	guMtxRotRad(mat,'z', sin(bbb)*0.55);
	guMtxConcat(mat,Model,Model);
	guMtxRotRad(mat,'y', M_PI + bbb*0.3f);
	guMtxConcat(mat,Model,Model);
	guMtxConcat(m_pWii->GetCamera()->GetcameraMatrix(),Model,Model);
	//Wii.GetRender3D()->RenderModelHardNorms("SaturnV", Model);
	m_pWii->GetRender3D()->RenderModelHardNorms( HashString::Satellite,Model);

	m_pWii->GetCamera()->SetLightOff();
	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);

	//=========================
	Util3D::CameraIdentity();
	Draw_Util::DrawRectangle( -320, -200-4,  640,  4,	 112, 0,0,0 );
	Draw_Util::DrawRectangle( -320, -200,	  640, 42,	  88, 0,0,0 );
	Draw_Util::DrawRectangle( -320, -200+42, 640,  4,	 112, 0,0,0 );
	// "O P T I O N S"
	m_pWii->GetFontManager()->DisplayTextCentre(m_pWii->GetText("OptionsMenuScreenTopTitle"),0,-180,190,HashString::LargeFont);
		//=========================

	m_pWii->GetMenuManager()->SetMenuGroup("OptionsMenu");
	
	m_pWii->GetMenuManager()->MenuLogic();
	
	m_pWii->GetMenuManager()->Draw();


	// Draw aim pointer
	m_pWii->GetImageManager()->GetImage( m_pWii->m_FrameEndStartConstainer[HashString::AimingPointer32x32].StartFrame )
			->DrawImageXYZ( m_pWii->GetCamera()->GetCamX() + WiiMote->x - (m_pWii->GetScreenWidth()/2), 
			m_pWii->GetCamera()->GetCamY() + WiiMote->y - (m_pWii->GetScreenHeight()/2),
			WiiMote->z, 255, 0) ; //GetPlrVessel()->GetFacingDirection() );

	m_pWii->SwapScreen();  
}

