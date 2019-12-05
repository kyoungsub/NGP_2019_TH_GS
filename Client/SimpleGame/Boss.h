#pragma once
#include "Object.h"
class Boss :
	public Object
{
public:
	Boss();
	virtual ~Boss();

	virtual void Update(float eTime);

	void GetSeqX(int *x);
	void GetSeqY(int *y);

	void SetSeq(int x, int y);

	void GetDir(int *x, int *y, int *z);
	void SetDir(int x, int y, int z);

	void GetDelta(float *x, float *y, float *z);
	void SetDelta(float x, float y, float z);

private:
	float m_AnimationNextTime = 0.075f;
	float m_AnimationNextCurTime = 0.075f;

	int m_SeqX = 0;
	int m_SeqY = 0;

	int m_DirX = 0, m_DirY = 0, m_DirZ = 0;
	float m_DeltaX = 0, m_DeltaY = 0, m_DeltaZ = 0;
};

