#include "FridgeMagnetManager.h"

#include "WiiManager.h"
#include "ImageManager.h"
#include "debug.h"
#include "Image.h"
#include "Util.h"

#include "InputDeviceManager.h"
#include <asndlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

//--------------------------------------------------------------------------
// FridgeMagnet components listed fist
//--------------------------------------------------------------------------
FridgeMagnet::FridgeMagnet(float x,float y, const char& Character)
{
	Init();
	SetCharacter(Character);
	SetPos(x,y,0.0f);
}

void FridgeMagnet::Init()
{
	m_Character=' ';

	m_Pos.x=0;
	m_Pos.y=0;
	m_Pos.z=-500;

	m_Colour.a = 0xff;
	m_Colour.r = 0xf0;
	m_Colour.g = 0xf0;
	m_Colour.b = 0xf0;

	m_Angle = 0;

	m_Scale = 1.0f;
	m_MinScale = m_Scale;

	m_Velocity.x=0.0f;
	m_Velocity.y=0.0f;
	m_Velocity.z=0.0f;

	m_bDisable = false;
	bInHand=false;
}

void FridgeMagnet::Display()
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );
	GXColor& Col( GetColour() );
	Wii.GetFontManager()->SetFontColour(Col.r,Col.g,Col.b);

	if ( IsDisabled() )
		Wii.GetFontManager()->GetFont()->DisplayTextScaled( &GetCharacter(), 0,0,GetScale(),64);
	else
		Wii.GetFontManager()->GetFont()->DisplayTextScaled( &GetCharacter(), 0,0,GetScale(),0xff);
}

void FridgeMagnet::DisplayShadow(u8 alpha, float Scale)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );
	Wii.GetFontManager()->SetFontColour(0,0,0);

	if ( IsDisabled() )
		Wii.GetFontManager()->GetFont()->DisplayTextScaled( &GetCharacter(), 3,3,Scale , 64);  // letter's shadow
	else
		Wii.GetFontManager()->GetFont()->DisplayTextScaled( &GetCharacter(), 3,3,Scale ,alpha);  // letter's shadow
}

FridgeMagnet* FridgeMagnet::SetColour(u8 Red,u8 Green,u8 Blue, u8 Alpha) 
{ 
	m_Colour.r=Red;
	m_Colour.g=Green;
	m_Colour.b=Blue; 
	m_Colour.a= Alpha; 
	return this;
}

//--------------------------------------------------------------------------
// Manager components
//--------------------------------------------------------------------------
FridgeMagnetManager::FridgeMagnetManager()
{
	m_AngleForOnscreenPointer=0.0f;
	m_pSelectedFridgeMagnet[0] = NULL;
	m_pSelectedFridgeMagnet[1] = NULL;
	m_pSelectedFridgeMagnet[2] = NULL;
	m_pSelectedFridgeMagnet[3] = NULL;
}

void FridgeMagnetManager::MotionLogic(float DropAmount, float StopAtGround)
{
	//bool bFirstUsed(false);
	for (std::vector<FridgeMagnet*>::iterator iter(GetBegin()); iter!=GetEnd(); ++iter)
	{
		FridgeMagnet* pFridgeMagnet(*iter);	

		pFridgeMagnet->SetZ(pFridgeMagnet->GetZ() + pFridgeMagnet->m_Velocity.z);
		pFridgeMagnet->SetY(pFridgeMagnet->GetY() + pFridgeMagnet->m_Velocity.y);
		pFridgeMagnet->SetX(pFridgeMagnet->GetX() + pFridgeMagnet->m_Velocity.x);
		
		if (pFridgeMagnet->GetZ() > StopAtGround)  // negitave is in the air, Positive in bellow the ground
		{
			if (ASND_StatusVoice(4)== SND_UNUSED)
			{
				Singleton<WiiManager>::GetInstanceByRef().GetSoundManager()->GetSound(5)->Play(4,120,120);
			}
			pFridgeMagnet->SetZ(StopAtGround);
			pFridgeMagnet->m_Velocity.x=(0.0f);
			pFridgeMagnet->m_Velocity.y=(0.0f);
			pFridgeMagnet->m_Velocity.z=(0.0f);
		}
		else if (pFridgeMagnet->GetZ() != StopAtGround)
		{
			pFridgeMagnet->m_Velocity.z+=DropAmount;
		}

		// Scale
		float Scale(pFridgeMagnet->GetScale() * 0.96f);
		if (Scale < pFridgeMagnet->GetMinScale()) 
		{
			Scale=pFridgeMagnet->GetMinScale();
		}
		pFridgeMagnet->SetScale(Scale);
	}
}

