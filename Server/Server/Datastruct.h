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

//���� ����ü
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
	int type;			//��ü�� ����
	int idx_num;		//��ü�� �ε���(�迭 ��ġ)
	float posX, posY, posZ;	//��ġ
	float sizeX, sizeY, sizeZ;	//ũ��
	float accX, accY, accZ;	//���ӵ�
	float velX, velY, velZ;	//�ӵ�
	float mass;			//����
	float coefFriction;	//�������
	float elapsed_time;	//����ð�
	int HP;				//ü��
	int State;			//���翩��
};