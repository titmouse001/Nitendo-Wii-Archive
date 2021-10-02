//------------------------------------------
// WORKING NOTES - THIS CODE IS NOT COMPILED
//------------------------------------------
#if 0

#define EFB_ADDRESS 0x08000000

u32 GetPixelFrom4x4RGBA(int x, int y, Image* tex) {
	u8 *truc = (u8*)tex->GetImageData();
    u8 r, g, b, a;
    u32 offset;

	offset = (((y >> 2)<<4) * tex->GetWidth() ) + ((x >> 2)<<6) + (((y%4 << 2) + x%4 ) << 1);

    a=*(truc+offset);
    r=*(truc+offset+1);
    g=*(truc+offset+32);
    b=*(truc+offset+33);

    return((r<<24) | (g<<16) | (b<<8) | a);
}

void SetPixelTo4x4RGBA(int x, int y, Image* tex, u32 color) {
    u8 *truc = (u8*)tex->GetImageData();
    u32 offset;

    offset = (((y >> 2)<<4) * tex->GetWidth() ) + ((x >> 2)<<6) + (((y%4 << 2) + x%4 ) <<1);

    *(truc+offset)=color & 0xFF;
    *(truc+offset+1)=(color>>24) & 0xFF;
    *(truc+offset+32)=(color>>16) & 0xFF;
    *(truc+offset+33)=(color>>8) & 0xFF;
}


=====================================

	if (Pad == WPAD_BUTTON_DOWN)//  && once == false)
		{
			once=true;
			for (int y=0; y<3; y++)
			{
				for (int x=0; x<3; x++)
				{
					u32 BlockID = ( (*pMapLayers->GetBegin())->GetValueFromOrigin(
						(x*16)+vehicle1.GetPosition().x-16,
						(y*16)+vehicle1.GetPosition().y-16));

					BlockID &=0x7fff;

					Wii.GetImageManager()->GetImage(BlockID)->DrawImageNoAlpha(x*16,y*16); 

				}
			}
			Wii.GetSpriteManager()->GetImage( ( vehicle1.GetDirection() / (M_PI*2.0f) )*64 + 64 )->DrawImage 
				( (int)vehicle1.GetPosition().x&0xf , ((int)vehicle1.GetPosition().y&0xf), 0x44 );

// Proof of concept .. to be refactored...later

// intrernal YUV giving me problems - This is a work around to let me take copies the internal 4x4RGBA format

// I'm unable to use 3x3 , the wii's colour format is hard to use
// so I'm using something 1 wide (16pixels) works fine with a memcpy
// anything bigger than 16 will not process the same way even if cut later in 16pixel steps


			GX_DrawDone();
			GX_InvalidateTexAll();
			GX_Flush();

		//	GX_SetDispCopyGamma(GX_GM_2_2);

//			qwerty[0] = DrawGXPipeToNewTexture(16*0,0,16*3,16*3);    // NEED TO FREE THIS
//			qwerty[1]= DrawGXPipeToNewTexture(16*1,0,16*1,16*3);  
//			qwerty[2]= DrawGXPipeToNewTexture(16*2,0,16*1,16*3);	

		//	GX_SetDispCopyGamma(GX_GM_1_0);

			for (int y=0; y<3; y++)
			{
				{
					for (u8 i=0;i<3;i++)
					{
						u32 ID = ( (*pMapLayers->GetBegin())->GetValueFromOrigin(
							(i*16)+vehicle1.GetPosition().x-16,	(y*16)+vehicle1.GetPosition().y-16));

				//		if ( !(ID & 0x8000) )
						{
							//Wii.GetImageManager()->StoreImage((u8*)qwerty[i]->GetImageData(),0,y*16,16,16);   // 3 strips of 16*48

							// Wii.GetImageManager()->StoreImage((u8*)qwerty[0]->GetImageData(),i*16,y*16,16,16);  // set get

							Wii.GetImageManager()->StoreImage(GXPipeToNewTexture(i*16,y*16,16,16));  // final simple method

							pMapLayers->GetSolidLayer()->SetValueFromOrigin(
							vehicle1.GetPosition().x-16+(16*i),	(y*16)+vehicle1.GetPosition().y-16, 0x8000 | ( Wii.GetImageManager()->GetImageDataSize()-1 ));
						}
					}

				//	pMapLayers->GetSolidLayer()->SetValueFromOrigin(
				//	vehicle1.GetPosition().x-16+(16*1),	(y*16)+vehicle1.GetPosition().y-16, 0x8000 | (Wii.GetImageManager()->GetImageDataSize()-2));

				//	pMapLayers->GetSolidLayer()->SetValueFromOrigin(
				//	vehicle1.GetPosition().x-16+(16*2),	(y*16)+vehicle1.GetPosition().y-16, 0x8000 | (Wii.GetImageManager()->GetImageDataSize()-1));
				}
				//else
				{

				}
			}
		}


	
