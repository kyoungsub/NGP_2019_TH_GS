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
	Sound *m_Sound;

	GLuint m_BGTexture = 0;
	GLuint m_CharacterTexture = 0;
	GLuint m_BulletTexture = 0;
	GLuint m_MonsterTexture = 0;
	GLuint m_BossTexture = 0;
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
	Renderer * m_Renderer;
	BOOL player1;

	ScnMgr();
	~ScnMgr();
	
	void RenderScene();
	
	void AddObject(float x, float y, float z, float sx, float sy, float sz, float vx, float vy, float vz, int kind, int hp, int idx_num);
	void DeleteObject(int id);
	void DeleteUI(int id);

	void ChangeMap(int kind);
};

