#include "stdafx.h"
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


Boss::~Boss()
{
}

void Boss::Update(float eTime)
{
	Object::Update(eTime);

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

	if (m_PosX > 3.0f || m_PosX < -3.0f) {
		std::random_device randDeivce;
		std::default_random_engine randEngine{ randDeivce() };
		std::uniform_real_distribution<float> distX{ 0.01f, 0.05f };
		std::uniform_real_distribution<float> distY{ 0.01f, 0.05f };
		SetDelta(distX(randEngine), distY(randEngine), 0.0f);
		m_DirX *= -1; 
	}
	if (m_PosY > 1.0f || m_PosY < -2.0f) {
		std::random_device randDeivce;
		std::default_random_engine randEngine{ randDeivce() };
		std::uniform_real_distribution<float> distX{ 0.01f, 0.05f };
		std::uniform_real_distribution<float> distY{ 0.01f, 0.05f };
		SetDelta(distX(randEngine), distY(randEngine), 0.0f);
		m_DirY *= -1; 
	}

	m_PosX += m_DeltaX * m_DirX;
	m_PosY += m_DeltaY * m_DirY;
	m_PosZ += m_DeltaZ * m_DirZ;
}

void Boss::GetSeqX(int * x)
{
	*x = m_SeqX;
}

void Boss::GetSeqY(int * y)
{
	*y = m_SeqY;
}

void Boss::SetSeq(int x, int y) {
	m_SeqX = x;
	m_SeqY = y;
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