// CPU VERSION - unfinnished


			//pTempImage3x3 = new Image(3*16,3*16);

			//u32 BlockID = (*pMapLayers->GetBegin())->GetValueFromOrigin((u32)vehicle1.GetPosition().x,(u32)vehicle1.GetPosition().y);
			//Image* pImage = Wii.GetImageManager()->GetImage(BlockID);
			//Wii.GetImageManager()->StoreImage(pImage);

			//for (int i=0; i<16; i++)
			//	memcpy(pTempImage3x3->GetImageData() + (i*16*3), pImage->GetImageData()+(i*16) , 16*4);

			//pTempImage3x3->ReadyTextureForHardWare();

			//(*pMapLayers->GetBegin())->SetValueFromOrigin(
			//	vehicle1.GetPosition().x,
			//	vehicle1.GetPosition().y, Wii.GetImageManager()->GetImageDataSize()-1);


	//---------


	Welcome to DCEmu, Dont Just be a Guest, join in the fun here - please click here to register...
 : Spelkontroll - Emulation64 - EmuHelp - DCEmu UK Homebrew Network Network Sites:  News Sites:- Emulation64- EmuHelp- DCEmu- Emutastic- The Saturn Shack- CPS2Emu- GameCube Emulation- GP32 Emulation- Nintendo DS News- PSP News- Alternative Handheld Emulation- Xbox 360 News- PS3 Evolution- Console Hardware News- PS2Emu- Xbox Evolution- GBA Emulation- Apple iPhone News- Dreamcast News- GP2X Emulation- Nintendo Wii News- N64 Homebrew- Sega Saturn NewsMessage Boards:- EmuTalk- EmuHelp Forums- EmuFanatics Forums- DCEmu ForumsEmulators:- GC - Dolphin- GC - Dolwin- GC - WhineCube- GC - Ninphin- PS2 - PS2Emu- PS2 - NSX2- DC - Dreamemu- N64 - 1964- N64 - Apollo64- N64 - Mupen64- N64 - PJ64- N64 - PJ64K- N64 - TR64- N64 - UHLE Alpha- PSX - PSXeven- PSX - FPSEce- NES - NesterDCGlide Wrappers:- eVoodooEmulator Plugins:- PS2 - GSMax- PSX - CDR Mooby- N64 - glnintendo64()Front Ends:- Giri Giri LoaderSupport:- Olax's Emulation Centre- 1964 Cheats- PJ64 Cheats- DreamComp- Emu64 CheatsHumour:- JCD's The DoejoOther:- Juventuz- XBMCScripts- Slipknot CPD- Stylefone- EFX2 Blogs- DW's Console History- Wiki Emulation- DarkEngine- SSB Emulation- JaboSoft- EmulNation- CKEmu- The Company 2064- E}I{'s Software- DCEmu Blog- DCEmu Interviews- DCEmu Reviews- DCEmu Tutorials- Harry Potter News- PC Gaming & Homebrew- PSP Demon- Cross & Taylors Chefs- Chui DC Dev Site- Deniska PSP Dev Site- DXDev- GPF Dev Site- PSMonkey Dev Site- ZX-81 Dev Site- Hamsterburt Dev Site- Miemt11 Dev Site- Tinnus Dev Site   About - Hosting - Donate  


Translate DCEmu 
            



   
    
