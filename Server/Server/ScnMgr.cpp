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

BOOL PLAY = TRUE;

ScnMgr::ScnMgr()
{
	// Init Objects List
	for (int i = 0; i < MAX_OBJECTS; ++i) {
		m_Objects[i] = NULL;
	}
}

ScnMgr::~ScnMgr()
{
	if (m_Objects[HERO_ID]) {
		delete[] m_Objects;
		for (int i = 0; i < HERO_ID; ++i) {
			m_Objects[i] = NULL;
		}
	}
}

void ScnMgr::Update(float eTime)
{
	int kind;
	player1DoorCollision = FALSE;
	player2DoorCollision = FALSE;
	

	if (PLAY) {
		for (int i = 0; i < MAX_OBJECTS; ++i) {
			if (m_Objects[i] != NULL) {

				m_Objects[i]->GetKind(&kind);

				if (kind == KIND_MONSTER) {
					float x, y, z;
					m_Objects[dynamic_cast<Monster*>(m_Objects[i])->target]->GetPos(&x, &y, &z);
					x += 0.1f, y += 0.1f;
					dynamic_cast<Monster*>(m_Objects[i])->Update(x, y, z, eTime);
				}
				else if (kind == KIND_BOSS) {
					dynamic_cast<Boss*>(m_Objects[2])->Update(eTime);
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
							//m_Sound->PlaySound(m_SoundCough, false, 0.3f);
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
					//DeleteObject(i);
					//AddObject(0.0f, -0.5f, 0.0f, 4.0f, 4.0f, 4.0f, 0.0f, 0.0f, 0.0f, kIND_DEATH, 20, STATE_GROUND);
					if (i == HERO_ID) {
						eventflag.diedPlayer1 = TRUE;
					}
					else if (i == HERO_ID2) {
						eventflag.diedPlayer2 = TRUE;
					}
					continue;
				}
			}
			if (kind == KIND_BOSS) {
				if (hp <= 0) {
					//DeleteObject(i);
					//AddObject(0.0f, -0.5f, 0.0f, 4.0f, 4.0f, 4.0f, 0.0f, 0.0f, 0.0f, KIND_WIN, 20, STATE_GROUND);
					PLAY = FALSE;
					eventflag.BossDead = TRUE;
					continue;
				}
			}
			if (kind == KIND_MONSTER) {
				if (hp <= 0) {
					DeleteObject(i);
					continue;
				}
			}
			if (kind == KIND_BULLET) {
				if (velx == 0.0f && vely == 0.0f) { // FLT_EPSILON을 사용하면 제자리에서 왼쪽과 아래쪽으로 미사일 발사가 안되는 현상이 있어서 0.0f값과 직접 비교

					float px, py, pz;
					float sx, sy, sz;

					m_Objects[i]->GetPos(&px, &py, &pz);
					m_Objects[i]->GetSize(&sx, &sy, &sz);

					//AddObject(px, py, pz, sx, sy, sz, 0.0f, 0.0f, 0.0f, KIND_EFFECT, 20, STATE_AIR);

					DeleteObject(i);
					continue;
				}
				if (x > 3.0f || x < -3.0f || y > 1.0f || y < -2.0f) {
					float px, py, pz;
					float sx, sy, sz;

					m_Objects[i]->GetPos(&px, &py, &pz);
					m_Objects[i]->GetSize(&sx, &sy, &sz);

					//AddObject(px, py, pz, sx, sy, sz, 0.0f, 0.0f, 0.0f, KIND_EFFECT, 20, STATE_AIR);

					DeleteObject(i);
					//m_Sound->PlaySound(m_SoundTearsImpacts, false, 0.5f);
					continue;
				}
				if (hp <= 0) {
					float px, py, pz;
					float sx, sy, sz;

					m_Objects[i]->GetPos(&px, &py, &pz);
					m_Objects[i]->GetSize(&sx, &sy, &sz);

					//AddObject(px, py, pz, sx, sy, sz, 0.0f, 0.0f, 0.0f, KIND_EFFECT, 20, STATE_AIR);

					DeleteObject(i);
					//m_Sound->PlaySound(m_SoundTearsImpacts, false, 0.5f);
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
}


void ScnMgr::ApplyForce(float x, float y, float z, int player, float eTime)
{
	if (PLAY) {
		int state;
		m_Objects[player]->GetState(&state);
		if (state == STATE_AIR) {
			z = 0.0f;
		}

		float cx, cy, cz;
		m_Objects[player]->GetPos(&cx, &cy, &cz);

		if (cx > 3.0f) { x = -1; }
		if (cx < -3.0f) { x = 1; }
		if (cy > 1.0f) { y = -1; }
		if (cy < -2.0f) { y = 1; }

		if (m_Objects[player] != NULL) {
			m_Objects[player]->ApplyForce(x, y, z, eTime);
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

void ScnMgr::GetPlayerCollision(int player, bool* dest)
{
	if (player == 1)
		*dest = player1DoorCollision;
	else if (player == 2)
		*dest = player2DoorCollision;
}

void ScnMgr::SetPlayerCollision(int player)
{
	if (player == 1)
		player1DoorCollision = TRUE;
	if (player == 2)
		player2DoorCollision = TRUE;
}

void ScnMgr::Shoot(int player, int shootID)
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
	m_Objects[player]->GetPos(&pX, &pY, &pZ);

	float vX, vY, vZ;
	m_Objects[player]->GetVel(&vX, &vY, &vZ);

	bvX = bvX + vX;
	bvY = bvY + vY;
	bvZ = bvZ + vZ;

	AddObject(pX, pY, pZ, 0.75f, 0.75f, 0.75f, bvX, bvY, bvZ, KIND_BULLET, 20, STATE_AIR);
	//m_Sound->PlaySound(m_SoundTearsFire, false, 0.5f);
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
				else if (kind == KIND_HERO && kind1 == KIND_BOSS_DOOR || kind == KIND_BOSS_DOOR && kind1 == KIND_HERO ) {
					if (RRCollision(minX, minY, minZ, maxX, maxY, maxZ, minX1, minY1, minZ1, maxX1, maxY1, maxZ1)) {

						if (i == HERO_ID || j == HERO_ID)
							player1DoorCollision = TRUE;
						else if (i == HERO_ID2 || j == HERO_ID2)
							player2DoorCollision = TRUE;

						if (player1DoorCollision == TRUE && player2DoorCollision == TRUE) {
							eventflag.changeMap = TRUE;

							// Delete Door Object
							if (kind == KIND_BOSS_DOOR) {
								DeleteObject(2);
							}
							else if (kind1 == KIND_BOSS_DOOR) {
								DeleteObject(2);
							}

							// Creat Boss Object
							m_Objects[2] = new Boss();
							m_Objects[2]->SetPos(1.5f, 0.0f, 0.0f);
							m_Objects[2]->SetVel(0.f, 0.f, 0.f);
							m_Objects[2]->SetAcc(0.0f, 0.0f, 0.0f);
							m_Objects[2]->SetSize(1.0f, 1.0f, 1.0f);
							m_Objects[2]->SetMass(0.15f);
							m_Objects[2]->SetCoefFrict(0.5f);
							m_Objects[2]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
							m_Objects[2]->SetKind(KIND_BOSS);
							m_Objects[2]->SetHP(200);
							m_Objects[2]->SetState(STATE_GROUND);

						}
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
		/*
		if (m_HP[index] != NULL) {
			index = hp / 40 / 2;
			SeqX = hp / 40 % 2;

			m_HP[index]->GetHP(&uhp);
			uhp -= 40;
			m_HP[index]->SetHP(uhp);

			dynamic_cast<UI*>(m_HP[index])->SetSeqX(SeqX);
		}
		*/
		//m_Sound->PlaySound(m_SoundHurt, false, 0.1f);
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
		/*
		if (m_HP[index] != NULL) {
			index = hp / 40 / 2;
			SeqX = hp / 40 % 2;

			m_HP[index]->GetHP(&uhp);
			uhp -= 40;
			m_HP[index]->SetHP(uhp);

			dynamic_cast<UI*>(m_HP[index])->SetSeqX(SeqX);
		}
		*/
		//m_Sound->PlaySound(m_SoundHurt, false, 0.1f);
	}
}
