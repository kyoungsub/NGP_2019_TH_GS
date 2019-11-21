#pragma once
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <random>

#define HERO_ID 0
#define HERO_ID2 1

#define MAX_OBJECTS 300

#define SHOOT_NONE -1
#define SHOOT_LEFT 1
#define SHOOT_RIGHT 2
#define SHOOT_UP 3
#define SHOOT_DOWN 4

#define KIND_HERO 0
#define KIND_BULLET 1
#define KIND_EFFECT 2
#define KIND_MONSTER 3
#define KIND_BOSS 4
#define KIND_POOP 5
#define KIND_BOSS_DOOR 6
#define KIND_HP 7
#define KIND_WIN 8
#define kIND_DEATH 9

#define STATE_GROUND 0
#define STATE_AIR 1

#define GRAVITY 9.8

#define	DEFAULTROOM 101
#define BOSSROOM 102

//전송 구조체
#pragma pack(1)
struct RecvSendData {
	float posX, posY, posZ;
	float VelX, VelY, VelZ;
	int type, idx_num;
};
#pragma pack()

#pragma pack(1)
struct InitData {
	float mass;
	float sizeX, sizeY, sizeZ;
	float coef_Frict;
};
#pragma pack()

struct Object
{
	int type;			//객체의 종류
	int idx_num;		//객체의 인덱스(배열 위치)
	float posX, posY, posZ;	//위치
	float sizeX, sizeY, sizeZ;	//크기
	float accX, accY, accZ;	//가속도
	float velX, velY, velZ;	//속도
	float mass;			//질량
	float coefFriction;	//마찰계수
	float elapsed_time;	//경과시간
	int HP;				//체력
	int State;			//생사여부
};