void FridgeMagnetManager::SelectLogic( u8 WiiControl)
{
	static float Once[4] = {true,true,true,true};
	static float StartAngle[4] = {0.0f,0.0f,0.0f,0.0f};
	static float PlacedAngle[4] = {0.0f,0.0f,0.0f,0.0f};

	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 

	vec3f_t* pos(Wii.GetInputDeviceManager()->GetIRPosition( WiiControl ));
	if (pos==NULL)
		return;

	float WiiMotePointerXpos = (pos->x * Wii.GetCamera()->GetCameraFactor()) + (Wii.GetCamera()->GetCamX() - 
			((Wii.GetScreenWidth()/2) * Wii.GetCamera()->GetCameraFactor() ));
	float WiiMotePointerYpos = (pos->y * Wii.GetCamera()->GetCameraFactor()) + (Wii.GetCamera()->GetCamY() - 
			((Wii.GetScreenHeight()/2) * Wii.GetCamera()->GetCameraFactor() ));

	u32			uButtonsHeld(WPAD_ButtonsHeld(WiiControl));
//	u32			uButtonsDown(WPAD_ButtonsDown(WiiControl));
	WPADData*	pPadData(WPAD_Data(WiiControl));

	float RadAngle( (pPadData->orient.roll/180.0f)*M_PI ); // hardware in degrees, bit odd!

	Wii.GetInputDeviceManager()->m_HandOver[WiiControl]=0;

	int JustPickOne(0);
	for (std::vector<FridgeMagnet*>::iterator iter(GetBegin()); iter!=GetEnd(); ++iter)
	{
		FridgeMagnet* pFridgeMagnet(*iter);	

		if (pFridgeMagnet->IsDisabled() )
			continue;

		if (pFridgeMagnet->GetZ() < -0.5 )
			continue;

		static const float Area(20.0f);
		if ((!pFridgeMagnet->bInHand) &&
			(m_pSelectedFridgeMagnet[WiiControl]==NULL) && 
			(WiiMotePointerXpos>pFridgeMagnet->GetX()-Area) && 
			(WiiMotePointerXpos<pFridgeMagnet->GetX()+Area) && 
			(WiiMotePointerYpos>pFridgeMagnet->GetY()-Area) && 
			(WiiMotePointerYpos<pFridgeMagnet->GetY()+Area))
		{
			Wii.m_bHideAllMenuItems = true;

			if (JustPickOne==0)
			{
				JustPickOne++;
				pFridgeMagnet->SetScale(2.4f);
				Wii.GetInputDeviceManager()->m_HandOver[WiiControl]=2;
			}
			if (uButtonsHeld & (WPAD_BUTTON_B|WPAD_BUTTON_A))
			{
				Wii.GetSoundManager()->GetSound(1)->Play(WiiControl);

				StartAngle[WiiControl] = RadAngle;
				PlacedAngle[WiiControl] = pFridgeMagnet->GetAngle();
				m_pSelectedFridgeMagnet[WiiControl] = pFridgeMagnet;

				Wii.GetInputDeviceManager()->m_HandOver[WiiControl]=1;

				pFridgeMagnet->bInHand = true;
			}
			else
			{
				pFridgeMagnet->bInHand = false;
			}
		}
		else
		{						
			if (m_pSelectedFridgeMagnet[WiiControl] == pFridgeMagnet)
			{
				if (uButtonsHeld & (WPAD_BUTTON_B|WPAD_BUTTON_A))
				{
					// Pick it up and move it
					pFridgeMagnet->SetScale(1.0f);
					pFridgeMagnet->SetPos(WiiMotePointerXpos,WiiMotePointerYpos,0);
					pFridgeMagnet->SetAngle(RadAngle - StartAngle[WiiControl] + PlacedAngle[WiiControl]);

					Wii.GetInputDeviceManager()->m_HandOver[WiiControl]=1;

					if (pFridgeMagnet->bInHand == false)
						Wii.GetSoundManager()->GetSound(1)->Play(WiiControl);

					pFridgeMagnet->bInHand = true;
					Once[WiiControl] = true; // allow sound, may be second times its picked up without leaving it's zone
				}
				else 
				{
					if (Once[WiiControl])
					{
						Once[WiiControl] = false;
						Wii.GetSoundManager()->GetSound(0)->Play(WiiControl);
					}

					if ( ! ((WiiMotePointerXpos>pFridgeMagnet->GetX()-Area) && 
							(WiiMotePointerXpos<pFridgeMagnet->GetX()+Area) && 
							(WiiMotePointerYpos>pFridgeMagnet->GetY()-Area) && 
							(WiiMotePointerYpos<pFridgeMagnet->GetY()+Area)))
					{
						// let go of the letter and no longer over
						Once[WiiControl] = true;
						m_pSelectedFridgeMagnet[WiiControl] = NULL;
						// only at this stage is is ok to pick another one up
					}
					pFridgeMagnet->bInHand = false;
					pFridgeMagnet->SetScale(1.0f);
				}
			}
		}
	}

	// deals with things after the circle selection picker has been used]
	if ( (m_pSelectedFridgeMagnet[WiiControl]!=NULL) && 
		(Wii.GetSelectionMode() == PickFromCircleLettersSelectionMode))
	{
		if (m_pSelectedFridgeMagnet[WiiControl]!=NULL)
		{
			m_pSelectedFridgeMagnet[WiiControl]
				->SetDisable(true)
				->SetScale(1.0f)
				->SetMinScale(1.0f);
				//->m_Velocity.z=0;
		}

		RemoveEnabledFridgeMagnets();  // get rid of the new cricle selection letters as we no longer need to show them
		DisableAllFridgeMagnets(false); // enable everthing - now nothing is grayed out
		Wii.SetSelectionMode( CompletedSelectionMode ); // now ready for next time
		Wii.SetEnableAndActivateForMenuItem(true,"Letters");
	}


	if ( m_pSelectedFridgeMagnet[WiiControl] != NULL ) 
		Wii.m_bHideAllMenuItems = true;

	
	DoControls(WiiControl);
}


