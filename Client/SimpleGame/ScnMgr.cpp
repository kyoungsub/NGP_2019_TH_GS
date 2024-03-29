#include "stdafx.h"
#include "ScnMgr.h"

int g_Seq = 0;
float g_time = 0;

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

	// Load Texture
	m_BGTexture = m_Renderer->CreatePngTexture("./textures/Basement01.png");
	m_CharacterTexture = m_Renderer->CreatePngTexture("./textures/isaac.png");
	m_BulletTexture = m_Renderer->CreatePngTexture("./textures/effect_015_tearpoofa.png");
	m_MonsterTexture = m_Renderer->CreatePngTexture("./textures/monster_001_pooter.png");
	m_BossTexture = m_Renderer->CreatePngTexture("./textures/boss_007_dukeofflies.png");
	m_HPTexture = m_Renderer->CreatePngTexture("./textures/ui_hearts.png");
	m_WinTexture = m_Renderer->CreatePngTexture("./textures/loadimages-002.png");
	m_DeathTexture = m_Renderer->CreatePngTexture("./textures/deathposter.png");
	m_BossDoorTexture = m_Renderer->CreatePngTexture("./textures/BossDoor.png");

	// Creat HP UI

	m_HP[HERO_ID][0] = new UI();
	m_HP[HERO_ID][0]->SetPos(-2.4f, 2.5f, 0.0f);
	m_HP[HERO_ID][0]->SetSize(0.3f, 0.3f, 0.3f);
	m_HP[HERO_ID][0]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_HP[HERO_ID][0]->SetKind(KIND_HP);
	m_HP[HERO_ID][0]->SetHP(80);
	m_HP[HERO_ID][0]->SetState(STATE_GROUND);

	m_HP[HERO_ID][1] = new UI();
	m_HP[HERO_ID][1]->SetPos(-2.1f, 2.5f, 0.0f);
	m_HP[HERO_ID][1]->SetSize(0.3f, 0.3f, 0.3f);
	m_HP[HERO_ID][1]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_HP[HERO_ID][1]->SetKind(KIND_HP);
	m_HP[HERO_ID][1]->SetHP(80);
	m_HP[HERO_ID][1]->SetState(STATE_GROUND);

	m_HP[HERO_ID][2] = new UI();
	m_HP[HERO_ID][2]->SetPos(-1.8f, 2.5f, 0.0f);
	m_HP[HERO_ID][2]->SetSize(0.3f, 0.3f, 0.3f);
	m_HP[HERO_ID][2]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_HP[HERO_ID][2]->SetKind(KIND_HP);
	m_HP[HERO_ID][2]->SetHP(80);
	m_HP[HERO_ID][2]->SetState(STATE_GROUND);

	m_HP[HERO_ID2][0] = new UI();
	m_HP[HERO_ID2][0]->SetPos(2.4f, 2.5f, 0.0f);
	m_HP[HERO_ID2][0]->SetSize(0.3f, 0.3f, 0.3f);
	m_HP[HERO_ID2][0]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_HP[HERO_ID2][0]->SetKind(KIND_HP);
	m_HP[HERO_ID2][0]->SetHP(80);
	m_HP[HERO_ID2][0]->SetState(STATE_GROUND);

	m_HP[HERO_ID2][1] = new UI();
	m_HP[HERO_ID2][1]->SetPos(2.7f, 2.5f, 0.0f);
	m_HP[HERO_ID2][1]->SetSize(0.3f, 0.3f, 0.3f);
	m_HP[HERO_ID2][1]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_HP[HERO_ID2][1]->SetKind(KIND_HP);
	m_HP[HERO_ID2][1]->SetHP(80);
	m_HP[HERO_ID2][1]->SetState(STATE_GROUND);

	m_HP[HERO_ID2][2] = new UI();
	m_HP[HERO_ID2][2]->SetPos(3.0f, 2.5f, 0.0f);
	m_HP[HERO_ID2][2]->SetSize(0.3f, 0.3f, 0.3f);
	m_HP[HERO_ID2][2]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_HP[HERO_ID2][2]->SetKind(KIND_HP);
	m_HP[HERO_ID2][2]->SetHP(80);
	m_HP[HERO_ID2][2]->SetState(STATE_GROUND);
	
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

	if (win) {
		m_Renderer->DrawTextureRectDepth(0, 0, 0, WINDOWS_WIDTH, WINDOWS_HEIGHT, 1, 1, 1, 1, m_WinTexture, 0.f);
	}
	if (lose) {
		m_Renderer->DrawTextureRectDepth(0, 0, 0, WINDOWS_WIDTH, WINDOWS_HEIGHT, 1, 1, 1, 1, m_DeathTexture, 0.f);
	}

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

			int hp;
			int kind;

			m_Objects[i]->GetHP(&hp);
			m_Objects[i]->GetKind(&kind);

			if (kind == KIND_HERO) {
				if (hp != 0) {
					m_Renderer->DrawTextureRectHeight(
						newX, newY, 0.0f,
						newsX, newsY,
						r, g, b, a,
						m_CharacterTexture, newZ
					);
				}
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
				int SeqX = 0, SeqY = 0;

				if (m_Objects[i] != NULL) {
					dynamic_cast<Monster*>(m_Objects[i])->GetSeqX(&SeqX);
					dynamic_cast<Monster*>(m_Objects[i])->GetSeqY(&SeqY);
				}
				m_Renderer->DrawTextureRectSeqXY(
					newX, newY, 0.0f,
					newsX, newsY,
					r, g, b, a,
					m_MonsterTexture, SeqX, SeqY,
					4, 4
				);
			}
			else if (kind == KIND_BOSS) {
				int SeqX = 0, SeqY = 0;

				if (m_Objects[i] != NULL) {
					dynamic_cast<Boss*>(m_Objects[i])->GetSeqX(&SeqX);
					dynamic_cast<Boss*>(m_Objects[i])->GetSeqY(&SeqY);
				}
				m_Renderer->DrawTextureRectSeqXY(
					newX, newY, 0.0f,
					newsX, newsY,
					r, g, b, a,
					m_BossTexture, SeqX, SeqY,
					2, 2
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

			float gauge;

			if (m_Objects[i] != NULL) {
				
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
			else {
				if (kind == KIND_BOSS) {
					m_Renderer->DrawSolidRectGauge(
						0.0f, newsY + 75.0f, 0,
						newsX * 3, 10,
						1, 0, 0, 1,
						newZ,
						0);
				}
			}
		}
	}
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (m_HP[i][j] != NULL) {
				float x, y, z, sizeX, sizeY, sizeZ, r, g, b, a;

				m_HP[i][j]->GetPos(&x, &y, &z);
				m_HP[i][j]->GetSize(&sizeX, &sizeY, &sizeZ);
				m_HP[i][j]->GetColor(&r, &g, &b, &a);

				float newX, newY, newZ, newsX, newsY, newsZ;

				newX = x * 100;
				newY = y * 100;
				newZ = z * 100;

				newsX = sizeX * 100;
				newsY = sizeY * 100;
				newsZ = sizeZ * 100;

				int SeqX;
				dynamic_cast<UI*>(m_HP[i][j])->GetSeqX(&SeqX);

				m_Renderer->DrawTextureRectSeqXY(
					newX, newY, 0.0f,
					newsX, newsY,
					r, g, b, a,
					m_HPTexture, SeqX, 0,
					5, 2
				);
			}
		}
	}
	g_time += 0.01f;
}

