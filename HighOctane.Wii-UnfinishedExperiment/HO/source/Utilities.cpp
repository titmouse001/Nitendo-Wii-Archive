#include <gccore.h>
#include "Terrain.h"
#include "Singleton.h"
#include "WiiManager.h"
#include "SpriteManager.h"
#include "vehicle.h"
#include "EFB.h"
#include "Utilities.h"

void Utilities::TireMark(Vehicle& rVehicle)
{
	
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );
	
	MapLayers* pMapLayers( Wii.GetMapManager()->GetCurrentWorkingMapLayer() );


	//-----------------------------------------------------
	// CAMERA - Use the frame as a scratch buffer before we drawn anything to keep.
	//			Anything done here will be overdrawn later.
	Mtx cameraMatrix, FinalMatrix,TransMatrix; 
	guMtxIdentity(TransMatrix);  
	// Put the camera in the centre
	// so possition 0,0 is top left (45+45deg FOV, so later we use a cam height to match)
	f32 CamX=Wii.GetScreenWidth()/2;  
	f32 CamY=Wii.GetScreenHeight()/2; 
	guVector camera = { CamX,CamY, -Wii.GetScreenHeight()/2 }; 
	guVector up =	{0.0F, -1.0F, 0.0F};  
	guVector look	= {CamX,CamY, 0.0F};
	guLookAt(cameraMatrix, &camera,	&up, &look);

	guMtxTransApply(TransMatrix,TransMatrix,0, 0,0);	// origin
	guMtxConcat(cameraMatrix,TransMatrix,FinalMatrix);
	GX_LoadPosMtxImm (FinalMatrix, GX_PNMTX0);     
	//----------------------------------------------------------


	Image* pImage(Wii.GetSpriteManager()->GetImage( ( rVehicle.GetDirection() / (M_PI*2.0f) )*64 + 64 ));
	f32 CentreVehicleX ( rVehicle.GetPosition().x - (pImage->GetWidth() / 2) );
	f32 CentreVehicleY ( rVehicle.GetPosition().y - (pImage->GetHeight() / 2) );

	// draw blocks 3x3
	for (int y(0); y<3; y++)
	{
		for (int x(0); x<3; x++)
		{
			MapSize BlockID( (*pMapLayers->GetBegin())->GetValueFromOrigin( (x*16) + CentreVehicleX, (y*16) + CentreVehicleY ) );
			Wii.GetImageManager()->GetImage(BlockID & 0x7fff)->DrawImageNoAlphaFor3D((x*16),(y*16)); 
		}
	}

	// draw the wheel marks
	pImage->DrawImageFor3D( ((int)rVehicle.GetPosition().x&0xf ), ((int)rVehicle.GetPosition().y&0xf ), 0x3f );

	// One drawback to this method is that re-sampling the same screen area causes smudging

	GX_DrawDone();
	GX_InvalidateTexAll();
	GX_Flush();

	for (int y(0); y<3; y++)
	{
		for (u8 x(0);x<3;x++)
		{
			MapSize ID( (*pMapLayers->GetBegin())->GetValueFromOrigin((16*x) + CentreVehicleX, (y*16) + CentreVehicleY) );

			if ( !(ID & 0x8000) )
			{
				// Copy texture as new image
				Wii.GetImageManager()->AddImage(EFB::NewTextureFromGX(x*16,y*16,16,16)); 
				(*pMapLayers->GetBegin())->SetValueFromOrigin( (16*x) + CentreVehicleX, (y*16) + CentreVehicleY, 
					0x8000 | (MapSize)( Wii.GetImageManager()->GetImageCount()-1 ));
			}
			else
			{
				// This texture has already been modified before
				Image* pImage( Wii.GetImageManager()->GetImage(ID & 0x7fff) );
				u32* pData( pImage->GetImageData() );
				EFB::CopyTextureFromGX(pData,x*16,y*16,16,16);
			}
		}
	}
}