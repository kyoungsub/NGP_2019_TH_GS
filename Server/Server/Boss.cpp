#pragma once
#include "Datastruct.h"
#include "Boss.h"

int g_SeqB = 0;

Boss::Boss()
{
	std::random_device randDeivce;
	std::default_random_engine randEngine{ randDeivce() };
	std::uniform_real_distribution<float> distX { 0.01f, 0.05f };
	std::uniform_real_distribution<float> distY { 0.01f, 0.05f };
	std::uniform_int_distribution<int> distDir{ 0, 3 };

	SetDelta(distX(randEngine), distY(randEngine), 0.0f);
	switch (distDir(randEngine))
	{
	case 0:
		SetDir(1, 1, 0);
		break;
	case 1:
		SetDir(-1, 1, 0);
		break;
	case 2:
		SetDir(1, -1, 0);
		break;
	case 3:
		SetDir(-1, -1, 0);
		break;
	default:
		break;
	}
}

void Boss::InitBoss()
{
	BossUnit.posX = 1.5f;
	BossUnit.posY = 0.0f;
	BossUnit.posZ = 0.0f;

	BossUnit.velX = 0.0f;
	BossUnit.velY = 0.0f;
	BossUnit.velZ = 0.0f;

	BossUnit.accX = 0.0f;
	BossUnit.accY = 0.0f;
	BossUnit.accZ = 0.0f;

	BossUnit.mass = 0.15f;
	BossUnit.coefFriction = 5.0f;
	BossUnit.type = KIND_BOSS;
	BossUnit.HP = 200;
	BossUnit.State = STATE_GROUND;

	BossUnit.idx_num = 0;
	BossUnit.elapsed_time = 0.0f;
}


Boss::~Boss()
{
}

void Boss::Update(float eTime)
{
	//Movemenet Update
	{
		float magvel = sqrtf(BossUnit.velX * BossUnit.velX 
			+ BossUnit.velY * BossUnit.velY + BossUnit.velZ * BossUnit.velZ);
		float velx = BossUnit.velX / magvel;
		float vely = BossUnit.velY / magvel;
		float velz = BossUnit.velZ / magvel;

		float fricX = -velx;
		float fricY = -vely;
		float friction = BossUnit.coefFriction * BossUnit.mass * 9.8f;

		fricX *= friction;
		fricY *= friction;

		if (magvel < FLT_EPSILON) {
			BossUnit.velX = 0.0f;
			BossUnit.velY = 0.0f;
			BossUnit.velZ = 0.0f;
		}
		else {
			float accx = fricX / BossUnit.mass;
			float accy = fricY / BossUnit.mass;

			float aftervelx = BossUnit.velX + accx * eTime;
			float aftervely = BossUnit.velY + accy * eTime;

			if (aftervelx * BossUnit.velX < 0.0f) {
				BossUnit.velX = 0.0f;
			}
			else {
				BossUnit.velX = aftervelx;
			}

			if (aftervely * BossUnit.velY < 0.0f) {
				BossUnit.velY = 0.0f;
			}
			else {
				BossUnit.velY = aftervely;
			}

			// Gravity
			BossUnit.velZ = BossUnit.velZ - GRAVITY * eTime;
		}

		// Cal Velocity
		BossUnit.velX = BossUnit.velX + BossUnit.accX * eTime;
		BossUnit.velY = BossUnit.velY + BossUnit.accY * eTime;
		BossUnit.velZ = BossUnit.velZ + BossUnit.accZ * eTime;

		// Cal Position
		BossUnit.posX = BossUnit.posX + BossUnit.velX * eTime;
		BossUnit.posY = BossUnit.posY + BossUnit.velY * eTime;
		BossUnit.posZ = BossUnit.posZ + BossUnit.velZ * eTime;

		if (BossUnit.posZ <= 0.0f) {
			BossUnit.State = STATE_GROUND;
			BossUnit.velZ = 0.0f;
			BossUnit.posZ = 0.0f;
		}
		else {
			BossUnit.State = STATE_AIR;
		}
	}

	m_AnimationNextCurTime -= eTime;
	if (m_AnimationNextCurTime <= 0.f)
	{
		m_SeqX = g_SeqB % 2;
		m_SeqY = (int)g_SeqB / 2;
		g_SeqB++;
		if (g_SeqB > 4) {
			g_SeqB = 0;
		}
		m_AnimationNextTime = 0.075f;
		m_AnimationNextCurTime = 0.075f;
	}

	if (BossUnit.posX > 3.0f || BossUnit.posX < -3.0f) {
		std::random_device randDeivce;
		std::default_random_engine randEngine{ randDeivce() };
		std::uniform_real_distribution<float> distX{ 0.01f, 0.05f };
		std::uniform_real_distribution<float> distY{ 0.01f, 0.05f };
		SetDelta(distX(randEngine), distY(randEngine), 0.0f);
		m_DirX *= -1; 
	}
	if (BossUnit.posY > 1.0f || BossUnit.posY < -2.0f) {
		std::random_device randDeivce;
		std::default_random_engine randEngine{ randDeivce() };
		std::uniform_real_distribution<float> distX{ 0.01f, 0.05f };
		std::uniform_real_distribution<float> distY{ 0.01f, 0.05f };
		SetDelta(distX(randEngine), distY(randEngine), 0.0f);
		m_DirY *= -1; 
	}

	BossUnit.posX += m_DeltaX * m_DirX;
	BossUnit.posY += m_DeltaY * m_DirY;
	BossUnit.posZ += m_DeltaZ * m_DirZ;
}

void Boss::GetSeqX(int * x)
{
	*x = m_SeqX;
}

void Boss::GetSeqY(int * y)
{
	*y = m_SeqY;
}

void Boss::GetDir(int * x, int * y, int * z)
{
	*x = m_DirX;
	*y = m_DirY;
	*z = m_DirZ;
}

void Boss::SetDir(int x, int y, int z)
{
	m_DirX = x;
	m_DirY = y;
	m_DirZ = z;
}

void Boss::GetDelta(float * x, float * y, float * z)
{
	*x = m_DeltaX;
	*y = m_DeltaY;
	*z = m_DeltaZ;
}

void Boss::SetDelta(float x, float y, float z)
{
	m_DeltaX = x;
	m_DeltaY = y;
	m_DeltaZ = z;
}
