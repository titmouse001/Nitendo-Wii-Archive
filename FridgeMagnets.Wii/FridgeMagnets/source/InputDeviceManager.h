#ifndef InputDeviceManager_H_
#define InputDeviceManager_H_

#include <gccore.h>
#include <vector>
#include <wiiuse/wpad.h>


class InputDeviceManager
{
public:

	InputDeviceManager( int Value = 60 );

	void Store();

	vec3f_t* GetIRPosition(u8 IRPadNumber);
	//std::vector<Vtx> GetIRPositionContainer(u8 IRPadNumber);
	u32 GetPadButton(u8 Controller) const { return WPAD_ButtonsHeld(Controller) ;} // m_PadButtonHeldState; }

	float GetAngle(u8 Item) const  { return m_AngleContainer[Item]; }

	bool IsWiiMoteReady(s32 Chan)
	{
		u32 Status;
		return (WPAD_Probe(Chan, &Status) != WPAD_ERR_NONE);
	}

	int	m_HandOver[4];

private:

	//u32 m_PadButtonHeldState;
	std::vector<vec3f_t>	ControllContainer[4];

	float m_AngleContainer[4];


	

};


#endif