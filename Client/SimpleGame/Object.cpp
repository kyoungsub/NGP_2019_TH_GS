#include "stdafx.h"
#include "Object.h"
#include "Global.h"
#include <math.h>
#include <float.h>

Object::Object(){
	
}

Object::~Object()
{
}

void Object::GetPos(float * x, float * y, float * z)
{
	*x = m_PosX;
	*y = m_PosY;
	*z = m_PosZ;
}

void Object::SetPos(float x, float y, float z)
{
	m_PosX = x;
	m_PosY = y;
	m_PosZ = z;
}

void Object::GetSize(float *sx, float *sy, float *sz)
{
	*sx = m_SizeX;
	*sy = m_SizeY;
	*sz = m_SizeZ;
}

void Object::SetSize(float sx, float sy, float sz)
{
	m_SizeX = sx;
	m_SizeY = sy;
	m_SizeZ = sz;
}

void Object::GetColor(float * r, float * g, float * b, float * a)
{
	*r = m_ColorR;
	*g = m_ColorG;
	*b = m_ColorB;
	*a = m_ColorA;
}

void Object::SetColor(float r, float g, float b, float a)
{
	m_ColorR = r;
	m_ColorG = g;
	m_ColorB = b;
	m_ColorA = a;
}

void Object::GetVel(float * x, float * y, float *z)
{
	*x = m_VelX;
	*y = m_VelY;
	*z = m_VelZ;
}

void Object::SetVel(float x, float y, float z)
{
	m_VelX = x;
	m_VelY = y;
	m_VelZ = z;
}

void Object::GetAcc(float * x, float * y, float *z)
{
	*x = m_AccX;
	*y = m_AccY;
	*z = m_AccZ;
}

void Object::SetAcc(float x, float y, float z)
{
	m_AccX = x;
	m_AccY = y;
	m_AccZ = z;
}

void Object::GetMass(float * x)
{
	*x = m_Mass;
}

void Object::SetMass(float x)
{
	m_Mass = x;
}

void Object::GetCoefFrict(float * x)
{
	*x = m_CoefFrict;
}

void Object::SetCoefFrict(float x)
{
	m_CoefFrict = x;
}

void Object::SetKind(int Kind)
{
	m_Kind = Kind;
}

void Object::GetKind(int *Kind)
{
	*Kind = m_Kind;
}

void Object::SetHP(int HP)
{
	m_HP = HP;
}

void Object::GetHP(int * HP)
{
	*HP = m_HP;
}

void Object::SetState(int State)
{
	m_State = State;
}

void Object::GetState(int * State)
{
	*State = m_State;
}

void Object::Update(float eTime)
{
	float magvel = sqrtf(m_VelX * m_VelX + m_VelY * m_VelY + m_VelZ * m_VelZ);
	float velx = m_VelX / magvel;
	float vely = m_VelY / magvel;
	float velz = m_VelZ / magvel;

	float fricX = -velx;
	float fricY = -vely;
	float friction = m_CoefFrict * m_Mass * 9.8f;

	fricX *= friction;
	fricY *= friction;

	if(magvel < FLT_EPSILON){
		m_VelX = 0.0f;
		m_VelY = 0.0f;
		m_VelZ = 0.0f;
	}
	else {
		float accx = fricX / m_Mass;
		float accy = fricY / m_Mass;

		float aftervelx = m_VelX + accx * eTime;
		float aftervely = m_VelY + accy * eTime;

		if (aftervelx * m_VelX < 0.0f) {
			m_VelX = 0.0f;
		}
		else {
			m_VelX = aftervelx;
		}

		if (aftervely * m_VelY < 0.0f) {
			m_VelY = 0.0f;
		}
		else {
			m_VelY = aftervely;
		}

		// Gravity
		m_VelZ = m_VelZ - GRAVITY * eTime;
	}

	// Cal Velocity
	m_VelX = m_VelX + m_AccX * eTime;
	m_VelY = m_VelY + m_AccY * eTime;
	m_VelZ = m_VelZ + m_AccZ * eTime;

	// Cal Position
	m_PosX = m_PosX + m_VelX * eTime;
	m_PosY = m_PosY + m_VelY * eTime;
	m_PosZ = m_PosZ + m_VelZ * eTime;

	if (m_PosZ <= 0.0f) {
		m_State = STATE_GROUND;
		m_VelZ = 0.0f;
		m_PosZ = 0.0f;
	}
	else {
		m_State = STATE_AIR;
	}
}

void Object::ApplyForce(float x, float y, float z, float eTime)
{
	//Calc Acc a = f/m
	m_AccX = x / m_Mass;
	m_AccY = y / m_Mass;
	m_AccZ = z / m_Mass;

	m_VelX = m_VelX + m_AccX * eTime;
	m_VelY = m_VelY + m_AccY * eTime;
	m_VelZ = m_VelZ + m_AccZ * eTime;

	m_AccX = 0.0f;
	m_AccY = 0.0f;
	m_AccZ = 0.0f;
}