#include "InputDeviceManager.h"

#include "debug.h"

InputDeviceManager::InputDeviceManager( int Value )
{ 
//	WPAD_SetIdleTimeout(Value); 
}


void InputDeviceManager::Store()
{
	WPAD_ScanPads();

//	m_PadButtonHeldState = WPAD_ButtonsHeld(0);
    // m_PadButtonHeldState = WPAD_ButtonsDown(0);

	for (s32 PadCount(0); PadCount<4; ++PadCount)
	{
		u32 Status;
		if  (WPAD_Probe(PadCount, &Status) == WPAD_ERR_NONE)
		{
			WPADData* pPadData( WPAD_Data(PadCount) );

			if ((pPadData != NULL) && (pPadData->ir.valid)) 
			{	
				vec3f_t VertexFor2D;
				VertexFor2D.x = pPadData->ir.x;
				VertexFor2D.y = pPadData->ir.y;
				VertexFor2D.z = pPadData->ir.z;

				if (ControllContainer[PadCount].size()>=10)
				{
					ControllContainer[PadCount].erase(ControllContainer[PadCount].begin());
				}

				float RadAngle( (pPadData->orient.roll/180.0f)*M_PI ); // hardware in degrees, bit odd!

				m_AngleContainer[PadCount] = RadAngle;

				ControllContainer[PadCount].push_back( VertexFor2D );
			}
		}
	}
}

vec3f_t* InputDeviceManager::GetIRPosition(u8 IRPadNumber)
{
	if (ControllContainer[IRPadNumber].empty())
	{
		// had a bad onscreen effect ... need to look into why it covered the whole screen
//		vec3f_t Vertex;
//		Vertex.x = -100.0f; // hide off screen
//		Vertex.y = -100.0f;
//		Vertex.z =  0.0f;
//		ControllContainer[IRPadNumber].push_back( Vertex );
//
		return NULL;
	}
	
	return &ControllContainer[IRPadNumber].back();
}

//std::vector<Vtx> InputDeviceManager::GetIRPositionContainer(u8 IRPadNumber)
//{
//	u32 IRPadNumber(0);
//	return ControllContainer[IRPadNumber];
//}
