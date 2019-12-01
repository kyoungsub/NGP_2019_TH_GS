#include "stdafx.h"
#include "ScnMgr.h"

int g_Seq = 0;
float g_time = 0;

// Hit Delay
DWORD g_HitStartTime = 0;
DWORD HitCurTime = 0;
DWORD HitElapsedTime = 0;

// Boss - Spawn monster time
int Spawncnt = 0;
float m_SpawnNextTime = 0.1f;
float m_SpawnNextCurTime = 0.1f;

// Poop seq
int Poop_seq = 0;

BOOL PLAY = TRUE;

ScnMgr::ScnMgr()
{
	m_Renderer = NULL;

	// Init Objects List
	for (int i = 0; i < MAX_OBJECTS; ++i) {
		m_Objects[i] = NULL;
	}

	// Init Renderer
	m_Renderer = new Renderer(WINDOWS_WIDTH, WINDOWS_HEIGHT);

	// Init Sound
	m_Sound = new Sound();

	m_SoundBG = m_Sound->CreateSound("./Sounds/The Binding of Isaac - Sacrificial.mp3");
	m_SoundTearsFire = m_Sound->CreateSound("./Sounds/Tears_Fire_0.mp3");
	m_SoundTearsImpacts = m_Sound->CreateSound("./Sounds/TearImpacts0.mp3");
	m_SoundHurt = m_Sound->CreateSound("./Sounds/Isaac_Hurt_Grunt0.mp3");
	m_SoundExplosion = m_Sound->CreateSound("./Sounds/Death_Burst_Large_0.mp3");
	m_SoundPlop = m_Sound->CreateSound("./Sounds/plop.mp3");
	m_SoundCough = m_Sound->CreateSound("./Sounds/Wheezy_Cough_1.mp3");
	m_SoundWin = m_Sound->CreateSound("./Sounds/Holy.mp3");
	m_SoundDeath = m_Sound->CreateSound("./Sounds/isaacdies.mp3");

	m_Sound->PlaySound(m_SoundBG, true, 0.3f);

	// Load Texture
	m_BGTexture = m_Renderer->CreatePngTexture("./textures/Basement01.png");
	m_CharacterTexture = m_Renderer->CreatePngTexture("./textures/isaac.png");
	m_BulletTexture = m_Renderer->CreatePngTexture("./textures/effect_015_tearpoofa.png");
	m_MonsterTexture = m_Renderer->CreatePngTexture("./textures/monster_001_pooter.png");
	m_BossTexture = m_Renderer->CreatePngTexture("./textures/boss_007_dukeofflies.png");
	m_PoopTexture = m_Renderer->CreatePngTexture("./textures/grid_poop_1.png");
	m_HPTexture = m_Renderer->CreatePngTexture("./textures/ui_hearts.png");
	m_WinTexture = m_Renderer->CreatePngTexture("./textures/loadimages-002.png");
	m_DeathTexture = m_Renderer->CreatePngTexture("./textures/deathposter.png");
	m_BossDoorTexture = m_Renderer->CreatePngTexture("./textures/BossDoor.png");

	/*
	// Creat Hero Object
	m_Objects[HERO_ID] = new Object();
	m_Objects[HERO_ID]->SetPos(0.0f, 0.0f, 0.0f);
	m_Objects[HERO_ID]->SetVel(0.0f, 0.0f, 0.0f);
	m_Objects[HERO_ID]->SetAcc(0.0f, 0.0f, 0.0f);
	m_Objects[HERO_ID]->SetSize(0.6f, 0.6f, 0.6f);
	m_Objects[HERO_ID]->SetMass(0.15f);
	m_Objects[HERO_ID]->SetCoefFrict(0.5f);
	m_Objects[HERO_ID]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Objects[HERO_ID]->SetKind(KIND_HERO);
	m_Objects[HERO_ID]->SetHP(240);
	m_Objects[HERO_ID]->SetState(STATE_GROUND);

	// Creat Poop Object
	AddObject(3.0f, -2.0f, 0.0f, 0.55f, 0.55f, 0.55f, 0.0f, 0.0f, 0.0f, KIND_POOP, 100, STATE_GROUND);

	// Creat BossDoor Object
	AddObject(0.0f, 1.35f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, KIND_BOSS_DOOR, 100, STATE_GROUND);

	*/

	// Creat HP UI
	m_HP[0] = new UI();
	m_HP[0]->SetPos(2.4f, 2.5f, 0.0f);
	m_HP[0]->SetSize(0.3f, 0.3f, 0.3f);
	m_HP[0]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_HP[0]->SetKind(KIND_HP);
	m_HP[0]->SetHP(80);
	m_HP[0]->SetState(STATE_GROUND);

	m_HP[1] = new UI();
	m_HP[1]->SetPos(2.7f, 2.5f, 0.0f);
	m_HP[1]->SetSize(0.3f, 0.3f, 0.3f);
	m_HP[1]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_HP[1]->SetKind(KIND_HP);
	m_HP[1]->SetHP(80);
	m_HP[1]->SetState(STATE_GROUND);

	m_HP[2] = new UI();
	m_HP[2]->SetPos(3.0f, 2.5f, 0.0f);
	m_HP[2]->SetSize(0.3f, 0.3f, 0.3f);
	m_HP[2]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_HP[2]->SetKind(KIND_HP);
	m_HP[2]->SetHP(80);
	m_HP[2]->SetState(STATE_GROUND);

}

