#ifndef MenuScreens_H
#define MenuScreens_H

class Timer;
class WiiManagewr;

class MenuScreens
{
public:
	MenuScreens();
	void Init();
	void DoCreditsScreen();
	void DoMenuScreen();
	void DoControlsScreen();
	void DoOptionsScreen();

	void SetTimeOutInSeconds(int Value = 121);
	bool HasMenuTimedOut();


private:
	float		m_ZoomAmountForSpaceBackground;
	Timer*		m_pTimer;
	WiiManager*	m_pWii;
	GameLogic*	m_pGameLogic;
};

#endif