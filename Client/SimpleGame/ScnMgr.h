#pragma once

#include "Renderer.h"
#include "Object.h"
#include "Monster.h"
#include "Boss.h"
#include "Effect.h"
#include "UI.h"
#include "Global.h"
#include "Sound.h"

class ScnMgr
{
private:
	Object *m_HP[20];
	Renderer * m_Renderer;
	Sound *m_Sound;

	GLuint m_BGTexture = 0;
	GLuint m_CharacterTexture = 0;
	GLuint m_BulletTexture = 0;
	GLuint m_MonsterTexture = 0;
	GLuint m_BossTexture = 0;
	GLuint m_PoopTexture = 0;
	GLuint m_HPTexture = 0;
	GLuint m_WinTexture = 0;
 	GLuint m_DeathTexture = 0;
	GLuint m_BossDoorTexture = 0;

	int m_SoundBG = 0;
	int m_SoundTearsFire = 0;
	int m_SoundTearsImpacts = 0;
	int m_SoundHurt = 0;
	int m_SoundExplosion = 0;
	int m_SoundPlop = 0;
	int m_SoundCough = 0;
	int m_SoundWin = 0;
	int m_SoundDeath = 0;

public:

	Object *m_Objects[MAX_OBJECTS];
	BOOL player1;

	ScnMgr();
	~ScnMgr();
	
	void RenderScene();
	void Update(float eTime);
	void GarbageCollector();
	void ApplyForce(float x, float y, float z, float eTime);

	void AddObject(float x, float y, float z, float sx, float sy, float sz, float vx, float vy, float vz, int kind, int hp, int state);
	void DeleteObject(int id);
	void DeleteUI(int id);
	void AddPlayer(float x, float y, float z, float vx, float vy, float vz);

	int FindEmptyObjectSlot();

	void Shoot(int shootID);

	void DoCollisionTest();
	bool RRCollision(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, float minX1, float minY1, float minZ1, float maxX1, float maxY1, float maxZ1);
	void ProcessCollison(int i, int j);	

	void ChangeMap(int kind);
};

