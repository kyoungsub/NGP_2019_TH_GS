#include "stdafx.h"
#include "Effect.h"

int g_SeqE = 0;

Effect::Effect()
{
}


Effect::~Effect()
{
}

void Effect::Update(float eTime)
{
	// Animation
	m_AnimationNextCurTime -= eTime;
	if (m_AnimationNextCurTime <= 0.f)
	{
		m_SeqX = g_SeqE % 4;
		m_SeqY = (int)g_SeqE / 4;
		g_SeqE++;
		if (g_SeqE > 15) {
			g_SeqE = 0;
			SetHP(0);
		}
		m_AnimationNextTime = 0.02f;
		m_AnimationNextCurTime = 0.02f;
	}
}

void Effect::GetSeqX(int *x)
{
	*x = m_SeqX;
}

void Effect::GetSeqY(int *y)
{
	*y = m_SeqY;
}
