#include "stdafx.h"
#include "Monster.h"

int g_SeqM = 0;

Monster::Monster()
{
	target = rand() % 2;
}


Monster::~Monster()
{
}

void Monster::Update(float x, float y, float z, float eTime)
{
	Object::Update(eTime);

	if (m_PosX < x) {
		m_PosX += 0.01;
	}
	if (m_PosX > x) {
		m_PosX -= 0.01;
	}
	if (m_PosY < y) {
		m_PosY += 0.01;
	}
	if (m_PosY > y) {
		m_PosY -= 0.01;
	}

	// Animation
	m_AnimationNextCurTime -= eTime;
	if (m_AnimationNextCurTime <= 0.f)
	{
		m_SeqX = g_SeqM % 4;
		m_SeqY = (int)g_SeqM / 4;
		g_SeqM++;
		if (g_SeqM > 15) {
			g_SeqM = 0;
		}
		m_AnimationNextTime = 0.02f;
		m_AnimationNextCurTime = 0.02f;
	}
}

void Monster::GetSeqX(int * x)
{
	*x = m_SeqX;
}

void Monster::GetSeqY(int * y)
{
	*y = m_SeqY;
}
