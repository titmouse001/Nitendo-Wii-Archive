#ifndef Menu_H
#define Menu_H

#include "GCTypes.h"
#include "ogc/gx.h"
#include <string>

class Image;

class Menu
{

public:

	Menu(Image* pImage, f32 x, f32 y, f32 w, f32 h,std::string Name);

	guVector& GetPos() { return m_pos; }
	void SetPos(f32 x, f32 y) { m_pos.x = x; m_pos.y = y; }

	void SetEnable(bool bState) { m_bEnable = bState; }
	bool GetEnable() const { return m_bEnable; }

	bool IsOverMenu(f32 x, f32 y);

	Image* GetImage() const	 { return m_pImage; }
	void SetImage(Image* pImage) { m_pImage = pImage; }

	bool GetHightLight() { return m_bHightLight; }
	void SetHightLight(bool bState) { m_bHightLight = bState; }

	void SetSelected(bool bState) { m_bSelected = bState; }
	bool GetSelected() { return m_bSelected;}

	std::string GetName() { return m_Name; }
	void SetName(std::string Name) { m_Name = Name; }

	void SetActive(bool bState) { m_bActive = bState; }
	bool GetActive() { return m_bActive;}


	void SetFadeValue(float Fade) { m_Fade = Fade; }
	void AddFadeValue(float Fade) { m_Fade += Fade; }
	float GetFadeValue() { return m_Fade;}

private:

	Image*		m_pImage;
	guVector	m_pos;
	bool		m_bActive;
	bool		m_bSelected;
	bool		m_bHightLight;
	bool		m_bEnable;
	f32			m_fWidth;
	f32			m_fHeight;
	std::string	m_Name;

	float		m_Fade;
};


#endif
