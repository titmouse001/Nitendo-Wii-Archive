#include "InputDeviceManager.h"



InputDeviceManager::InputDeviceManager( int Value )
{ 
	WPAD_SetIdleTimeout(Value); 
}


void InputDeviceManager::Store()
{
	WPAD_ScanPads();

//TEMP just 1 InputDeviceManager for now

	m_PadButtonHeldState = WPAD_ButtonsHeld(0);
	
    // m_PadButtonHeldState = WPAD_ButtonsDown(0);

	for (s32 PadCount(0); PadCount<1; ++PadCount)
	{
		WPADData* pPadData(WPAD_Data(PadCount));

		if (pPadData->ir.valid) 
		{	
			Vtx VertexFor2D;
			VertexFor2D.x = pPadData->ir.x;
			VertexFor2D.y = pPadData->ir.y;
			VertexFor2D.z = pPadData->ir.z;

			if (ControllContainer[PadCount].size()>=100)
			{
				ControllContainer[PadCount].erase(ControllContainer[PadCount].begin());
			}

			ControllContainer[PadCount].push_back(VertexFor2D);
		}
	}
}

Vtx* InputDeviceManager::GetIRPosition()
{
	u32 IRPadNumber(0);
	if (ControllContainer[IRPadNumber].empty())
	{
		return NULL;
	}
	else
	{
		return &ControllContainer[IRPadNumber].back();
	}
}

std::vector<Vtx> InputDeviceManager::GetIRPositionContainer()
{
	u32 IRPadNumber(0);
	return ControllContainer[IRPadNumber];
}