The DCEmu Network 
DCEmu Network Home ·DCEmu Forums ·DC ·SAT ·PSP ·PS3 ·PS2 
·NDS ·GBA ·GIZ ·GC ·Wii ·GP2X ·iTouch iPhone ·XBOX ·N64 ·XBOX360
·DCEmu Blog ·DCEmu Interviews ·Submit News ·DCEmu Reviews ·DCEmu Tutorials  

Welcome to the DCEmu Forums:: The Homebrew & Gaming Network :: forums. 

You are currently viewing our boards as a guest which gives you limited access to view most discussions and access our other features. By joining our free community you will have access to post topics, communicate privately with other members (PM), respond to polls, upload content and access many other special features. Registration is fast, simple and absolutely free so please, join our community today! 

If you have any problems with the registration process or your account login, please contact contact us.  

   DCEmu Forums:: The Homebrew & Gaming Network :: > Nintendo Console Forums > Nintendo Wii News Forum  
 Wiimote Speaker Script!!!  
 Tags User Name  Remember Me? 
Password   
      
Register Forum Jump   vBJirc Chat Members List Calendar Arcade Today's Posts Search   Tags 


Welcome to DCEmu 
 

Forums 
DCEmu Network Forums Other Console & Homebrew Forums 
DCEmu Network Announcement News Forum  Pandora News Forum  
DCEmu Network News Forum     Pandora Discussion Forum  
DCEmu Interviews  Apple iPhone, iPod Touch, and iPod News Forum  
DCEmu Reviews    Apple iPhone, iPod Touch, and iPod Discussion Forum  
   DCEmu Reviews News Forum  The Apple Iphone & Ipod Touch Gaming News Forum  
   Games Reviews Forum  GP2X Wiz & GP2X News Forum  
   Sony PS3 Reviews Forum     GP2X Wiz, GP2X & GP32 Discussion Forum  
   Sony PSP Reviews Forum  Dingoo A320 News Forum  
   Xbox 360 Reviews Forum     Dingoo A320 Discussion Forum  
   Nintendo DS Reviews Forum  Palm Pre News Forum  
   Nintendo Wii Reviews Forum  Gamepark 32 News Forum  
   Miscellaneous Reviews Forum     GP2X Wiz, GP2X & GP32 Discussion Forum 
   User Submitted Reviews Forum  Alternative Handheld Emulation News Forum  
DCEmu Console History     Minor Handhelds Discussion Forum  
The Joypad Forums  Retro Console & Gadgets News Forum  
   The Joypad Discussion Forum     Retro Consoles and Gadgets Discussion Forum  
DCEmu Tutorials PC Gaming, Emulation & Homebrew News Forum  
   DCEmu Guides/Tutorials News Forum     PC Gaming, Hardware, Emulation and Homebrew Discussion Forum  
   Submit Guides/Tutorials Forum  Hosted Coders Sites Forum 
   PSP Guides/Tutorials Forum  ZX-81's Forum Section 
   PS2 Guides/Tutorials Forum     ZX-81's Dev and Discussion Forum  
   Nintendo DS Guides/Tutorials Forum     ZX-81's PSP News & Release Forum  
   Nintendo Wii Guides/Tutorials Forum    ZX-81's GP2X News & Release Forum  
   PS3 Guides/Tutorials Forum     ZX-81's Site News Forum  
   Xbox 360 Guides/Tutorials Forum     ZX-81's Dingoo A320 News & Release Forum  
