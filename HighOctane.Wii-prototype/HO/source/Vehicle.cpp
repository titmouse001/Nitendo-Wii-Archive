#include "Vehicle.h"
#include "math.h"
#include "WiiManager.h"
#include "MapManager.h"
#include "SpriteManager.h"
#include "ogc\gu.h"

#include "FontManager.h"

Vehicle::Vehicle(f32 x,f32 y,f32 z) :  m_Direction(0.0f), m_StearingDirection(0.0f), m_bStearingUsed(false)
{ 
	Set(x,y,z); 
}

void Vehicle::Set(f32 x,f32 y,f32 z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void Vehicle::Add(f32 x,f32 y,f32 z)
{
	m_Position.x += x;
	m_Position.y += y;
	m_Position.z += z;
}

void Vehicle::Add(const Vector& v)
{
	Add(v.x, v.y, v.z);
}

void Vehicle::Drive()
{
	TurnSteeringWheel( GetStearingDirection() );

	m_VelocityVector.x =   sin(m_Direction / ((M_PI*2.0f)/64.0f) * ((M_PI*2.0f)/64.0f) )*m_Speed;
	m_VelocityVector.y = - cos(m_Direction / ((M_PI*2.0f)/64.0f) * ((M_PI*2.0f)/64.0f) )*m_Speed;

	Add( m_VelocityVector );

	WiiManager& Wii = Singleton<WiiManager>::GetInstanceByRef();
	MapLayers* pMapLayer(Wii.GetMapManager()->GetCurrentWorkingMapLayer());
	vector<Map*>::iterator pMap(pMapLayer->GetBegin());

	// TEMP - CATCH OFF MAP
	if ( (m_Position.x < 0) || (m_Position.y < 0) || 
		 (m_Position.x > (*pMap)->GetTotalPixelWidth())  )
	{
		m_Position.x -= m_VelocityVector.x;
		m_Position.y -= m_VelocityVector.y;

		m_Speed = -m_Speed*0.25;
//		m_VelocityVector.x = -m_VelocityVector.x;
//		m_VelocityVector.y = -m_VelocityVector.y;
	}

	m_Speed = m_Speed * 0.991;

	CentreStearingDirection();
}

void	Vehicle::TurnSteeringWheel(f32 fValue) 
{
	m_Direction += fValue; 

	if (m_Direction < 0)
	{
		m_Direction = m_Direction + (M_PI * 2.0f);
	}
	else if (m_Direction > M_PI * 2.0f)
	{
		m_Direction =  (M_PI * 2.0f) - m_Direction;
	}
}

void	Vehicle::TurnDrivingDirection(f32 fValue)
{
	m_bStearingUsed = true;  //todo ... when value is zero?
	m_StearingDirection += fValue;

	static const float TopAmount( ((M_PI*2.0f) / 64.0f) * 1.0f );

	if (m_StearingDirection < -TopAmount)
		m_StearingDirection = -TopAmount;
	else if (m_StearingDirection > TopAmount)
		m_StearingDirection = TopAmount;
}

void	Vehicle::CentreStearingDirection()
{
	if (!m_bStearingUsed)
	{
		m_StearingDirection = m_StearingDirection * 0.95;
	}
	m_bStearingUsed = false;
}



void Vehicle::Draw()
{
	WiiManager& Wii( Singleton<WiiManager>::GetInstanceByRef() );
	Image* const pImage( Wii.GetSpriteManager()->GetImage( (GetDirection()/(M_PI*2.0f))*64 ) );


			static Vector lastpos;
			Vector v2 = GetPosition();

			float distance_x = v2.x - lastpos.x;
			float distance_y = v2.y - lastpos.y; 
			float diffx = sqrt( (distance_x * distance_x) + (distance_y * distance_y));
float diffx2 = ( (distance_x * distance_x) + (distance_y * distance_y));

			lastpos = GetPosition();


//	char text[128];
//	//sprintf(text,"%d %4.3f",  (int) ( (GetDirection()/(M_PI*2.0f))*64 )  ,GetDirection() );
//	sprintf(text,"%f %f",  diffx, diffx2 );
//	Wii.GetFontManager()->GetFont()->DisplayText(text,m_Position.x,m_Position.y-50);

	Vector& v(GetPosition());
	pImage->DrawImageFor3D( v.x -(pImage->GetWidth()/2), v.y -(pImage->GetHeight()/2));
}