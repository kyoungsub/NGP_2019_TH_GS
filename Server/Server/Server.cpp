#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include<iostream>
#include<io.h>
#include<fcntl.h>

#include "Datastruct.h"
#include "Boss.h"

#define SERVERPORT 9000
#define BUFSIZE 1024

Object g_Object[MAX_OBJECTS];
int player1 = 0;		//1P = 0, 2P = 1의 값을 받음
int player1RID;
int player2RID;

HANDLE recvsendHandle[2];
HANDLE gameLogicHandle;

HANDLE wait_Recv[2];
HANDLE wait_Send;

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
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

//Object를 RSData로 변환
void ObjectToRS(Object a, RecvSendData* b) {
	b->posX = a.posX;
	b->posY = a.posY;
	b->posZ = a.posZ;
	b->VelX = a.velX;
	b->VelY = a.velY;
	b->VelZ = a.velZ;
	b->type = a.type;
	b->idx_num = a.idx_num;
}

//RSData를 Object로 변환
void RSToObject(Object* a, RecvSendData* b) {
	a->posX = b->posX;
	a->posY = b->posY;
	a->posZ = b->posZ;
	a->velX = b->VelX;
	a->velY = b->VelY;
	a->velZ = b->VelZ;
	a->type = b->type;
	a->idx_num = b->idx_num;
}

//송신 스레드
DWORD WINAPI RecvSendThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int len;
	char buf[BUFSIZE];
	InitData* initial_data = nullptr;
	RecvSendData* RS_data = nullptr;
	RecvSendData SendData;
	SendData.idx_num = 0;

	addrlen = sizeof(client_sock);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);
	
	while (1) {
		//data 받기
		retval = recvn(client_sock, (char *)& len, sizeof(int), 0);
		//if (retval == SOCKET_ERROR) err_display("recv()");
		retval = recvn(client_sock, buf, len, 0);
		//if (retval == SOCKET_ERROR) err_display("recv()");

		int i = 0;
		if (len == sizeof(InitData)) {
			initial_data = (InitData*)&buf;
			//디버깅용 출력코드
			printf("mass : %f, size : %f %f %f, coef_frict : %f \n",
				initial_data->mass, initial_data->sizeX, initial_data->sizeY, initial_data->sizeZ, initial_data->coef_Frict);
		}

		if (initial_data->mass == 0.15f) {		//플레이어 데이터
			if (player1 == 0)		//1번플레이어 없을때
			{
				g_Object[HERO_ID].posX = -0.5f;
				g_Object[HERO_ID].posY = 0.f;
				g_Object[HERO_ID].posZ = 0.f;
				g_Object[HERO_ID].velX = 0.f;
				g_Object[HERO_ID].velY = 0.f;
				g_Object[HERO_ID].velZ = 0.f;
				g_Object[HERO_ID].type = KIND_HERO;
				g_Object[HERO_ID].idx_num = HERO_ID;
				g_Object[HERO_ID].mass = initial_data->mass;
				g_Object[HERO_ID].sizeX = initial_data->sizeX;
				g_Object[HERO_ID].sizeY = initial_data->sizeY;
				g_Object[HERO_ID].sizeZ = initial_data->sizeZ;
				g_Object[HERO_ID].coefFriction = initial_data->coef_Frict;
				g_Object[HERO_ID].updated_1p = true;
				g_Object[HERO_ID].updated_2p = true;

				player1RID = GetCurrentThreadId();
				player1 += 1;
			}
			else if(player1 == 1){								//2번플레이어
				g_Object[HERO_ID2].posX = 0.5f;
				g_Object[HERO_ID2].posY = 0.f;
				g_Object[HERO_ID2].posZ = 0.f;
				g_Object[HERO_ID2].velX = 0.f;
				g_Object[HERO_ID2].velY = 0.f;
				g_Object[HERO_ID2].velZ = 0.f;
				g_Object[HERO_ID2].type = KIND_HERO;
				g_Object[HERO_ID2].idx_num = HERO_ID2;
				g_Object[HERO_ID2].mass = initial_data->mass;
				g_Object[HERO_ID2].sizeX = initial_data->sizeX;
				g_Object[HERO_ID2].sizeY = initial_data->sizeY;
				g_Object[HERO_ID2].sizeZ = initial_data->sizeZ;
				g_Object[HERO_ID2].coefFriction = initial_data->coef_Frict;
				g_Object[HERO_ID2].updated_1p = true;
				g_Object[HERO_ID2].updated_2p = true;

				player2RID = GetCurrentThreadId();
				player1 += 1;
			}
		}

		if (len == sizeof(RecvSendData)) {
			RS_data = (RecvSendData*)&buf;
			if (player2RID == GetCurrentThreadId()) {
				if (RS_data->idx_num == HERO_ID) {
					RS_data->idx_num = HERO_ID2;
				}
				else if (RS_data->idx_num == HERO_ID2) {
					RS_data->idx_num = HERO_ID;
				}
			}
			RSToObject(&g_Object[RS_data->idx_num], RS_data);

			if (RS_data->type == KIND_BULLET) {
				g_Object[RS_data->idx_num].mass = 0.2f;
				g_Object[RS_data->idx_num].coefFriction = 5.0f;
				g_Object[RS_data->idx_num].sizeX = 0.75f;
				g_Object[RS_data->idx_num].sizeY = 0.75f;
				g_Object[RS_data->idx_num].sizeZ = 0.75f;

				if (RS_data->VelX == 0.f  && RS_data->VelY == 0.f) {
					g_Object[RS_data->idx_num].updated_1p = false;
					g_Object[RS_data->idx_num].updated_2p = false;
				}
			}
			else {
				g_Object[RS_data->idx_num].updated_1p = true;
				g_Object[RS_data->idx_num].updated_2p = true;
			}
			//디버깅용 출력코드
			//printf("Pos : %f %f %f, Vel : %f %f %f, type: %d, idx_num : %d\n",
			//	RS_data->posX, RS_data->posY, RS_data->posZ,
			//	RS_data->VelX, RS_data->VelY, RS_data->VelZ,
			//	RS_data->type, RS_data->idx_num);
		}

		if (GetCurrentThreadId() == player1RID)
			SetEvent(wait_Recv[0]);
		else if (GetCurrentThreadId() == player2RID)
			SetEvent(wait_Recv[1]);


		/////////////////////////////SEND
		if(player1 == 2)
			WaitForSingleObject(wait_Send, INFINITE);

		
		//g_Object에 있는 값을 RecvSendData에 넣어 보낸다.
		if (player2RID == GetCurrentThreadId()) {
			Object temp = g_Object[HERO_ID2];
			g_Object[HERO_ID2] = g_Object[HERO_ID];
			g_Object[HERO_ID] = temp;

			g_Object[HERO_ID].idx_num = HERO_ID;
			g_Object[HERO_ID2].idx_num = HERO_ID2;
		}

		for (int i = 0; i < MAX_OBJECTS; ++i) {
			if (g_Object[i].updated_1p == true || g_Object[i].updated_2p == true) {

				ObjectToRS(g_Object[i], &SendData);
				len = sizeof(SendData);

				retval = send(client_sock, (char*)& len, sizeof(int), 0);
				retval = send(client_sock, (char*)& SendData, len, 0);

				if(GetCurrentThreadId() == player1RID)
					g_Object[i].updated_1p = false;
				else if (GetCurrentThreadId() == player2RID)
					g_Object[i].updated_2p = false;

				//디버깅용 출력코드
				printf("(%d) Pos : %f %f %f, Vel : %f %f %f, type: %d, idx_num : %d\n", GetCurrentThreadId(),
					SendData.posX, SendData.posY, SendData.posZ,
					SendData.VelX, SendData.VelY, SendData.VelZ,
					SendData.type, SendData.idx_num);
			}
		}
		


	}

	return 0;
}

