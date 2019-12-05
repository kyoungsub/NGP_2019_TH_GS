#pragma once

//#include "Renderer.h"
#include "Object.h"
#include "Monster.h"
#include "Boss.h"
#include "Effect.h"
#include "UI.h"
#include "Global.h"
//#include "Sound.h"

class ScnMgr
{
private:
	Object *m_HP[20];
	
public:
	EventSet eventflag;
	Object *m_Objects[MAX_OBJECTS];

	ScnMgr();
	~ScnMgr();

	void Update(float eTime);
	void GarbageCollector();
	void ApplyForce(float x, float y, float z, int player, float eTime);

	void AddObject(float x, float y, float z, float sx, float sy, float sz, float vx, float vy, float vz, int kind, int hp, int state);
	void DeleteObject(int id);
	void DeleteUI(int id);

	int FindEmptyObjectSlot();

	void Shoot(int player, int shootID);

	void DoCollisionTest();
	bool RRCollision(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, float minX1, float minY1, float minZ1, float maxX1, float maxY1, float maxZ1);
	void ProcessCollison(int i, int j);

	void ChangeMap(int kind);
};