void FridgeMagnetManager::DoDisplayAllControllers()
{

	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 

	float LastFactor = Wii.GetCamera()->GetCameraFactor();

	//------ correct 3d camera for menu view
	float Factor = 1.0f;
	Wii.GetCamera()->SetCameraFactor(Factor);
	Wii.GetCamera()->SetCamZ( -(Wii.GetScreenHeight()/2) * Factor );
	Wii.GetCamera()->SetCameraView(); 
	//------

	m_AngleForOnscreenPointer+=0.065;  //Spin the wiimote onscreen pointer
	if (m_AngleForOnscreenPointer>M_PI*2) 
		m_AngleForOnscreenPointer-=M_PI*2;

	for (int i(WPAD_CHAN_0);i<=WPAD_CHAN_3; i++)
	{
		vec3f_t* pos(Wii.GetInputDeviceManager()->GetIRPosition( i ));

		if (pos==NULL)
			continue;

	//	float WiiMotePointerXpos = (pos->x) + (Wii.GetCamera()->GetCamX() - ((Wii.GetScreenWidth()/2) * Wii.GetCamera()->GetCameraFactor() ));
	//	float WiiMotePointerYpos = (pos->y) + (Wii.GetCamera()->GetCamY() - ((Wii.GetScreenHeight()/2) * Wii.GetCamera()->GetCameraFactor() ));
		float WiiMotePointerXpos = (pos->x) + (Wii.GetCamera()->GetCamX() - ((Wii.GetScreenWidth()/2)  ));
		float WiiMotePointerYpos = (pos->y) + (Wii.GetCamera()->GetCamY() - ((Wii.GetScreenHeight()/2) ));

		float  Angle(Wii.GetInputDeviceManager()->GetAngle( i ));
		int  Over(Wii.GetInputDeviceManager()->m_HandOver[i]);

		switch (i)
		{
		case 0: DrawWiiMotePointer(WiiMotePointerXpos,WiiMotePointerYpos,0xff,0xff,0xff,Angle,Over); 
			break;
		case 1: DrawWiiMotePointer(WiiMotePointerXpos,WiiMotePointerYpos,0xff,0xdd,0xdd,Angle,Over); 
			break;
		case 2: DrawWiiMotePointer(WiiMotePointerXpos,WiiMotePointerYpos,0xdd,0xff,0xdd,Angle,Over); 
			break;
		case 3: DrawWiiMotePointer(WiiMotePointerXpos,WiiMotePointerYpos,0xdd,0xdd,0xff,Angle,Over); 
			break;
		}
	}

		//----- back to normal view
	Wii.GetCamera()->SetCameraFactor(LastFactor);
	Wii.GetCamera()->SetCamZ( -(Wii.GetScreenHeight()/2) * LastFactor );
	Wii.GetCamera()->SetCameraView(); 
	//------
}