DWORD WINAPI GameLogicThread(LPVOID arg)
{
	//initialize boss data
	//Boss data = g_Object[3]
	Boss Boss;
	Boss.InitBoss();
	g_Object[3] = Boss.BossUnit;
	for (int i = 0; i < MAX_OBJECTS; ++i)
		g_Object[i].idx_num = i;
	
	//보스갱신및 충돌체크
	while (1) {
		//Recv가 끝나면 시작
		WaitForMultipleObjects(2, wait_Recv, TRUE, INFINITE);


		//종료시 Send 시작
		SetEvent(wait_Send);
	}

	return 0;
}


int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)& serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	gameLogicHandle = CreateThread(NULL, 0, GameLogicThread, NULL, 0, NULL);

	wait_Recv[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	wait_Recv[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

	wait_Send = CreateEvent(NULL, FALSE, FALSE, NULL);

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		if (player1 == 0) {
			recvsendHandle[0] = CreateThread(NULL, 0, RecvSendThread, (LPVOID)client_sock, 0, NULL);
		}
		else if (player1 == 1) {
			recvsendHandle[1] = CreateThread(NULL, 0, RecvSendThread, (LPVOID)client_sock, 0, NULL);
		}




		//if ((SendHandle[0] == NULL&& RecvHandle[0] == NULL)) closesocket(client_sock);
		//else {
		//	CloseHandle(RecvHandle[0]);
		//	CloseHandle(RecvHandle[1]);
		//	CloseHandle(SendHandle[0]);
		//	CloseHandle(SendHandle[1]);
		//}

	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
