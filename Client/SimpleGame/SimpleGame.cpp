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

bool now_play = FALSE;
bool map_switch = FALSE;
EventSet eventbuf;
HANDLE wait_start;
HANDLE wait_Update;

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

	recvData rData;

	while (now_play == TRUE){
		int curread = 0;

		recvn(sock, (char *)&len, sizeof(int), 0);
		recvn(sock, buf, len, 0);
		recvn(sock, (char *)&eventbuf, sizeof(EventSet), 0);

		
		//if (len <= 84) {			
			// Garbage Collector
			for (int i = 3; i < MAX_OBJECTS; ++i) {
				if (g_ScnMgr->m_Objects[i] != NULL) {
					g_ScnMgr->DeleteObject(i);
				}
			}
		//}
		
		while (len > 0) {
			
			memcpy((void*)&rData, buf + curread, sizeof(recvData));

			
			int idx_num = rData.idx_num;

			if (rData.type == KIND_HERO) {
				if (g_ScnMgr->m_Objects[rData.idx_num] == NULL) {
					// Create Player Character
					g_ScnMgr->m_Objects[idx_num] = new Object();
					g_ScnMgr->m_Objects[idx_num]->SetPos(rData.posX, rData.posY, 0.0f);
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
				g_ScnMgr->m_Objects[idx_num]->SetPos(rData.posX, rData.posY, temp);
				g_ScnMgr->m_Objects[idx_num]->SetHP(rData.hp);

				int index = rData.hp / 40 / 2;
				int SeqX = rData.hp / 40 % 2;
				int hp;		

				if (index >= 0) {
					if (SeqX == 0) {
						g_ScnMgr->DeleteUI(idx_num, index);
					}

					if (g_ScnMgr->m_HP[idx_num][index] != NULL) {
						g_ScnMgr->m_HP[idx_num][index]->SetHP(rData.hp);
						dynamic_cast<UI*>(g_ScnMgr->m_HP[idx_num][index])->SetSeqX(SeqX);
					}
				}

			}
			else if (rData.type == KIND_BULLET) {
				// Create Bullet
				if (g_ScnMgr->m_Objects[rData.idx_num] == NULL) {
					g_ScnMgr->AddObject(0.f, 0.f, 0.f, 0.75f, 0.75f, 0.75f, 0, 0, 0, KIND_BULLET, 20, idx_num);
				}
				g_ScnMgr->m_Objects[rData.idx_num]->SetPos(rData.posX, rData.posY, temp);
			}
			else if (rData.type == KIND_BOSS_DOOR) {
				// Create Door
				if (g_ScnMgr->m_Objects[rData.idx_num] == NULL) {
					g_ScnMgr->AddObject(rData.posX, rData.posY, 0.f, 1.25f, 1.25f, 1.25f, 0, 0, 0, KIND_BOSS_DOOR, 20, idx_num);
				}
			}
			else if (rData.type == KIND_BOSS) {
				// Create BOSS 
				if (g_ScnMgr->m_Objects[rData.idx_num] == NULL) {
					g_ScnMgr->AddObject(rData.posX, rData.posY, 0.f, 1.f, 1.f, 1.f, 0, 0, 0, KIND_BOSS, 200, idx_num);
				}
				int kind;

				g_ScnMgr->m_Objects[rData.idx_num]->GetKind(&kind);
				g_ScnMgr->m_Objects[idx_num]->SetHP(rData.hp);

				if (kind == KIND_BOSS_DOOR) {
					g_ScnMgr->AddObject(rData.posX, rData.posY, 0.f, 1.f, 1.f, 1.f, 0, 0, 0, KIND_BOSS, 200, idx_num);
				}
				g_ScnMgr->m_Objects[rData.idx_num]->SetPos(rData.posX, rData.posY, temp);

				if (kind == KIND_BOSS)
					dynamic_cast<Boss*>(g_ScnMgr->m_Objects[rData.idx_num])->SetSeq(rData.SeqX, rData.SeqY);
			}
			else if (rData.type == KIND_MONSTER) {				
				// Create MONSTER
				if (g_ScnMgr->m_Objects[rData.idx_num] == NULL) {
					g_ScnMgr->AddObject(rData.posX, rData.posY, 0.f, 0.6f, 0.6f, 0.6f, 0, 0, 0, KIND_MONSTER, 60, idx_num);
				}
				g_ScnMgr->m_Objects[rData.idx_num]->SetPos(rData.posX, rData.posY, temp);

				int kind;
				g_ScnMgr->m_Objects[rData.idx_num]->GetKind(&kind);
				if(kind == KIND_MONSTER)
					dynamic_cast<Monster*>(g_ScnMgr->m_Objects[rData.idx_num])->SetSeq(rData.SeqX, rData.SeqY);
			}

			curread += sizeof(recvData);
			len -= sizeof(recvData);
		}

		SetEvent(wait_Update);
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

	WaitForSingleObject(wait_Update, INFINITE);
	g_ScnMgr->RenderScene();   // Render  

	if (eventbuf.changeMap == TRUE && map_switch == FALSE) {
		g_ScnMgr->ChangeMap(BOSSROOM);
		map_switch = TRUE;
	}

	glutSwapBuffers();

	ResetEvent(wait_Update);
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
}

void SetKeyRepeat() {

}


void SpecialKeyDownInput(int key, int x, int y) {

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

	wait_Update = CreateEvent(NULL, FALSE, FALSE, NULL);

	hThread[0] = CreateThread(NULL, 0, RecvThread, (LPVOID)g_sock, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, SendThread, (LPVOID)g_sock, 0, NULL);

	g_PrevTime = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc(16, RenderScene, 0);

	glutMainLoop();

	delete g_ScnMgr;

	return 0;

}