ScnMgr::~ScnMgr()
{
	if (m_Renderer) {
		delete m_Renderer;
		m_Renderer = NULL;
	}

	if (m_Objects[HERO_ID]) {
		delete[] m_Objects;
		for (int i = 0; i < HERO_ID; ++i) {
			m_Objects[i] = NULL;
		}
	}
}

void ScnMgr::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// Render BG
	m_Renderer->DrawTextureRectDepth(0, 0, 0, WINDOWS_WIDTH, WINDOWS_HEIGHT, 1, 1, 1, 1, m_BGTexture, 1.f);

	// Render Object
	for (int i = 0; i < MAX_OBJECTS; ++i) {
		if (m_Objects[i] != NULL) {
			float x, y, z, sizeX, sizeY, sizeZ, r, g, b, a;

			m_Objects[i]->GetPos(&x, &y, &z);
			m_Objects[i]->GetSize(&sizeX, &sizeY, &sizeZ);
			m_Objects[i]->GetColor(&r, &g, &b, &a);

			float newX, newY, newZ, newsX, newsY, newsZ;

			newX = x * 100;
			newY = y * 100;
			newZ = z * 100;

			newsX = sizeX * 100;
			newsY = sizeY * 100;
			newsZ = sizeZ * 100;

			int kind;

			m_Objects[i]->GetKind(&kind);

			if (kind == KIND_HERO) {
				m_Renderer->DrawTextureRectHeight(
					newX, newY, 0.0f,
					newsX, newsY,
					r, g, b, a,
					m_CharacterTexture, newZ
				);
			}
			else if (kind == KIND_BULLET) {
				m_Renderer->DrawTextureRectSeqXY(
					newX, newY, 0.0f,
					newsX, newsY,
					r, g, b, a,
					m_BulletTexture, 0, 0,
					4, 4
				);
			}
			else if (kind == KIND_MONSTER) {
				int SeqX, SeqY;

				dynamic_cast<Monster*>(m_Objects[i])->GetSeqX(&SeqX);
				dynamic_cast<Monster*>(m_Objects[i])->GetSeqY(&SeqY);

				m_Renderer->DrawTextureRectSeqXY(
					newX, newY, 0.0f,
					newsX, newsY,
					r, g, b, a,
					m_MonsterTexture, SeqX, SeqY,
					4, 4
				);
			}
			else if (kind == KIND_BOSS) {
				int SeqX, SeqY;

				dynamic_cast<Boss*>(m_Objects[i])->GetSeqX(&SeqX);
				dynamic_cast<Boss*>(m_Objects[i])->GetSeqY(&SeqY);

				m_Renderer->DrawTextureRectSeqXY(
					newX, newY, 0.0f,
					newsX, newsY,
					r, g, b, a,
					m_BossTexture, SeqX, SeqY,
					2, 2
				);
			}
			else if (kind == KIND_POOP) {
				m_Renderer->DrawTextureRectSeqXY(
					newX, newY, 0.0f,
					newsX, newsY,
					r, g, b, a,
					m_PoopTexture, Poop_seq, 0,
					5, 1
				);
			}
			else if (kind == KIND_BOSS_DOOR) {
				m_Renderer->DrawTextureRectHeight(
					newX, newY, -1.5f,
					newsX, newsY,
					r, g, b, a,
					m_BossDoorTexture, newZ
				);
			}
			else if (kind == KIND_EFFECT) {
				int SeqX, SeqY;

				dynamic_cast<Effect*>(m_Objects[i])->GetSeqX(&SeqX);
				dynamic_cast<Effect*>(m_Objects[i])->GetSeqY(&SeqY);

				m_Renderer->DrawTextureRectSeqXY(
					newX, newY, 0.0f,
					newsX, newsY,
					r, g, b, a,
					m_BulletTexture, SeqX, SeqY,
					4, 4
				);
			}
			else if (kind == KIND_WIN) {
				m_Renderer->DrawTextureRectDepth(
					0.0f, 0.0f, 0.0f,
					WINDOWS_WIDTH * 0.75f, WINDOWS_HEIGHT * 0.75f,
					1, 1, 1, 1,
					m_WinTexture, 0.0f
				);
			}
			else if (kind == kIND_DEATH) {
				m_Renderer->DrawTextureRectDepth(
					0.0f, 0.0f, 0.0f,
					WINDOWS_WIDTH * 0.75f, WINDOWS_HEIGHT * 0.75f,
					1, 1, 1, 1,
					m_DeathTexture, 0.0f
				);
			}

			int hp;
			float gauge;

			m_Objects[i]->GetHP(&hp);
			gauge = hp / 200.0f;

			if (kind == KIND_BOSS) {
				m_Renderer->DrawSolidRectGauge(
					0.0f, newsY + 75.0f, 0,
					newsX * 3, 10,
					1, 0, 0, 1,
					newZ,
					gauge);
			}
		}
	}
	for (int i = 0; i < 3; ++i) {
		if (m_HP[i] != NULL) {
			float x, y, z, sizeX, sizeY, sizeZ, r, g, b, a;

			m_HP[i]->GetPos(&x, &y, &z);
			m_HP[i]->GetSize(&sizeX, &sizeY, &sizeZ);
			m_HP[i]->GetColor(&r, &g, &b, &a);

			float newX, newY, newZ, newsX, newsY, newsZ;

			newX = x * 100;
			newY = y * 100;
			newZ = z * 100;

			newsX = sizeX * 100;
			newsY = sizeY * 100;
			newsZ = sizeZ * 100;

			int SeqX;
			dynamic_cast<UI*>(m_HP[i])->GetSeqX(&SeqX);

			m_Renderer->DrawTextureRectSeqXY(
				newX, newY, 0.0f,
				newsX, newsY,
				r, g, b, a,
				m_HPTexture, SeqX, 0,
				5, 2
			);
		}
	}
	g_time += 0.01f;
}
void ScnMgr::Update(float eTime)
{
	int kind;

	if (PLAY) {
		for (int i = 0; i < MAX_OBJECTS; ++i) {
			if (m_Objects[i] != NULL) {

				m_Objects[i]->GetKind(&kind);

				if (kind == KIND_MONSTER) {
					float x, y, z;
					m_Objects[HERO_ID]->GetPos(&x, &y, &z);
					dynamic_cast<Monster*>(m_Objects[i])->Update(x, y, z, eTime);
				}
				else if (kind == KIND_BOSS) {
					dynamic_cast<Boss*>(m_Objects[i])->Update(eTime);
					m_SpawnNextCurTime -= eTime;

					if (m_SpawnNextCurTime <= 0.f)
					{
						float x, y, z;
						m_Objects[i]->GetPos(&x, &y, &z);
						Spawncnt++;
						if (Spawncnt > 20) {
							Spawncnt = 0;
							// Creat Test monster Object
							AddObject(x, y, z, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, KIND_MONSTER, 60, STATE_GROUND);
							m_Sound->PlaySound(m_SoundCough, false, 0.3f);
						}
						m_SpawnNextTime = 0.1f;
						m_SpawnNextCurTime = 0.1f;
					}
				}
				else if (kind == KIND_EFFECT) {
					dynamic_cast<Effect*>(m_Objects[i])->Update(eTime);
				}
				else {
					m_Objects[i]->Update(eTime);
				}
			}
		}
	}
}

