#ifndef FridgeMagnets_H
#define FridgeMagnets_H

#include "GCTypes.h"
#include "ogc/gx.h"
#include "wiiuse/wpad.h"
#include <malloc.h>
#include <string.h>
#include <gccore.h>
#include <string>
#include "WiiManager.h"
#include "FontManager.h"

#include <map>

using namespace std;

class CharInfo;


class FridgeMagnet
{

public:

//	FridgeMagnet(float x,float y, char& Character);
	FridgeMagnet(float x,float y, const char& Character);

	float GetX() { return m_Pos.x; }
	float GetY() { return m_Pos.y; }
	float GetZ() { return m_Pos.z; }
	void SetZ( float value) { m_Pos.z=value; }
	void SetY( float value) { m_Pos.y=value; }
	void SetX( float value) { m_Pos.x=value; }
	guVector& GetPos() { return  m_Pos; }

	void SetPos(float x, float y, float z) { m_Pos.x = x; m_Pos.y = y; m_Pos.z = z; }
	void SetPos(guVector& Pos) { m_Pos = Pos; }

	float GetAngle() { return m_Angle; }
	FridgeMagnet* SetAngle(float Value) { m_Angle = Value;  return this;}

	float GetScale() { return m_Scale; }
	FridgeMagnet* SetScale(float Value) { m_Scale = Value; return this; }

	float GetMinScale() { return m_MinScale; }
	FridgeMagnet* SetMinScale(float Value) { m_MinScale = Value; return this;}

	void SetCharacter(char& Char)			{ m_Character = Char; }
	void SetCharacter(const char& Char)			{ m_Character = Char; }
	char& GetCharacter()	{ return m_Character; }

	void Display();
	void DisplayShadow(u8 alpha, float Scale);
	FridgeMagnet* SetColour(u8 Red,u8 Green,u8 Blue, u8 Alpha);


	GXColor& GetColour() { return m_Colour; }
	guVector  m_Velocity;

	bool IsDisabled() const { return m_bDisable; }
	FridgeMagnet* SetDisable(bool value=true) { m_bDisable = value; return this;}

	bool	bInHand;

private:


	FridgeMagnet() { Init(); }

	void Init();

	guVector			m_Pos; 
	//std::string			m_Character;
	char				m_Character;
	GXColor				m_Colour;
	float				m_Angle;
	float				m_Scale;
	float				m_MinScale;

	bool				m_bDisable;

};



class FridgeMagnetManager
{
public:

	FridgeMagnetManager();

	FridgeMagnet* HitsFridgeMagnet(float x, float y, float area = 20.0f)
	{
		for (std::vector<FridgeMagnet*>::iterator iter(GetBegin()); iter!=GetEnd(); ++iter )
		{
			FridgeMagnet* pFridgeMagnet(*iter);
			if ((x>pFridgeMagnet->GetX()-area) && (x<pFridgeMagnet->GetX()+area) && 
				(y>pFridgeMagnet->GetY()-area) && (y<pFridgeMagnet->GetY()+area))
			{
				return pFridgeMagnet;
			}
		}
		return NULL;
	}

	FridgeMagnet* AddNewFridgeMagnet(float x, float y, const char& Character)
	{
		FridgeMagnet* pFridgeMagnet = new FridgeMagnet(x,y,Character);
		Add(pFridgeMagnet);
		return pFridgeMagnet;
	}

	//FridgeMagnet* AddNewFridgeMagnet(float x, float y, char& Character)
	//{
	//	return AddNewFridgeMagnet( x,  y, (const char&) Character);
	//	//FridgeMagnet* pFridgeMagnet = new FridgeMagnet(x,y,Character);
	//	//Add(pFridgeMagnet);
	//	//return pFridgeMagnet;
	//}

	void RemoveLastFridgeMagnet()
	{
		if (m_FridgeMagnetContainer.size()>1)
		{
			delete m_FridgeMagnetContainer.back();
			m_FridgeMagnetContainer.pop_back();
		}
	}

	void RemoveAllFridgeMagnets()
	{
		for (std::vector<FridgeMagnet*>::iterator iter(GetBegin()); iter!=GetEnd(); ++iter )
		{
			delete *iter;
		}
		m_FridgeMagnetContainer.clear();

		m_pSelectedFridgeMagnet[0]=NULL;
		m_pSelectedFridgeMagnet[1]=NULL;
		m_pSelectedFridgeMagnet[2]=NULL;
		m_pSelectedFridgeMagnet[3]=NULL;
	}

	void RemoveFridgeMagnet(FridgeMagnet* pFridgeMagnet)
	{
		if (pFridgeMagnet==NULL)
			return;

		for (std::vector<FridgeMagnet*>::iterator iter(GetBegin()); iter!=GetEnd(); ++iter )
		{
			if (pFridgeMagnet == *iter)
			{
				iter = m_FridgeMagnetContainer.erase(iter);
				break;
			}
		}
		delete pFridgeMagnet;
	}

	std::vector<FridgeMagnet*>::iterator GetBegin() { return m_FridgeMagnetContainer.begin(); }
	std::vector<FridgeMagnet*>::iterator GetEnd() { return m_FridgeMagnetContainer.end(); }
	
	std::map<char, GXColor> LetterColourContainer;

	void FridgeMagnetText(float x, float y, std::string Text, float WobbleFactor = 0.25, float Scale=1.0 )
	{
		float xpos(x);
		float ypos(y);
		for (std::string::iterator iter(Text.begin()); iter!=Text.end(); ++iter)
		{
			u8 red = LetterColourContainer[ *iter ].r;
			u8 green = LetterColourContainer[ *iter ].g;
			u8 blue = LetterColourContainer[ *iter ].b;

			//AddNewFridgeMagnet(xpos,ypos,&(*iter))
			AddNewFridgeMagnet( xpos, ypos, *iter )
				->SetColour(red,green,blue,0xff)
				->SetAngle( ( ( (float)rand()/RAND_MAX ) - 0.5f ) * WobbleFactor )
				->SetScale(Scale)
				->SetMinScale(Scale);

			xpos += 38.0f*Scale;
		}
	}


	void DisableAllFridgeMagnets(bool bFlag = true)
	{
		for (std::vector<FridgeMagnet*>::iterator iter(GetBegin()); iter!=GetEnd(); ++iter )
		{
			(*iter)->SetDisable( bFlag );
		}
	}

	
	void RemoveEnabledFridgeMagnets()
	{
		std::vector<FridgeMagnet*>::iterator iter( GetBegin() );
		while ( iter!=GetEnd() )
		{
			if ( (*iter)->IsDisabled() == false )
			{
				delete (*iter);
				iter = m_FridgeMagnetContainer.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}


	void SelectLogic( u8 WiiControll=WPAD_CHAN_0);
	void MotionLogic(float DropAmount, float StopAtGround);
	void DoControls(u8 Contoller);
	void DrawWiiMotePointer(float x, float y, u8 r,u8 g, u8 b, float Angle, int Image);

	void DoMainDisplay();
	void DoDisplayAllControllers();

	float m_AngleForOnscreenPointer;
	FridgeMagnet* m_pSelectedFridgeMagnet[4];

	void AddPickerCircleLetters(std::string SelectionMenu, float Scale,float LetterSize);

private:

	void Add(FridgeMagnet* pFridgeMagnet) {m_FridgeMagnetContainer.push_back(pFridgeMagnet); }

	std::vector<FridgeMagnet*> m_FridgeMagnetContainer;
};



#endif