DCEmu Blog Site  GPF's Forum Section 
The Submit News and Releases Forum     GPF's Site News Forum  
Sega Console Forums    GPF's DS News & Release Forum  
Sega Dreamcast News Forum     GPF's Dreamcast News & Release Forum  
Sega Saturn News Forum     GPF's Wii News & Release Forum  
Dreamcast Gaming, Emulation and Homebrew Forum     GPF's Mobile Phone News & Release Forum  
Dreamcast Development Forum     GPF's Dev and Discussion Forum  
Dreamcast Help Forum  Deniska`s Forums 
Dreamcast Porting Ideas     Deniska`s News Forum  
Sega Saturn and Vintage Sega Consoles Discussion Forum     Deniska`s Discussion Forum  
Chuis's Dev & Discussion Forum  Archived Developer Forums 
Sony Playstation Console Forums    PSPdemon's Forums 
PSP News Forum        PSPdemon's News Forum  
PS3 News Forum        PSPdemon's Pub  
PS2 News Forum        PSPdemon's Misc/Site News Forum  
The PSP, PS3 and PS2 Gaming News Forum        Team MAYH3M Forum  
PSP Emulation and Homebrew Forum     PSPzProject Forums 
PSP Help Forum        PSPzProject News Forum  
PSP Hacking & Development Forum        PSPzProject Discussion Forum  
PSP Gaming, Hardware and Discussion Forum     DXDev`s Forum Section 
PSP & PS3 Themes Forum        DXDev`s Development and Discussion Forum  
Porting Ideas for PSP/PS2/PS3        DxDev News Forum  
The Game Modders Forum for PSP/PS2/PS3     PSmonkey's Forums 
PS3 Gaming, Emulation & Homebrew Forum        PSmonkey's News Forum  
PS3 Hardware, Hacking and Development Forum        PSMonkey Discussion Forum  
PS3 Help & Misc Discussion Forum     Tinnus`s Official Dev Forum  
PS2 Discussion Forum     Miemt11 Official Dev Forum  
Nintendo Console Forums    Hamsterbert`s Official Dev Forum  
Nintendo DS/DSI News Forum     Plynx Official Dev Forum  
Nintendo Wii News Forum  General Discussion Forums 
The Nintendo Wii and DS Gaming News Forum  Off Topic Forum  
Gamecube News Forum     Junk  
GBA Micro & Advance News Forum     Cheats And Guides Section  
Nintendo 64 Homebrew News Forum  Art/Design Forum  
Nintendo DS/DSI Emulation and Homebrew Forum  Hardware Forum  
Nintendo DS/DSI Gaming & Hardware Forum     Dreamcast Hardware Tutorials  
Nintendo DS/DSI Help & Misc Discussion Forum     Other Console Hardware Tutorials  
Nintendo DS/DSI Hacking and Development Forum  Shop and Sellers Forum  
Nintendo DS/DSI/ Wii Friend Codes Forum  DCEmu Coding & Competition Forums  
Nintendo Consoles Porting Ideas Forum     Dream Coding Grand Prix 2005 Forum  
Nintendo Wii Emulation and Homebrew Forum     Dream Coding Grand Prix 2004 Forum  
Nintendo Wii Hacking and Development Forum     Dream Coding Grand Prix 2006 Forum  
Nintendo Wii Mii, Gaming & Hardware Forum  Gaming & Gadgets News Forum  
Nintendo Wii Help & Misc Discussion Forum  The Arboretum News Forum  
Gamecube Discussion Forum    
Nintendo64 & Vintage Nintendo Consoles Discussion Forum  
GBA Discussion Forum  
Microsoft Consoles Forum Section 
Xbox 360 News Forum  
   Xbox 360 Discussion Forum  
Xbox & Zune News Forum  
   Xbox & Zune Discussion Forum  
The Xbox 360 & Zune Gaming News Forum  

Search Forums 
     Show Threads   Show Posts  
Advanced Search 
Search Tags 
  
Advanced Search 

Go to Page... 
   

DCEmu Breaking News 
SELECTIVE FORUM FILTER You Can Now Choose what forums you want to see and want to ignore with our Selective Forum Filter, by going into Edit Options in Your USER CP you can choose the forums you want to view, you can also reset it at anytime. 

Getting the msg "The message you have entered is too short. Please lengthen your message to at least 1 characters." when trying to pos?t
To fix go to "User CP", "Edit Options" and select "Basic Editor- a simple text box"" under the "Message Editor Interface" section. Hopefully we will fix it proper soon. 

Want a quick chat? Come join on in our IRC Chat room. Click here to use our Java chat app, or use a IRC program like mIRC and join us on the Freenode server in #dcemuuk 

 
Add this thread to:   Digg   Reddit   Del.icio.us   Yahoo This   Technorati   
   Thread Tools   Search this Thread   Display Modes   
 January 3rd, 2007, 04:04    #1  
wraggster 
DCEmu Webmaster
 
 
 

Join Date: Apr 2003


Location: Nottingham, England
Posts: 75,655   Wiimote Speaker Script!!! 

--------------------------------------------------------------------------------
Carl Kenner posted a new script for Glovepie that uses the Wiimote Speaker.

Heres the details:

Wiimote Speaker Script!!! Second attempt. 
Should work in any version of GlovePIE that isn't super old. 

Code: 
// Carl Kenner's Wiimote Speaker Test script! Version 2 
// A = start sound, B = stop sound 
// Minus = decrease sample frequency 
// Plus = increase sample frequency 
// It takes a short time to start (due to delays built into Poke) 

// Change the next line to set the rate at which sound data is sent 
// BUT it must be low enough for the wiimote to respond to the B button 
// it may depend on your PC speed. Must be at least 91 for freq 13. 
pie.FrameRate = 120 Hz 

if not var.initialized then 
var.freq = 13 // Set sample rate = 3640 Hz (so computer can keep up) 
var.volume = 0x40 // volume = 40 ??? Seems to be about max 
debug = var.freq 
var.initialized = true 
end if 

if var.On and (not Wiimote.One) and (not Wiimote.Two) then 
// Report 18, send 20 bytes, square wave, 1/4 sample rate freq 
WiimoteSend(1, 0x18, 20 shl 3, 0xCC,0x33,0xCC,0x33,0xCC,0x33,0xCC,0x33,0xCC,0x33, 0xCC,0x33,0xCC,0x33,0xCC,0x33,0xCC,0x33,0xCC,0x33) 
else if var.On and Wiimote.One then 
// Report 18, send 20 bytes, square wave, 1/2 sample rate freq 
WiimoteSend(1, 0x18, 20 shl 3, 0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3, 0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3) 
else if var.On and Wiimote.Two then 
// Report 18, send 20 bytes, square wave, 1/8 sample rate freq 
WiimoteSend(1, 0x18, 20 shl 3, 0xCC,0xCC,0x33,0x33,0xCC,0xCC,0x33,0x33,0xCC,0xCC, 0x33,0x33,0xCC,0xCC,0x33,0x33,0xCC,0xCC,0x33,0x33) 
end if 


if pressed(Wiimote.A) then 
// Enable Speaker 
Wiimote.Report14 = 0x04 | Int(Wiimote.Rumble) 
// Mute Speaker 
Wiimote.Report19 = 0x04 | Int(Wiimote.Rumble) 
// Write 0x01 to register 0xa20009 
WiimotePoke(1, 0x04a20009, 0x01) 
// Write 0x08 to register 0xa20001 
WiimotePoke(1, 0x04a20001, 0x08) 
// Write 7-byte configuration to registers 0xa20001-0xa20008 
WiimotePoke(1, 0x04a20001, 0x00) 
WiimotePoke(1, 0x04a20002, 0x00) 
WiimotePoke(1, 0x04a20003, 0x00) 
WiimotePoke(1, 0x04a20004, var.freq) 
WiimotePoke(1, 0x04a20005, var.volume) // 40 
WiimotePoke(1, 0x04a20006, 0x00) 
WiimotePoke(1, 0x04a20007, 0x00) 
// Write 0x01 to register 0xa20008 
WiimotePoke(1, 0x04a20008, 0x01) 
// Unmute speaker 
Wiimote.Report19 = 0x00 | Int(Wiimote.Rumble) 
var.On = true 
end if 

if pressed(Wiimote.B) then 
var.On = false 
Wiimote.Report19 = 0x04 | Int(Wiimote.Rumble) // Mute Speaker 
Wiimote.Report14 = 0x00 | Int(Wiimote.Rumble) // Disable speaker 
end if 

if pressed(Wiimote.Plus) then 
var.freq-- 
debug = var.freq 
// Mute Speaker 
Wiimote.Report19 = 0x04 | Int(Wiimote.Rumble) 
// Write 0x01 to register 0xa20009 
WiimotePoke(1, 0x04a20009, 0x01) 
// Write 0x08 to register 0xa20001 
WiimotePoke(1, 0x04a20001, 0x08) 
// Write 7-byte configuration to registers 0xa20001-0xa20008 
WiimotePoke(1, 0x04a20001, 0x00) 
WiimotePoke(1, 0x04a20002, 0x00) 
WiimotePoke(1, 0x04a20003, 0x00) 
WiimotePoke(1, 0x04a20004, var.freq) // max volume? 
WiimotePoke(1, 0x04a20005, var.volume) 
WiimotePoke(1, 0x04a20006, 0x00) 
WiimotePoke(1, 0x04a20007, 0x00) 
// Write 0x01 to register 0xa20008 
WiimotePoke(1, 0x04a20008, 0x01) 
// Unmute speaker 
Wiimote.Report19 = 0x00 | Int(Wiimote.Rumble) 
end if 

if pressed(Wiimote.Minus) then 
var.freq++ 
debug = var.freq 
// Mute Speaker 
Wiimote.Report19 = 0x04 | Int(Wiimote.Rumble) 
// Write 0x01 to register 0xa20009 
WiimotePoke(1, 0x04a20009, 0x01) 
// Write 0x08 to register 0xa20001 
WiimotePoke(1, 0x04a20001, 0x08) 
// Write 7-byte configuration to registers 0xa20001-0xa20008 
WiimotePoke(1, 0x04a20001, 0x00) 
WiimotePoke(1, 0x04a20002, 0x00) 
WiimotePoke(1, 0x04a20003, 0x00) 
WiimotePoke(1, 0x04a20004, var.freq) // max volume? 
WiimotePoke(1, 0x04a20005, var.volume) 
WiimotePoke(1, 0x04a20006, 0x00) 
WiimotePoke(1, 0x04a20007, 0x00) 
// Write 0x01 to register 0xa20008 
WiimotePoke(1, 0x04a20008, 0x01) 
// Unmute speaker 
Wiimote.Report19 = 0x00 | Int(Wiimote.Rumble) 
end if 
__________________

Webmaster & Owner of DCEmu The Homebrew & Gaming Network
Also Webmaster of the Copy/Paste News!
PSP News - Nintendo DS News - GP2X News- Xbox Evolution - GBA Emulation - Apple Iphone, Itouch News- Gamecube Emulation - Console News - GP32 Emulation - PS2 Emulation - Dreamcast News - Sega Saturn News - PS3 Evolution - Xbox 360 News - Gizmondo Homebrew - Nintendo Wii News - DCEmu Interviews - DCEmu Blog - DCEmu Reviews - Nintendo 64 News - DCEmu Tutorials - Harry Potter News - PC Gaming - Cross & Taylors Chefs - PSP Demon - Chui - ZX-81 - Deniska - GPF - DCEmu Blog -

Lets see how long it takes for you to notice! 
      
wraggster 
View Public Profile 
Visit wraggster's homepage! 
Find More Posts by wraggster 


Visit Our Sponsor  

 
Tags: script, speaker, wiimote 



« Previous Thread | Next Thread » 


Thread Tools 
 Show Printable Version 
 Email this Page 

Display Modes 
 Linear Mode 
 Switch to Hybrid Mode 
 Switch to Threaded Mode 

Search this Thread 
   
 
Advanced Search 

 Posting Rules  
You may not post new threads
You may not post replies
You may not post attachments
You may not edit your posts

--------------------------------------------------------------------------------

vB code is On
Smilies are On
[IMG] code is On
HTML code is Off 
  
 



All times are GMT +1. The time now is 19:23.

Contact Us - DCEmu The Homebrew & Gaming Network Forums - Archive - Top  
  
   


Powered by vBulletin® Version 3.6.8
Copyright ©2000 - 2010, Jelsoft Enterprises Ltd.
vB.Sponsors 


 


  ·DCEmu.com Next Gen Gaming ·Console Hacking.Com  





  void	
CCamera::ScreenToPoint2D( int WiimoteX, int WiimoteY, Vector* ptScreen) 
{
   // stretch with the view perspective   	
    ptScreen->x = + (((2.0f * WiimoteX) / FRAMEWORK->GetVMode()->viWidth) - 1) / m_perspective[0][0] ;
    ptScreen->y = - (((2.0f * WiimoteY) / FRAMEWORK->GetVMode()->viHeight) - 1) / m_perspective[1][1] ;
    ptScreen->z = - 1.0f ;
}



#endif