void ScnMgr::GarbageCollector()
{
	for (int i = 0; i < MAX_OBJECTS; ++i) {
		if (m_Objects[i] != NULL) {
			float x, y, z;
			float velx, vely, velz;
			int kind, hp;

			m_Objects[i]->GetPos(&x, &y, &z);
			m_Objects[i]->GetKind(&kind);
			m_Objects[i]->GetHP(&hp);
			m_Objects[i]->GetVel(&velx, &vely, &velz);

			if (kind == KIND_HERO) {
				if (hp <= 0) {
					DeleteObject(i);
					AddObject(0.0f, -0.5f, 0.0f, 4.0f, 4.0f, 4.0f, 0.0f, 0.0f, 0.0f, kIND_DEATH, 20, STATE_GROUND);
					m_Sound->PlaySound(m_SoundDeath, false, 0.5f);
					PLAY = FALSE;
					continue;
				}
			}
			if (kind == KIND_BOSS) {
				if (hp <= 0) {
					DeleteObject(i);
					AddObject(0.0f, -0.5f, 0.0f, 4.0f, 4.0f, 4.0f, 0.0f, 0.0f, 0.0f, KIND_WIN, 20, STATE_GROUND);
					m_Sound->PlaySound(m_SoundExplosion, false, 0.5f);
					m_Sound->PlaySound(m_SoundWin, false, 0.5f);
					PLAY = FALSE;
					continue;
				}
			}
			if (kind == KIND_MONSTER) {
				if (hp <= 0) {
					DeleteObject(i);
					m_Sound->PlaySound(m_SoundExplosion, false, 0.5f);
					continue;
				}
			}
			if (kind == KIND_BULLET) {
				if (velx == 0.0f && vely == 0.0f) { // FLT_EPSILON을 사용하면 제자리에서 왼쪽과 아래쪽으로 미사일 발사가 안되는 현상이 있어서 0.0f값과 직접 비교

					float px, py, pz;
					float sx, sy, sz;

					m_Objects[i]->GetPos(&px, &py, &pz);
					m_Objects[i]->GetSize(&sx, &sy, &sz);

					AddObject(px, py, pz, sx, sy, sz, 0.0f, 0.0f, 0.0f, KIND_EFFECT, 20, STATE_AIR);

					DeleteObject(i);
					m_Sound->PlaySound(m_SoundTearsImpacts, false, 0.5f);
					continue;
				}
				if (x > 3.0f || x < -3.0f || y > 1.0f || y < -2.0f) {
					float px, py, pz;
					float sx, sy, sz;

					m_Objects[i]->GetPos(&px, &py, &pz);
					m_Objects[i]->GetSize(&sx, &sy, &sz);

					AddObject(px, py, pz, sx, sy, sz, 0.0f, 0.0f, 0.0f, KIND_EFFECT, 20, STATE_AIR);

					DeleteObject(i);
					m_Sound->PlaySound(m_SoundTearsImpacts, false, 0.5f);
					continue;
				}
				if (hp <= 0) {
					float px, py, pz;
					float sx, sy, sz;

					m_Objects[i]->GetPos(&px, &py, &pz);
					m_Objects[i]->GetSize(&sx, &sy, &sz);

					AddObject(px, py, pz, sx, sy, sz, 0.0f, 0.0f, 0.0f, KIND_EFFECT, 20, STATE_AIR);

					DeleteObject(i);
					m_Sound->PlaySound(m_SoundTearsImpacts, false, 0.5f);
					continue;
				}
			}
			if (kind == KIND_POOP) {
				if (Poop_seq > 4) {
					DeleteObject(i);
					m_Sound->PlaySound(m_SoundPlop, false, 1.1f);
					continue;
				}
			}
			if (kind == KIND_EFFECT) {
				if (hp <= 0) {
					DeleteObject(i);
					continue;
				}
			}
		}
	}
	for (int i = 0; i < 3; ++i) {
		if (m_HP[i] != NULL) {
			int kind, hp;
			m_HP[i]->GetKind(&kind);
			m_HP[i]->GetHP(&hp);
			if (kind == KIND_HP) {
				if (hp <= 0) {
					DeleteUI(i);
					continue;
				}
			}
		}
	}
}


