#pragma once
class Object
{
protected:
	float m_PosX, m_PosY, m_PosZ;
	float m_SizeX, m_SizeY, m_SizeZ;
	float m_ColorR, m_ColorG, m_ColorB, m_ColorA;
	float m_VelX, m_VelY, m_VelZ;
	float m_AccX, m_AccY, m_AccZ;
	float m_Mass;
	float m_CoefFrict;

	int m_Kind;
	int m_HP;
	int m_State;

public:
	Object();
	virtual ~Object();

	void GetPos(float *x, float *y, float* z);
	void SetPos(float x, float y, float z);

	void GetSize(float *sx, float *sy, float *sz);
	void SetSize(float sx, float sy, float sz);

	void GetColor(float *r, float *g, float *b, float *a);
	void SetColor(float r, float g, float b, float a);

	void GetVel(float *x, float *y, float *z);
	void SetVel(float x, float y, float z);

	void GetAcc(float *x, float *y, float *z);
	void SetAcc(float x, float y, float z);

	void GetMass(float *x);
	void SetMass(float x);

	void GetCoefFrict(float *x);
	void SetCoefFrict(float x);

	void SetKind(int Kind);
	void GetKind(int *Kind);

	void SetHP(int HP);
	void GetHP(int *HP);

	void SetState(int State);
	void GetState(int *State);

	void Update(float eTime);

	void ApplyForce(float x, float y, float z, float eTime);

};