void FridgeMagnetManager::DoMainDisplay()
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 

	for (std::vector<FridgeMagnet*>::iterator iter(GetBegin()); iter!=GetEnd(); ++iter)
	{
		FridgeMagnet* pFridgeMagnet(*iter);	
		Mtx FinalMatrix, TransMatrix; 
		//guMtxIdentity(TransMatrix); 
		guMtxRotRad(TransMatrix,'Z',pFridgeMagnet->GetAngle());  // Rotage

		float z = pFridgeMagnet->GetZ();
		if ( (z >= -255) && (z<=0))
		{
			guMtxTransApply(TransMatrix,TransMatrix,pFridgeMagnet->GetX(), pFridgeMagnet->GetY(),0);	// Position
			guMtxConcat(Wii.GetCamera()->GetcameraMatrix(),TransMatrix,FinalMatrix);
			GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0); 
			pFridgeMagnet->DisplayShadow( 255 + (z) , pFridgeMagnet->GetScale() + ( z *-0.01) );
		}
		else if (z > 0)
		{
			guMtxTransApply(TransMatrix,TransMatrix,pFridgeMagnet->GetX(), pFridgeMagnet->GetY(), z );	// Position
			guMtxConcat(Wii.GetCamera()->GetcameraMatrix(),TransMatrix,FinalMatrix);
			GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0); 
			pFridgeMagnet->DisplayShadow( 255 , pFridgeMagnet->GetScale() );
		}

		//guMtxIdentity(TransMatrix); 
		guMtxRotRad(TransMatrix,'Z',pFridgeMagnet->GetAngle()); 
		guMtxTransApply(TransMatrix,TransMatrix,pFridgeMagnet->GetX(), pFridgeMagnet->GetY(), z );
		guMtxConcat(Wii.GetCamera()->GetcameraMatrix(),TransMatrix,FinalMatrix);
		GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0);  

		pFridgeMagnet->Display();
	}
}

void FridgeMagnetManager::DrawWiiMotePointer(float x, float y, u8 r, u8 g, u8 b, float Angle, int Image)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );

	Mtx FinalMatrix, TransMatrix; 
//	guMtxIdentity(TransMatrix);
	guMtxRotRad(TransMatrix, 'Z', Angle);
	guMtxTransApply(TransMatrix,TransMatrix,x,y,0);	// origin
	guMtxConcat(Wii.GetCamera()->GetcameraMatrix(),TransMatrix,FinalMatrix);
	GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0);  

	switch(Image)
	{
	case 0:
		Wii.GetImageManager()->GetImage(0)->DrawImage(255,r,g,b);  // hand point
		break;
	case 1:
		Wii.GetImageManager()->GetImage(1)->DrawImage(134,r,g,b);  // hand Grab
		break;
	case 2:
		Wii.GetImageManager()->GetImage(2)->DrawImage(160,r,g,b);   // hand over
		break;
	}
}