void ScnMgr::ApplyForce(float x, float y, float z, float eTime)
{
	if (PLAY) {
		int state;
		m_Objects[HERO_ID]->GetState(&state);
		if (state == STATE_AIR) {
			z = 0.0f;
		}

		float cx, cy, cz;
		m_Objects[HERO_ID]->GetPos(&cx, &cy, &cz);

		if (cx > 3.0f) { x = -1; }
		if (cx < -3.0f) { x = 1; }
		if (cy > 1.0f) { y = -1; }
		if (cy < -2.0f) { y = 1; }

		if (m_Objects[HERO_ID] != NULL) {
			m_Objects[HERO_ID]->ApplyForce(x, y, z, eTime);
		}
	}
}

void ScnMgr::AddObject(float x, float y, float z, float sx, float sy, float sz, float vx, float vy, float vz, int kind, int hp, int state)
{
	int id = FindEmptyObjectSlot();

	if (id < 0) {
		return;
	}

	if (kind == KIND_MONSTER) {
		m_Objects[id] = new Monster();
	}
	else if (kind == KIND_BOSS) {
		m_Objects[id] = new Boss();
	}
	else if (kind == KIND_EFFECT) {
		m_Objects[id] = new Effect();
	}
	else {
		m_Objects[id] = new Object();
	}

	m_Objects[id]->SetPos(x, y, z);
	m_Objects[id]->SetVel(vx, vy, vz);
	m_Objects[id]->SetAcc(0.0f, 0.0f, 0.0f);
	m_Objects[id]->SetSize(sx, sy, sz);
	m_Objects[id]->SetMass(0.2f);
	m_Objects[id]->SetCoefFrict(5.0f);
	m_Objects[id]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Objects[id]->SetKind(kind);
	m_Objects[id]->SetHP(hp);

}

