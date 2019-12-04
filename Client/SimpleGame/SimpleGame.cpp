/*

Copyright 2017 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.

*/

//#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include "stdafx.h"
#include <iostream>
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"
#include "Global.h"
#include "ScnMgr.h"

ScnMgr *g_ScnMgr = NULL;

DWORD g_PrevTime = 0;

BOOL g_KeyW = FALSE;
BOOL g_KeyA = FALSE;
BOOL g_KeyS = FALSE;
BOOL g_KeyD = FALSE;
BOOL g_KeySP = FALSE;

int g_Shoot = SHOOT_NONE;
DWORD g_ShootStartTime = 0;

bool now_play = FALSE;
HANDLE wait_start;

SOCKET g_sock;

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

//송신 스레드
DWORD WINAPI RecvThread(LPVOID arg)
{
	int retval;
	SOCKET sock = (SOCKET)arg; 
	int len;
	
	float temp = 0.f;
	char buf[BUFSIZE];	

	recvData PlayerInfo;

	while (now_play == TRUE){
		int curread = 0;

		recvn(sock, (char *)&len, sizeof(int), 0);
		recvn(sock, buf, len, 0);

		if (len <= 20) {
			int kind;

			for (int i = HERO_ID + 2; i < MAX_OBJECTS; ++i) {
				g_ScnMgr->DeleteObject(i);
			}
		}

		while (len > 0) {
			memcpy((void*)&PlayerInfo, buf + curread, sizeof(recvData));

			int idx_num = PlayerInfo.idx_num;

			if (PlayerInfo.type == KIND_HERO) {
				if (g_ScnMgr->m_Objects[PlayerInfo.idx_num] == NULL) {
					g_ScnMgr->m_Objects[idx_num] = new Object();

					g_ScnMgr->m_Objects[idx_num]->SetPos(PlayerInfo.posX, PlayerInfo.posY, 0.0f);
					g_ScnMgr->m_Objects[idx_num]->SetVel(0.0f, 0.0f, 0.0f);
					g_ScnMgr->m_Objects[idx_num]->SetAcc(0.0f, 0.0f, 0.0f);
					g_ScnMgr->m_Objects[idx_num]->SetSize(0.6f, 0.6f, 0.6f);
					g_ScnMgr->m_Objects[idx_num]->SetMass(0.15f);
					g_ScnMgr->m_Objects[idx_num]->SetCoefFrict(0.5f);
					g_ScnMgr->m_Objects[idx_num]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
					g_ScnMgr->m_Objects[idx_num]->SetKind(KIND_HERO);
					g_ScnMgr->m_Objects[idx_num]->SetHP(240);
					g_ScnMgr->m_Objects[idx_num]->SetState(STATE_GROUND);
				}

				g_ScnMgr->m_Objects[idx_num]->SetPos(PlayerInfo.posX, PlayerInfo.posY, temp);
				//printf("ID: %d, %f, %f, %d, %d \n", PlayerInfo.idx_num, PlayerInfo.posX, PlayerInfo.posY, PlayerInfo.type, PlayerInfo.hp);
			}
			else if (PlayerInfo.type == KIND_BULLET) {
				if (g_ScnMgr->m_Objects[PlayerInfo.idx_num] == NULL) {
					g_ScnMgr->AddObject(0, 0, 0, 0.75f, 0.75f, 0.75f, 0, 0, 0, KIND_BULLET, 20, idx_num);
				}
				g_ScnMgr->m_Objects[PlayerInfo.idx_num]->SetPos(PlayerInfo.posX, PlayerInfo.posY, temp);
				//printf("ID: %d, %f, %f, %d, %d \n", PlayerInfo.idx_num, PlayerInfo.posX, PlayerInfo.posY, PlayerInfo.type, PlayerInfo.hp);
			}

			curread += sizeof(recvData);
			len -= sizeof(recvData);
		}

	}

	return 0;
}

DWORD WINAPI SendThread(LPVOID arg)
{
	int retval;
	SOCKET sock = (SOCKET)arg;
	int addrlen;
	int len;
	char buf[8];

	while (now_play == TRUE) {
		if (g_KeyW) {
			buf[0] = TRUE;
		}
		if (g_KeyA) {
			buf[1] = TRUE;
		}
		if (g_KeyS) {
			buf[2] = TRUE;
		}
		if (g_KeyD) {
			buf[3] = TRUE;
		}
		if (g_Shoot == SHOOT_LEFT) {
			buf[4] = TRUE;
		}
		if (g_Shoot == SHOOT_RIGHT) {
			buf[5] = TRUE;
		}
		if (g_Shoot == SHOOT_UP) {
			buf[6] = TRUE;
		}
		if (g_Shoot == SHOOT_DOWN) {
			buf[7] = TRUE;
		}

		if (!g_KeyW) {
			buf[0] = FALSE;
		}
		if (!g_KeyA) {
			buf[1] = FALSE;
		}
		if (!g_KeyS) {
			buf[2] = FALSE;
		}
		if (!g_KeyD) {
			buf[3] = FALSE;
		}
		if (g_Shoot == SHOOT_NONE) {
			buf[4] = FALSE;
			buf[5] = FALSE;
			buf[6] = FALSE;
			buf[7] = FALSE;
		}
		
		int retval;
		len = sizeof(buf);
		retval = send(sock, (char*)&len, sizeof(int), 0);
		retval = send(sock, buf, len, 0);

		Sleep(16);

	}

	return 0;
}

