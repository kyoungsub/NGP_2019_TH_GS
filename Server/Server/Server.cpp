#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include<io.h>
#include<fcntl.h>
#include <list>

#include "stdafx.h"
#include "Global.h"
#include "Object.h"

#define SERVERPORT 9000
#define BUFSIZE 1024

Object g_Object[MAX_OBJECTS];
std::list<char> player1Event;
std::list<char> player2Event;

HANDLE recvsendHandle[2];
HANDLE gameLogicHandle;

HANDLE wait_Recv[2];
HANDLE wait_Send;

int player1ID;
int player2ID;

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

//송신 스레드
DWORD WINAPI RecvSendThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int len;
	char buf[BUFSIZE];
	int player_num;

	addrlen = sizeof(client_sock);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	if (GetCurrentThreadId() == player1ID) {
		g_Object[HERO_ID].SetPos(-0.5f, 0.0f, 0.0f);
		g_Object[HERO_ID].SetVel(0.f, 0.f, 0.f);
		g_Object[HERO_ID].SetAcc(0.0f, 0.0f, 0.0f);
		g_Object[HERO_ID].SetSize(0.6f, 0.6f, 0.6f);
		g_Object[HERO_ID].SetMass(0.15f);
		g_Object[HERO_ID].SetCoefFrict(0.5f);
		g_Object[HERO_ID].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		g_Object[HERO_ID].SetKind(KIND_HERO);
		g_Object[HERO_ID].SetHP(240);
		g_Object[HERO_ID].SetState(STATE_GROUND);
		player_num = HERO_ID;
	}
	else if (GetCurrentThreadId() == player2ID) {
		g_Object[HERO_ID2].SetPos(0.5f, 0.0f, 0.0f);
		g_Object[HERO_ID2].SetVel(0.f, 0.f, 0.f);
		g_Object[HERO_ID2].SetAcc(0.0f, 0.0f, 0.0f);
		g_Object[HERO_ID2].SetSize(0.6f, 0.6f, 0.6f);
		g_Object[HERO_ID2].SetMass(0.15f);
		g_Object[HERO_ID2].SetCoefFrict(0.5f);
		g_Object[HERO_ID2].SetColor(1.0f, 1.0f, 1.0f, 1.0f);
		g_Object[HERO_ID2].SetKind(KIND_HERO);
		g_Object[HERO_ID2].SetHP(240);
		g_Object[HERO_ID2].SetState(STATE_GROUND);
		player_num = HERO_ID2;
	}

	while (1) {
		//data 받기
		retval = recvn(client_sock, (char *)& len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) err_display("recv()");
		retval = recvn(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) err_display("recv()");

		if (player_num == 0) {
			for (int i = 0; i < len; ++i)
				player1Event.push_back(buf[i]);
		}
		else if (player_num == 1) {
			for (int i = 0; i < len; ++i)
				player2Event.push_back(buf[i]);
		}

		SetEvent(wait_Recv[player_num]);

		/////////////////////////////SEND
		WaitForSingleObject(wait_Send, INFINITE);

		//while (len > 0) {
		//	memcpy(RS_data, buf + len, sizeof(RecvSendData));
		//}

		retval = send(client_sock, (char*)& len, sizeof(int), 0);
		retval = send(client_sock, buf, len, 0);


	}
	return 0;
}

DWORD WINAPI GameLogicThread(LPVOID arg)
{
	//initialize boss data
	//Boss data = g_Object[3]

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
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		if (recvsendHandle[0] == NULL) {
			recvsendHandle[0] = CreateThread(NULL, 0, RecvSendThread, (LPVOID)client_sock, 0, NULL);
			player1ID = GetThreadId(recvsendHandle[0]);
		}
		else if (recvsendHandle[1] == NULL) {
			recvsendHandle[1] = CreateThread(NULL, 0, RecvSendThread, (LPVOID)client_sock, 0, NULL);
			player2ID = GetThreadId(recvsendHandle[1]);
		}

	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