void ScnMgr::DeleteObject(int id)
{
	if (m_Objects[id] != NULL) {
		delete m_Objects[id];
		m_Objects[id] = NULL;
	}
}

void ScnMgr::DeleteUI(int id)
{
	if (m_HP[id] != NULL) {
		delete m_HP[id];
		m_HP[id] = NULL;
	}
}

void ScnMgr::AddPlayer(float x, float y, float z, float vx, float vy, float vz) {
	// Creat Hero Object

	if (x == -0.5f) {
		player1 = TRUE;
		m_Objects[HERO_ID] = new Object();
		m_Objects[HERO_ID]->SetPos(x, y, z);
		m_Objects[HERO_ID]->SetVel(0.f, 0.f, 0.f);
		m_Objects[HERO_ID]->SetAcc(0.0f, 0.0f, 0.0f);
		m_Objects[HERO_ID]->SetSize(0.6f, 0.6f, 0.6f);
		m_Objects[HERO_ID]->SetMass(0.15f);
		m_Objects[HERO_ID]->SetCoefFrict(0.5f);
		m_Objects[HERO_ID]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_Objects[HERO_ID]->SetKind(KIND_HERO);
		m_Objects[HERO_ID]->SetHP(240);
		m_Objects[HERO_ID]->SetState(STATE_GROUND);

		m_Objects[HERO_ID2] = new Object();
		m_Objects[HERO_ID2]->SetPos(-x, -y, -z);
		m_Objects[HERO_ID2]->SetVel(0.f, 0.f, 0.f);
		m_Objects[HERO_ID2]->SetAcc(0.0f, 0.0f, 0.0f);
		m_Objects[HERO_ID2]->SetSize(0.6f, 0.6f, 0.6f);
		m_Objects[HERO_ID2]->SetMass(0.15f);
		m_Objects[HERO_ID2]->SetCoefFrict(0.5f);
		m_Objects[HERO_ID2]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_Objects[HERO_ID2]->SetKind(KIND_HERO);
		m_Objects[HERO_ID2]->SetHP(240);
		m_Objects[HERO_ID2]->SetState(STATE_GROUND);
	}
	else if (x == 0.5f) {
		player1 = FALSE;
		m_Objects[HERO_ID] = new Object();
		m_Objects[HERO_ID]->SetPos(x, y, z);
		m_Objects[HERO_ID]->SetVel(0.f, 0.f, 0.f);
		m_Objects[HERO_ID]->SetAcc(0.0f, 0.0f, 0.0f);
		m_Objects[HERO_ID]->SetSize(0.6f, 0.6f, 0.6f);
		m_Objects[HERO_ID]->SetMass(0.15f);
		m_Objects[HERO_ID]->SetCoefFrict(0.5f);
		m_Objects[HERO_ID]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_Objects[HERO_ID]->SetKind(KIND_HERO);
		m_Objects[HERO_ID]->SetHP(240);
		m_Objects[HERO_ID]->SetState(STATE_GROUND);

		m_Objects[HERO_ID2] = new Object();
		m_Objects[HERO_ID2]->SetPos(-x, -y, -z);
		m_Objects[HERO_ID2]->SetVel(0.f, 0.f, 0.f);
		m_Objects[HERO_ID2]->SetAcc(0.0f, 0.0f, 0.0f);
		m_Objects[HERO_ID2]->SetSize(0.6f, 0.6f, 0.6f);
		m_Objects[HERO_ID2]->SetMass(0.15f);
		m_Objects[HERO_ID2]->SetCoefFrict(0.5f);
		m_Objects[HERO_ID2]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_Objects[HERO_ID2]->SetKind(KIND_HERO);
		m_Objects[HERO_ID2]->SetHP(240);
		m_Objects[HERO_ID2]->SetState(STATE_GROUND);
	}
}

