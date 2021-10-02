#include "Menu.h"
#include "Image.h"



Menu::Menu(Image* pImage, f32 x, f32 y, f32 w, f32 h,std::string Name) : 
		m_bActive(true), m_bSelected(false), m_bHightLight(false), m_bEnable(true), m_fWidth(w), m_fHeight(h), m_Fade(1.0f)
{
	m_pImage = pImage;
	m_pos.x = x;
	m_pos.y = y;
	m_Name = Name;
}

bool Menu::IsOverMenu(f32 x, f32 y)
{
	return  ((x > m_pos.x-(m_fWidth/2)) && (x < m_pos.x+(m_fWidth/2)) 
		&& (y > m_pos.y-(m_fHeight/2)) && (y < m_pos.y+(m_fHeight/2)));	
}