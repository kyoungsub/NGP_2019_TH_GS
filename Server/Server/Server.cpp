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
void ObjectToRS(Object a, RecvSendData b) {
	b.posX = a.posX;
	b.posY = a.posY;
	b.posZ = a.posZ;
	b.VelX = a.velX;
	b.VelY = a.velY;
	b.VelZ = a.velZ;
	b.type = a.type;
	b.idx_num = a.idx_num;
}

//RSData를 Object로 변환
void RSToObject(Object a, RecvSendData b) {
	a.posX = b.posX;
	a.posY = b.posY;
	a.posZ = b.posZ;
	a.velX = b.VelX;
	a.velY = b.VelY;
	a.velZ = b.VelZ;
	a.type = b.type;
	a.idx_num = b.idx_num;
}

void Update() {

}

//송신 스레드
DWORD WINAPI RecvThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int len;
	char buf[BUFSIZE];
	InitData* initial_data = nullptr;
	RecvSendData* RS_data;

	bool player1;


	addrlen = sizeof(client_sock);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);
	
	while (1) {
		//data 받기
		retval = recvn(client_sock, (char *)& len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) err_display("recv()");
		retval = recvn(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) err_display("recv()");

		if (len == sizeof(InitData)) {
			initial_data = (InitData*)&buf;
			//디버깅용 출력코드
			printf("mass : %f, size : %f %f %f, coef_frict : %f \n",
				initial_data->mass, initial_data->sizeX, initial_data->sizeY, initial_data->sizeZ, initial_data->coef_Frict);
		}

		if (initial_data->mass == 0.15f) {		//플레이어 데이터
			if (g_Object[0].posX == NULL)		//1번플레이어 없을때
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

				player1 = true;
				send(client_sock, (char*)player1, sizeof(player1), 0);
			}
			else {								//2번플레이어
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

				player1 = false;
				send(client_sock, (char*)player1, sizeof(player1), 0);
			}
		}
		else if (initial_data->mass == 0.2f) {		//총알

		}


		if (len == sizeof(RecvSendData)) {
			RS_data = (RecvSendData*)&buf;
			//디버깅용 출력코드
			printf("Pos : %f %f %f, Vel : %f %f %f, type: %d, idx_num : %d\n",
				RS_data->posX, RS_data->posY, RS_data->posZ,
				RS_data->VelX, RS_data->VelY, RS_data->VelZ,
				RS_data->type, RS_data->idx_num);
		}
	}

	return 0;
}

DWORD WINAPI SendThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;
	bool both_exist;
	int len;

	RecvSendData SendData;
	SendData.idx_num = 0;

	addrlen = sizeof(client_sock);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	if (g_Object[0].posX != NULL && g_Object[1].posX != NULL) {
		both_exist = true;
		send(client_sock, (char*)&both_exist, sizeof(both_exist), 0);
	}

	//g_Object에 있는 값을 RecvSendData에 넣어 보낸다.
	for (int i = 0; i < MAX_OBJECTS; ++i) {
		if (g_Object[i].posX == NULL)
			break;

		ObjectToRS(g_Object[i], SendData);
		len = sizeof(SendData);

		retval = send(client_sock, (char*)& len, sizeof(int), 0);
		retval = send(client_sock, (char*)& SendData, len, 0);
	}

	return 0;
}


int main(int argc, char* argv[])
{
	int retval;

	//initialize boss data
	//Boss data = g_Object[3]
	Boss Boss;
	Boss.InitBoss();
	g_Object[2] = Boss.BossUnit;

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

	HANDLE RecvHandle;
	HANDLE SendHandle;

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


		RecvHandle = CreateThread(NULL, 0, RecvThread, (LPVOID)client_sock, 0, NULL);

		SendHandle = CreateThread(NULL, 0, SendThread, (LPVOID)client_sock, 0, NULL);
		if (RecvHandle == NULL || SendHandle == NULL) closesocket(client_sock);
		else {
			CloseHandle(RecvHandle);
			CloseHandle(SendHandle);
		}

	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
