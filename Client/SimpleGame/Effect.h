#pragma once
#include "Object.h"

class Effect : public Object
{
public:
	Effect();
	virtual ~Effect();

	virtual void Update(float eTime);

	void GetSeqX(int *x);
	void GetSeqY(int *y);

private:
	float m_AnimationNextTime = 0.02f;
	float m_AnimationNextCurTime = 0.02f;

	int m_SeqX = 0;
	int m_SeqY = 0;
};

