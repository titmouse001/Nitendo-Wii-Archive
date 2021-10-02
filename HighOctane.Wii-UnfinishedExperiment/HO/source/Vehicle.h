#ifndef Vehicle_H
#define Vehicle_H

#include "GCTypes.h"

struct Vector
{
	f32 x,y,z;
};

// note: 
// To calculate the magnitude of the velocity vector (speed) use: sqrt(v.x*v.x + v.y*v.y); 
// traction = DirectionVector * Engineforce, 

//  speed = sqrt(v.x*v.x + v.y*v.y); 
//    fdrag.x = - drag * v.x * speed; 
//    fdrag.y = - drag * v.y * speed;

class Vehicle 
{
public:

	Vehicle(f32 x,f32 y,f32 z);
	void	Set(f32 x,f32 y,f32 z);

	Vector& GetPosition() { return m_Position; }
	const Vector& 	GetVelocityVector() const  { return m_VelocityVector; }

	void	SetDirection(f32 fValue) { m_Direction = fValue; }
	f32		GetDirection() const  { return m_Direction; }
	void	TurnDrivingDirection(f32 fValue);
	f32		GetStearingDirection() const { return m_StearingDirection; }

	void	AddSpeed(float Speed) { m_Speed += Speed; }
	float	GetSpeed() const {return  m_Speed; }
	void	SetSpeed(float Speed) {  m_Speed = Speed; }


	void	Add(f32 x,f32 y,f32 z);
	void	Add(const Vector& v);

	void	Drive();

	void	Draw();

private:

	Vehicle() {} // makes sure we use overloaded public version

	f32		m_Speed;

	f32		m_AirResistance;
	f32		m_CurfaceFriction; 

	f32		m_EngineForce;
	f32		m_TransmissionEfficiency; 

	Vector	m_VelocityVector;
	f32		m_Direction;
	f32		m_StearingDirection;
	Vector	m_Position;

	bool	m_bStearingUsed;

	void	TurnSteeringWheel(f32 fValue);
	void	CentreStearingDirection();
};

#endif