void RenderScene(int temp) {

	// Calc Elapsed Time
	if (g_PrevTime == 0) {
		g_PrevTime = GetTickCount();
		return;
	}
	DWORD CurTime = GetTickCount();
	DWORD ElapsedTime = CurTime - g_PrevTime;
	g_PrevTime = CurTime;
	float eTime = (float)ElapsedTime / 1000.0f;

	// Calc Shoot Delay
	DWORD ShootCurTime = GetTickCount();
	DWORD ShootElapsedTime = ShootCurTime - g_ShootStartTime;

	// Apply Force
	float forceX = 0.0f;
	float forceY = 0.0f;
	float forceZ = 0.0f;
	float amount = 1.0f;
	float zAmount = 20.0f;

	if (g_KeyW) {
		forceY += amount;
	}
	if (g_KeyS) {
		forceY -= amount;
	}
	if (g_KeyA) {
		forceX -= amount;
	}
	if (g_KeyD) {
		forceX += amount;
	}
	if (g_KeySP) {
		forceZ += zAmount;
	}

	g_ScnMgr->RenderScene();   // Render   
	if (ShootElapsedTime % 50 == 0) { // Shoot
		//g_ScnMgr->Shoot(g_Shoot);
	}
	//g_ScnMgr->GarbageCollector();   // 화면 밖으로 나가는 오브젝트 삭제
	g_ScnMgr->DoCollisionTest();

	glutSwapBuffers();

	glutTimerFunc(16, RenderScene, 0);
}

void Display(void) {
}

void Idle(void) {
}

void MouseInput(int button, int state, int x, int y) {
}

void KeyDownInput(unsigned char key, int x, int y) {
	if (key == 'w') {
		g_KeyW = TRUE;
	}
	if (key == 's') {
		g_KeyS = TRUE;
	}
	if (key == 'a') {
		g_KeyA = TRUE;
	}
	if (key == 'd') {
		g_KeyD = TRUE;
	}
	if (key == ' ') {
		g_KeySP = TRUE;
	}
}

void KeyUpInput(unsigned char key, int x, int y) {
	if (key == 'w') {
		g_KeyW = FALSE;
	}
	if (key == 's') {
		g_KeyS = FALSE;
	}
	if (key == 'a') {
		g_KeyA = FALSE;
	}
	if (key == 'd') {
		g_KeyD = FALSE;
	}
	if (key == ' ') {
		g_KeySP = FALSE;
	}
}

void SetKeyRepeat() {

}


void SpecialKeyDownInput(int key, int x, int y) {
	g_ShootStartTime = GetTickCount();

	switch (key)
	{
	case GLUT_KEY_UP:
		g_Shoot = SHOOT_UP;
		break;
	case GLUT_KEY_DOWN:
		g_Shoot = SHOOT_DOWN;
		break;
	case GLUT_KEY_LEFT:
		g_Shoot = SHOOT_LEFT;
		break;
	case GLUT_KEY_RIGHT:
		g_Shoot = SHOOT_RIGHT;
		break;
	default:
		break;
	}

}

void SpecialKeyUpInput(int key, int x, int y) {
	g_Shoot = SHOOT_NONE;
}

int main(int argc, char **argv) {

	//-----------------------------------------------------------------------------------//
	int retval;
	char ipaddress[16];
	printf("Input Ip Address:");
	fgets(ipaddress, sizeof(ipaddress), stdin);
	if (ipaddress == NULL) {
		return 1;
	}
	//ipaddress[strlen(ipaddress) - 1] = '\0';

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipaddress);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(g_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");	
	
	//-----------------------------------------------------------------------------------//

	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(300, 200);
	glutInitWindowSize(WINDOWS_WIDTH, WINDOWS_HEIGHT);

	glutCreateWindow("Game Software Engineering KPU");

	glewInit();

	if (glewIsSupported("GL_VERSION_3_0")) {
		std::cout << " GLEW Version is 3.0\n ";
	}
	else {
		std::cout << "GLEW 3.0 not supported\n ";
	}

	g_PrevTime = GetTickCount();

	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyDownInput);
	glutKeyboardUpFunc(KeyUpInput);
	glutMouseFunc(MouseInput);
	glutSpecialFunc(SpecialKeyDownInput);
	glutSpecialUpFunc(SpecialKeyUpInput);

	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	// Init ScnMgr
	g_ScnMgr = new ScnMgr();

	HANDLE hThread[2];

	wait_start = CreateEvent(NULL, TRUE, FALSE, NULL);

	while (now_play == FALSE) {
		recvn(g_sock, (char*)&now_play, sizeof(bool), 0);
	}

	hThread[0] = CreateThread(NULL, 0, RecvThread, (LPVOID)g_sock, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, SendThread, (LPVOID)g_sock, 0, NULL);

	g_PrevTime = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc(16, RenderScene, 0);

	glutMainLoop();

	delete g_ScnMgr;

	return 0;

}