void FridgeMagnetManager::DoControls(u8 Controller)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 
	u32 Pad (Wii.GetInputDeviceManager()->GetPadButton(Controller) );

	static const float ScrollSpeed(4);
	bool bClearSelection = false;

	if (Pad & WPAD_BUTTON_LEFT)
	{
		Wii.GetCamera()->AddCamX(-ScrollSpeed);
		bClearSelection=true;
	}
	if (Pad & WPAD_BUTTON_RIGHT)
	{
		Wii.GetCamera()->AddCamX(ScrollSpeed);
		bClearSelection=true;
	}
	if (Pad & WPAD_BUTTON_UP)
	{
		Wii.GetCamera()->AddCamY(-ScrollSpeed);
		bClearSelection=true;
	}
	if (Pad & WPAD_BUTTON_DOWN)
	{
		Wii.GetCamera()->AddCamY(ScrollSpeed);
		bClearSelection=true;
	}
	
	if ((Wii.GetSelectionMode() != CompletedSelectionMode) && bClearSelection)
	{
		RemoveEnabledFridgeMagnets(); 
		DisableAllFridgeMagnets(false);
		Wii.SetSelectionMode( CompletedSelectionMode );
		Wii.SetEnableAndActivateForMenuItem(true,"Letters");
	}

	if (Pad & WPAD_BUTTON_MINUS) 
	{
		float CameraFactor (Wii.GetCamera()->GetCameraFactor());
		CameraFactor += 0.01;
		if (CameraFactor>2.2f) 
			CameraFactor=2.2f;
		// wiimote pointer onscreen size is now constant - zoom no longer effects it
		//WPAD_SetVRes(WPAD_CHAN_ALL, Wii.GetGXRMode()->fbWidth* CameraFactor, Wii.GetGXRMode()->xfbHeight * CameraFactor);  // resolution of IR
		Wii.GetCamera()->SetCameraFactor(CameraFactor);
		Wii.GetCamera()->SetCamZ( -(Wii.GetScreenHeight()/2) * CameraFactor );
	}

	if (Pad & WPAD_BUTTON_PLUS) 
	{
		float CameraFactor (Wii.GetCamera()->GetCameraFactor());
		CameraFactor -= 0.01;
		if (CameraFactor<0.8f) 
			CameraFactor=0.8f;
		//WPAD_SetVRes(WPAD_CHAN_ALL, Wii.GetGXRMode()->fbWidth* CameraFactor,Wii.GetGXRMode()->xfbHeight * CameraFactor);  // resolution of IR
		Wii.GetCamera()->SetCameraFactor(CameraFactor);
		Wii.GetCamera()->SetCamZ( -(Wii.GetScreenHeight()/2) * CameraFactor );
	}

	if (Pad & WPAD_BUTTON_1) // Shaking WiiMote
	{
		vec3f_t* pos(Wii.GetInputDeviceManager()->GetIRPosition( Controller ));
		if (pos!=NULL)
		{
			float WiiMotePointerXpos = (pos->x * Wii.GetCamera()->GetCameraFactor()) + (Wii.GetCamera()->GetCamX() - 
				((Wii.GetScreenWidth()/2) * Wii.GetCamera()->GetCameraFactor() ));
			float WiiMotePointerYpos = (pos->y * Wii.GetCamera()->GetCameraFactor()) + (Wii.GetCamera()->GetCamY() - 
				((Wii.GetScreenHeight()/2) * Wii.GetCamera()->GetCameraFactor() ));

			FridgeMagnet* pFridgeMagnet(HitsFridgeMagnet(WiiMotePointerXpos,WiiMotePointerYpos));
			if (pFridgeMagnet!=NULL)
			{
//				// before it's removed make sure no ones holding it - for simplicity we even check the hand doing the removal
//				if ((m_pSelectedFridgeMagnet[0] != pFridgeMagnet) &&
//					(m_pSelectedFridgeMagnet[1] != pFridgeMagnet) &&
//					(m_pSelectedFridgeMagnet[2] != pFridgeMagnet) &&
//					(m_pSelectedFridgeMagnet[3] != pFridgeMagnet))
				if (!pFridgeMagnet->bInHand)
				{
					RemoveFridgeMagnet( pFridgeMagnet );
					Wii.GetSoundManager()->GetSound(2)->Play(Controller);
				}
			}
		}
	}

	if (Pad & WPAD_BUTTON_2 ) // pile up magnets 
	{
		if (ASND_StatusVoice(Controller) == SND_UNUSED)
			Wii.GetSoundManager()->GetSound(6)->Play(Controller);

		vec3f_t* pos(Wii.GetInputDeviceManager()->GetIRPosition( Controller ));
		if (pos!=NULL)
		{
			float WiiMotePointerXpos = (pos->x * Wii.GetCamera()->GetCameraFactor()) + (Wii.GetCamera()->GetCamX() - 
				((Wii.GetScreenWidth()/2) * Wii.GetCamera()->GetCameraFactor() ));
			float WiiMotePointerYpos = (pos->y * Wii.GetCamera()->GetCameraFactor()) + (Wii.GetCamera()->GetCamY() - 
				((Wii.GetScreenHeight()/2) * Wii.GetCamera()->GetCameraFactor() ));

			for (std::vector<FridgeMagnet*>::iterator iter(GetBegin()); iter!=GetEnd(); ++iter)
			{
				FridgeMagnet* pFridgeMagnet(*iter);	
				if (!pFridgeMagnet->bInHand)
				{
					float rx= (float)((rand()%1000)-500)*Wii.GetCamera()->GetCameraFactor() ;
					float ry= (float)((rand()%1000)-500)*Wii.GetCamera()->GetCameraFactor() ;
					pFridgeMagnet->m_Velocity.x = (float)((WiiMotePointerXpos - rx) - pFridgeMagnet->GetX())*0.01;
					pFridgeMagnet->m_Velocity.y = (float)((WiiMotePointerYpos - ry) - pFridgeMagnet->GetY())*0.01;
					pFridgeMagnet->m_Velocity.z = -1;
					pFridgeMagnet->SetZ(0);
				}
			}
		}
	}

	// Shake
	u32 Status;
	if (WPAD_Probe(Controller, &Status) == WPAD_ERR_NONE)  // check wiimote has not timed out/sleeping to save battery power
	{
		WPADData* pPadData2(WPAD_Data(Controller));
		int shake(((pPadData2->accel.x/100)+(pPadData2->accel.y/80)+(pPadData2->accel.z/80)));

		if (shake!=0)
		{
			//	ExitPrintf("%d",shake);  //debug
			if (shake < 8 || shake > 20)
			{
				Wii.GetSoundManager()->GetSound(3)->Play(Controller);
				u32 Totalshakepower( (pPadData2->accel.x + pPadData2->accel.y + pPadData2->accel.z) );
				//ExitPrintf("    %f  %d",Totalshakepower,pPadData2->accel.x + pPadData2->accel.y + pPadData2->accel.z);  //debug
				for (std::vector<FridgeMagnet*>::iterator iter(/*Manager.*/GetBegin()); iter!=/*Manager.*/GetEnd(); ++iter)
				{
					FridgeMagnet* pFridgeMagnet(*iter);	

					if (!pFridgeMagnet->bInHand)
					{
						pFridgeMagnet->m_Velocity.z=  -( ( rand()%Totalshakepower + (Totalshakepower/2)) * 0.0026f ) ;
						pFridgeMagnet->m_Velocity.x=( (float)(rand()%1000) - 500.0f )*(pFridgeMagnet->m_Velocity.z*0.0005);
						pFridgeMagnet->m_Velocity.y=( (float)(rand()%1000) - 500.0f )*(pFridgeMagnet->m_Velocity.z*0.0005);
					}
				}
			}
		}
	}
	
	// spit out incremental magnets or clone if one is held
	if (( (WPAD_ButtonsDown(Controller) & WPAD_BUTTON_A) &&
	     (WPAD_ButtonsHeld(Controller) & WPAD_BUTTON_B) )
		||
		  ( (WPAD_ButtonsHeld(Controller) & WPAD_BUTTON_A) &&
	     (WPAD_ButtonsDown(Controller) & WPAD_BUTTON_B) ))
	{
		if (Wii.GetSelectionMode() == PickFromCircleLettersSelectionMode)
		{
			RemoveEnabledFridgeMagnets();  // get rid of the new cricle selection letters as we no longer need to show them
			DisableAllFridgeMagnets(false); // enable everthing - now nothing is grayed out
			Wii.SetSelectionMode( CompletedSelectionMode ); // now ready for next time
			Wii.SetEnableAndActivateForMenuItem(true,"Letters");
		}

		// add a new letter - it will drop down from a height above the pointer.
		static int count[]= {0,0,0,0};
		char a;

		if (m_pSelectedFridgeMagnet[Controller]!=NULL)
		{
			a = m_pSelectedFridgeMagnet[Controller]->GetCharacter();
		}
		else
		{
			a = 'a' + count[Controller];
			count[Controller] = (count[Controller]+1) % 26;
		}

		u8 red =   LetterColourContainer[ a ].r;
		u8 green = LetterColourContainer[ a ].g;
		u8 blue =  LetterColourContainer[ a ].b;

		vec3f_t* pos(Wii.GetInputDeviceManager()->GetIRPosition( Controller ));
		if (pos!=NULL)
		{
			float WiiMotePointerXpos = (pos->x * Wii.GetCamera()->GetCameraFactor()) + (Wii.GetCamera()->GetCamX() - 
				((Wii.GetScreenWidth()/2) * Wii.GetCamera()->GetCameraFactor() ));
			float WiiMotePointerYpos = (pos->y * Wii.GetCamera()->GetCameraFactor()) + (Wii.GetCamera()->GetCamY() - 
				((Wii.GetScreenHeight()/2) * Wii.GetCamera()->GetCameraFactor() ));

			FridgeMagnet* pMag(AddNewFridgeMagnet(WiiMotePointerXpos, WiiMotePointerYpos, a));

			pMag->SetColour(red,green,blue,0xaf)
				->SetAngle( ( (float)rand()/RAND_MAX ) - 0.5f )
				->SetZ(-255);

			pMag->m_Velocity.z=0;

			Wii.GetSoundManager()->GetSound(4)->Play(Controller);
		}
	}
}