void ScnMgr::AddObject(float x, float y, float z, float sx, float sy, float sz, float vx, float vy, float vz, int kind, int hp, int idx_num)
{
	if (kind == KIND_MONSTER) {
		m_Objects[idx_num] = new Monster();
	}
	else if (kind == KIND_BOSS) {
		m_Objects[idx_num] = new Boss();
	}
	else if (kind == KIND_EFFECT) {
		m_Objects[idx_num] = new Effect();
	}
	else {
		m_Objects[idx_num] = new Object();
	}

	m_Objects[idx_num]->SetPos(x, y, z);
	m_Objects[idx_num]->SetVel(vx, vy, vz);
	m_Objects[idx_num]->SetAcc(0.0f, 0.0f, 0.0f);
	m_Objects[idx_num]->SetSize(sx, sy, sz);
	m_Objects[idx_num]->SetMass(0.2f);
	m_Objects[idx_num]->SetCoefFrict(5.0f);
	m_Objects[idx_num]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Objects[idx_num]->SetKind(kind);
	m_Objects[idx_num]->SetHP(hp);

}

void ScnMgr::DeleteObject(int id)
{
	if (m_Objects[id] != NULL) {
		delete m_Objects[id];
		m_Objects[id] = NULL;
	}
}

void ScnMgr::DeleteUI(int player, int id)
{
	if (m_HP[player][id] != NULL) {
		delete m_HP[player][id];
		m_HP[player][id] = NULL;
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