int ScnMgr::FindEmptyObjectSlot()
{
	for (int i = 0; i < MAX_OBJECTS; ++i) {
		if (m_Objects[i] == NULL) {
			return i;
		}
	}
	std::cout << "No more empty slot!" << std::endl;
	return -1;
}

void ScnMgr::Shoot(int shootID)
{
	if (shootID == SHOOT_NONE) {
		return;
	}

	float amount = 20.0f;
	float bvX, bvY, bvZ;

	bvX = 0.0f;
	bvY = 0.0f;
	bvZ = 0.0f;

	switch (shootID)
	{
	case SHOOT_UP:
		bvX = 0.0f;
		bvY = amount;
		break;
	case SHOOT_DOWN:
		bvX = 0.0f;
		bvY = -amount;
		break;
	case SHOOT_LEFT:
		bvX = -amount;
		bvY = 0.0f;
		break;
	case SHOOT_RIGHT:
		bvX = amount;
		bvY = 0.0f;
		break;

	default:
		break;
	}

	float pX, pY, pZ;
	m_Objects[HERO_ID]->GetPos(&pX, &pY, &pZ);

	float vX, vY, vZ;
	m_Objects[HERO_ID]->GetVel(&vX, &vY, &vZ);

	bvX = bvX + vX;
	bvY = bvY + vY;
	bvZ = bvZ + vZ;

	AddObject(pX, pY, pZ, 0.75f, 0.75f, 0.75f, bvX, bvY, bvZ, KIND_BULLET, 20, STATE_AIR);
	m_Sound->PlaySound(m_SoundTearsFire, false, 0.5f);
}