void FridgeMagnetManager::AddPickerCircleLetters(std::string SelectionMenu, float size, float LetterSize)
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() ); 

	float Zoom = Wii.GetCamera()->GetCameraFactor();
	float camx = Wii.GetCamera()->GetCamX();
	float camy = Wii.GetCamera()->GetCamY();
	for (std::string::iterator Iter(SelectionMenu.begin()); Iter!=SelectionMenu.end(); ++Iter)
	{
		int ItemNo = std::distance(SelectionMenu.begin(), Iter);
		float segment = (M_PI*2) / (SelectionMenu.length());
		float x = size * sin(segment * ItemNo) * (Wii.GetScreenWidth()/3);
		float y = size * -cos(segment * ItemNo) * (Wii.GetScreenHeight()/3);
		//size*=0.985;
		x += camx - Zoom;
		y += camy - Zoom;

		u8 red = LetterColourContainer[ *Iter ].r;
		u8 green = LetterColourContainer[ *Iter ].g;
		u8 blue = LetterColourContainer[ *Iter ].b;

		AddNewFridgeMagnet( x, y, *Iter )
			->SetColour(red,green,blue,0xff)
			->SetScale( exp((float)ItemNo*0.05) )
			->SetMinScale(LetterSize);	// nice chunky size for selecting
	}
}