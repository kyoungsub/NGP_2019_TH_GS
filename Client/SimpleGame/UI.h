#pragma once
#include "Object.h"
class UI :
	public Object
{
public:
	UI();
	virtual ~UI();

	void GetSeqX(int *x);
	void SetSeqX(int x);

private:
	int m_SeqX = 0;

};