void ScnMgr::DoCollisionTest()
{
	if (PLAY) {
		for (int i = 0; i < MAX_OBJECTS; ++i) {

			if (m_Objects[i] == NULL) {
				continue;
			}

			int collisonCount = 0;

			for (int j = i + 1; j < MAX_OBJECTS; ++j) {

				if (m_Objects[j] == NULL) {
					continue;
				}
				if (i == j) {
					continue;
				}

				// i object
				float pX, pY, pZ;
				float sX, sY, sZ;
				float minX, minY, minZ, maxX, maxY, maxZ;
				int kind;

				m_Objects[i]->GetPos(&pX, &pY, &pZ);
				m_Objects[i]->GetSize(&sX, &sY, &sZ);
				m_Objects[i]->GetKind(&kind);
				minX = pX - sX / 2.f;
				maxX = pX + sX / 2.f;
				minY = pY - sY / 2.f;
				maxY = pY + sY / 2.f;
				minZ = pZ - sZ / 2.f;
				maxZ = pZ + sZ / 2.f;

				// j object
				float pX1, pY1, pZ1;
				float sX1, sY1, sZ1;
				float minX1, minY1, minZ1, maxX1, maxY1, maxZ1;
				int kind1;

				m_Objects[j]->GetPos(&pX1, &pY1, &pZ1);
				m_Objects[j]->GetSize(&sX1, &sY1, &sZ1);
				m_Objects[j]->GetKind(&kind1);
				minX1 = pX1 - sX1 / 2.f;
				maxX1 = pX1 + sX1 / 2.f;
				minY1 = pY1 - sY1 / 2.f;
				maxY1 = pY1 + sY1 / 2.f;
				minZ1 = pZ1 - sZ1 / 2.f;
				maxZ1 = pZ1 + sZ1 / 2.f;

				if (kind == KIND_HERO && kind1 == KIND_BULLET || kind == KIND_BULLET && kind1 == KIND_HERO || kind == KIND_HERO && kind1 == KIND_EFFECT || kind == KIND_EFFECT && kind1 == KIND_HERO || kind == KIND_HERO && kind1 == KIND_POOP || kind == KIND_POOP && kind1 == KIND_HERO) {
					continue;
				}
				// BOSSDOOR와 충돌 했을 때
				else if (kind == KIND_HERO && kind1 == KIND_BOSS_DOOR || kind == KIND_BOSS_DOOR && kind1 == KIND_HERO || kind == KIND_BULLET && kind1 == KIND_BOSS_DOOR || kind == KIND_BOSS_DOOR && kind1 == KIND_BULLET || kind == KIND_EFFECT && kind1 == KIND_BOSS_DOOR || kind == KIND_BOSS_DOOR && kind1 == KIND_EFFECT) {
					if (RRCollision(minX, minY, minZ, maxX, maxY, maxZ, minX1, minY1, minZ1, maxX1, maxY1, maxZ1)) {
						// Delete Door Object
						if (kind == KIND_BOSS_DOOR) {
							DeleteObject(i);
						}
						else if (kind1 == KIND_BOSS_DOOR) {
							DeleteObject(j);
						}

						// Creat Boss Object
						AddObject(1.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, KIND_BOSS, 200, STATE_GROUND);

						// Change Map
						ChangeMap(BOSSROOM);
					}
					continue;
				}
				else {
					if (RRCollision(minX, minY, minZ, maxX, maxY, maxZ, minX1, minY1, minZ1, maxX1, maxY1, maxZ1)) {
						collisonCount++;

						// Calc Hit Delay
						HitCurTime = GetTickCount();
						HitElapsedTime = HitCurTime - g_HitStartTime;

						if (HitElapsedTime > 1000) { // Hit
							ProcessCollison(i, j);
						}
					}
				}
			}
			if (collisonCount > 0) {
				m_Objects[i]->SetColor(1.0f, 0.0f, 0.0f, 0.75f);
			}
			else {
				m_Objects[i]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
	}
}

bool ScnMgr::RRCollision(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, float minX1, float minY1, float minZ1, float maxX1, float maxY1, float maxZ1)
{
	if (maxX < minX1) return false;
	if (maxX1 < minX) return false;
	if (maxY < minY1) return false;
	if (maxY1 < minY) return false;
	if (maxZ < minZ1) return false;
	if (maxZ1 < minZ) return false;

	return true;
}

void ScnMgr::ProcessCollison(int i, int j)
{


	if (m_Objects[i] == NULL || m_Objects[j] == NULL)
		return;

	Object *obj1 = m_Objects[i];
	Object *obj2 = m_Objects[j];

	int kind1, kind2;

	obj1->GetKind(&kind1);
	obj2->GetKind(&kind2);

	if (kind1 == KIND_BOSS && kind2 == KIND_BULLET || kind1 == KIND_MONSTER && kind2 == KIND_BULLET) {
		int hp1, hp2;

		obj1->GetHP(&hp1);	// building
		obj2->GetHP(&hp2);	// bullet

		hp1 = hp1 - hp2;	// apply damage
		hp2 = 0;

		obj1->SetHP(hp1);
		obj2->SetHP(hp2);
	}
	if (kind1 == KIND_BULLET && kind2 == KIND_BOSS || kind1 == KIND_BULLET && kind2 == KIND_MONSTER) {
		int hp1, hp2;

		obj1->GetHP(&hp1);	// bullet
		obj2->GetHP(&hp2);	// building

		hp2 = hp2 - hp1;	// apply damage
		hp1 = 0;

		obj1->SetHP(hp1);
		obj2->SetHP(hp2);
	}
	if (kind1 == KIND_HERO && kind2 == KIND_BOSS || kind1 == KIND_HERO && kind2 == KIND_MONSTER) {
		g_HitStartTime = GetTickCount();

		int hp;
		obj1->GetHP(&hp);
		hp -= 40;
		obj1->SetHP(hp);

		int uhp = 0;

		int index = 0;
		int SeqX = 0;

		if (m_HP[index] != NULL) {
			index = hp / 40 / 2;
			SeqX = hp / 40 % 2;

			m_HP[index]->GetHP(&uhp);
			uhp -= 40;
			m_HP[index]->SetHP(uhp);

			dynamic_cast<UI*>(m_HP[index])->SetSeqX(SeqX);
		}
		m_Sound->PlaySound(m_SoundHurt, false, 0.1f);
	}
	if (kind1 == KIND_BOSS && kind2 == KIND_HERO || kind1 == KIND_MONSTER && kind2 == KIND_HERO) {
		g_HitStartTime = GetTickCount();

		int hp;
		obj2->GetHP(&hp);
		hp -= 40;
		obj2->SetHP(hp);

		int uhp = 0;

		int index = 0;
		int SeqX = 0;

		if (m_HP[index] != NULL) {
			index = hp / 40 / 2;
			SeqX = hp / 40 % 2;

			m_HP[index]->GetHP(&uhp);
			uhp -= 40;
			m_HP[index]->SetHP(uhp);

			dynamic_cast<UI*>(m_HP[index])->SetSeqX(SeqX);
		}
		m_Sound->PlaySound(m_SoundHurt, false, 0.1f);
	}
	if (kind1 == KIND_POOP && kind2 == KIND_BULLET) {
		int hp;

		Poop_seq++;
		hp = 0;
		obj2->SetHP(hp);
	}
	if (kind1 == KIND_BULLET && kind2 == KIND_POOP) {
		int hp;

		Poop_seq++;
		hp = 0;
		obj1->SetHP(hp);
	}
}

void ScnMgr::ChangeMap(int kind)
{
	if (kind == DEFAULTROOM) {
		m_BGTexture = m_Renderer->CreatePngTexture("./textures/Basement01.png");
	}
	else if (kind == BOSSROOM) {
		m_BGTexture = m_Renderer->CreatePngTexture("./textures/BossRoom.png");
	}
}

void ScnMgr::RecvDataToObject(recvData rData) {
	m_Objects[rData.idx_num]->SetPos(rData.posX, rData.posY, rData.posZ);
	m_Objects[rData.idx_num]->SetVel(rData.VelX, rData.VelY, rData.VelZ);
	m_Objects[rData.idx_num]->SetKind(rData.type);

	//디버깅용 출력코드
	printf("Pos : %f %f %f, Vel : %f %f %f, type: %d, idx_num : %d\n",
		rData.posX, rData.posY, rData.posZ,
		rData.VelX, rData.VelY, rData.VelZ,
		rData.type, rData.idx_num);
}