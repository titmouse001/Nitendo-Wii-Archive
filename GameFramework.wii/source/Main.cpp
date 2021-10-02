//
// -= Game Framework, ver0.1 =- 
//
// Framework stripped from the Wii homebrew game 'Bolt Thrower', Paul Overy - 2010
// Sice this framework has been stripped from my game which is still in progress there may well
// be some mistakes/fudges and even some bad hacks.

// see Configuration.xml to see what things are loaded at game startup
// see http://www.sgi.com/tech/stl/ for c++ STL guide
// see WiiManager, this is a Singleton class and is the frameworks backbone (first line in main gets things ready)
// see Config.h for build mode & GetGamePath() in WiiFile  - keeps the games working path dir your files are loaded from SD card when debugging
// see Google for all other things... pretty please with sugar on top


#include <math.h>
#include "Singleton.h"
#include "WiiManager.h"
#include "Util3d.h"
#include "HashString.h"
#include "WiiFile.h"
#include "ImageManager.h"
#include "FontManager.h"
#include "Image.h"
#include "wiiuse\wpad.h"


// Please look in Config.h before compiling
// Set LAUNCH_VIA_WII to run from SD card or LAUNCH_VIA_WIISEND for a wireless conection via 'WiiLoad' & 'Homebrew channel'
// The framework package comes with LAUNCH_VIA_WII set ready to run from SD card.
// I tend to use the Dolphin emulator - so I use LAUNCH_VIA_WIISEND or LAUNCH_VIA_WII_EMULATOR while developing.


int main(int argc, char **argv) 
{	
	WiiManager& Wii ( Singleton<WiiManager>::GetInstanceByRef() );

	Wii.InitWii(); // do this first
		
	// Configuration.xml is stored on your SD card alongside your dol or elf file, GetXmlVariable() gives us access to this information.
	Wii.ProgramStartUp(WiiFile::GetGamePath() + "Configuration.xml"); // See xml file to see what's loaded

	// now ready to use GetXmlVariable() ... the call to ProgramStartUp() is needed before we can use GetXmlVariable
	WPAD_SetIdleTimeout(Wii.GetXmlVariable( HashString::WiiMoteIdleTimeoutInSeconds )); // example use of GetXmlVariable

	int Value = Wii.GetXmlVariable( "AnotherExampleVariable" ); // another GetXmlVariable example to get values from the XML config file
	printf ("%d",Value);
	

	Wii.GetCamera()->InitialiseCamera();  // this view defaults to top left.

	// The Image class can load TGA's, PNG's or JPG's - Each have there uses.
	//Image* pTestImage = new Image(Util::GetGamePath() + "Test.TGA"); // No support for image descriptor bits (vh flip bits)
	//Image* pTestImage = new Image(Util::GetGamePath() + "Test.PNG"); // uses -lpng
	Image* pTestImage = new Image(WiiFile::GetGamePath() + "Test.JPG");  // Avoid using progressive jpegs

	Vtx WiiMote = {0,0,0};
	do
	{	
		// Zbuffer compare & write needs to be of for flat 2D stuff
		GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_FALSE);
	
		// WiiMote
		WPAD_ScanPads();
		WPADData* pPadData(WPAD_Data(0));
		if ((pPadData!=NULL) && (pPadData->ir.valid)) 
		{	
			Wii.GetCamera()->SetCameraView( (Wii.GetScreenWidth()/2), (Wii.GetScreenHeight()/2) ); // view to top left

			WiiMote.x = pPadData->ir.x;
			WiiMote.y = pPadData->ir.y;
			WiiMote.z = pPadData->ir.z;
			float RadAngle( (pPadData->orient.roll/180.0f)*M_PI ); // returns degrees (bit odd)
			int alpha=255;
			float z=0.0f;
			pTestImage->DrawImageXYZ(WiiMote.x,WiiMote.y,z,alpha,RadAngle);
		}

		Wii.GetCamera()->SetCameraView( 0, 0) ; // centre the view 

		string Text("The quick brown fox jumps over the lazy dog");
		static float spin(0.0f);	
		Util3D::TransRot(0,0,sin(spin)*550.0f,spin); // zoom & rotate - origin of font
		spin+=0.01f;
		Wii.GetFontManager()->DisplayLargeTextCentre(Text,0,0,166); // params: Message, X, Y, Colour Alpha
		
		Util3D::Trans(-320,-240,0);
		Wii.GetFontManager()->DisplayLargeText(Text,0,0,200);
		
		Util3D::TransRot(-320+Wii.GetFontManager()->GetFont(HashString::LargeFont)->GetHeight() ,-240,0,(M_PI/2));
		Wii.GetFontManager()->DisplayLargeText(Text,0,0,80);

		Util3D::Trans(0,0);
		Wii.GetFontManager()->DisplaySmallText(Text,0,0,255);
		Wii.GetFontManager()->DisplaySmallTextCentre(Text,0,32,255);
		
		// 'Printf' is really intended for debug
		Wii.Printf(WiiMote.x,WiiMote.y + pTestImage->GetHeight()/2,"pPadData->ir.valid = %d",pPadData->ir.valid);
		Wii.Printf(WiiMote.x,WiiMote.y + pTestImage->GetHeight()/2+20,"debug text %f",spin);

		GX_SetZMode (GX_TRUE, GX_LEQUAL, GX_TRUE);
		Wii.SwapScreen();  // to clear zbuffer keep GX_SetZMode on until after this call 
		
	} while ( ( WPAD_ButtonsUp(0) & WPAD_BUTTON_HOME ) == 0 );

	Wii.UnInitWii();

	return 0;
}