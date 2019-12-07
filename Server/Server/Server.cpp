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
#include "ScnMgr.h"
#include "Object.h"

#define SERVERPORT 9000

ScnMgr* g_ScnMgr = NULL;

std::list<int> player1Event;
std::list<int> player2Event;
int player1EventSize = 0;
int player2EventSize = 0;

HANDLE recvHandle[2];
HANDLE sendHandle[2];

HANDLE wait_Recv[2];
HANDLE wait_Logic[2];
CRITICAL_SECTION cs;

DWORD g_PrevTime = 0;
DWORD g_ShootStartTime = 0;
DWORD ShootElapsedTime = 0;
float eTime;

int player1ID;
int playerS1ID;
int player2ID;
int playerS2ID;

bool now_play = FALSE;

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
DWORD WINAPI RecvThread(LPVOID arg)
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
		player_num = HERO_ID;
	}
	else if (GetCurrentThreadId() == player2ID) {
		player_num = HERO_ID2;
	}

	while (now_play == FALSE) {
		
	}

	if(player_num == 0)
		send(client_sock, (char*)&now_play, sizeof(bool), 0);
	else if(player_num == 1)
		send(client_sock, (char*)&now_play, sizeof(bool), 0);
	

	//Recv Data
	while (now_play == TRUE) {
		retval = recvn(client_sock, (char *)& len, sizeof(int), 0);
		retval = recvn(client_sock, buf, len, 0);

		if (player_num == HERO_ID) {
			if (player1EventSize == 0) {
				for (int i = 0; i < len; ++i) {
					player1Event.push_back(buf[i]);
					player1EventSize += 1;
				}
			}
		}
		else if (player_num == HERO_ID2) {
			if (player2EventSize == 0) {
				for (int i = 0; i < len; ++i) {
					player2Event.push_back(buf[i]);
					player2EventSize += 1;
				}
			}
		}

		SetEvent(wait_Recv[player_num]);
	}
	return 0;
}

DWORD WINAPI SendThread(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int player_num;
	char buf[BUFSIZE];

	addrlen = sizeof(client_sock);
	getpeername(client_sock, (SOCKADDR*)& clientaddr, &addrlen);

	if (GetCurrentThreadId() == playerS1ID)
		player_num = HERO_ID;
	else if(GetCurrentThreadId() == playerS2ID)
		player_num = HERO_ID2;

	while (1) {
		//갱신 끝나고 송신
		WaitForSingleObject(wait_Logic[player_num], INFINITE);

		int read_data = 0;
		SendData sData;
		float temp;
		for (int i = 0; i < MAX_OBJECTS; ++i) {
			if (g_ScnMgr->m_Objects[i] != NULL) {
				g_ScnMgr->m_Objects[i]->GetPos(&sData.posX, &sData.posY, &temp);
				g_ScnMgr->m_Objects[i]->GetKind(&sData.type);
				g_ScnMgr->m_Objects[i]->GetHP(&sData.hp);
				sData.idx_num = i;

				if (sData.type == KIND_BOSS) {
					dynamic_cast<Boss*>(g_ScnMgr->m_Objects[i])->GetSeqX(&sData.SeqX);
					dynamic_cast<Boss*>(g_ScnMgr->m_Objects[i])->GetSeqY(&sData.SeqY);
				}
				else if (sData.type == KIND_MONSTER) {
					dynamic_cast<Monster*>(g_ScnMgr->m_Objects[i])->GetSeqX(&sData.SeqX);
					dynamic_cast<Monster*>(g_ScnMgr->m_Objects[i])->GetSeqY(&sData.SeqY);
				}
				else {
					sData.SeqX = 0;
					sData.SeqY = 0;
				}

				memcpy(buf + read_data, (void*)&sData, sizeof(SendData));
				read_data += sizeof(SendData);
			}
		}

		EventSet eventbuf;
		eventbuf = g_ScnMgr->eventflag;

		send(client_sock, (char*)& read_data, sizeof(int), 0);
		send(client_sock, buf, read_data, 0);
		send(client_sock, (char*)&eventbuf, sizeof(EventSet), 0);

		ZeroMemory(buf, BUFSIZE);	
	}

	return 0;
}

void Initialize_data() {
	g_ScnMgr->m_Objects[HERO_ID] = new Object();
	g_ScnMgr->m_Objects[HERO_ID]->SetPos(-0.5f, 0.0f, 0.0f);
	g_ScnMgr->m_Objects[HERO_ID]->SetVel(0.f, 0.f, 0.f);
	g_ScnMgr->m_Objects[HERO_ID]->SetAcc(0.0f, 0.0f, 0.0f);
	g_ScnMgr->m_Objects[HERO_ID]->SetSize(0.6f, 0.6f, 0.6f);
	g_ScnMgr->m_Objects[HERO_ID]->SetMass(0.15f);
	g_ScnMgr->m_Objects[HERO_ID]->SetCoefFrict(0.5f);
	g_ScnMgr->m_Objects[HERO_ID]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	g_ScnMgr->m_Objects[HERO_ID]->SetKind(KIND_HERO);
	g_ScnMgr->m_Objects[HERO_ID]->SetHP(240);
	g_ScnMgr->m_Objects[HERO_ID]->SetState(STATE_GROUND);

	g_ScnMgr->m_Objects[HERO_ID2] = new Object();
	g_ScnMgr->m_Objects[HERO_ID2]->SetPos(0.5f, 0.0f, 0.0f);
	g_ScnMgr->m_Objects[HERO_ID2]->SetVel(0.f, 0.f, 0.f);
	g_ScnMgr->m_Objects[HERO_ID2]->SetAcc(0.0f, 0.0f, 0.0f);
	g_ScnMgr->m_Objects[HERO_ID2]->SetSize(0.6f, 0.6f, 0.6f);
	g_ScnMgr->m_Objects[HERO_ID2]->SetMass(0.15f);
	g_ScnMgr->m_Objects[HERO_ID2]->SetCoefFrict(0.5f);
	g_ScnMgr->m_Objects[HERO_ID2]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	g_ScnMgr->m_Objects[HERO_ID2]->SetKind(KIND_HERO);
	g_ScnMgr->m_Objects[HERO_ID2]->SetHP(240);
	g_ScnMgr->m_Objects[HERO_ID2]->SetState(STATE_GROUND);

	// Create Door
	g_ScnMgr->m_Objects[2] = new Object();
	g_ScnMgr->m_Objects[2]->SetPos(0.0f, 1.25f, 0.0f);
	g_ScnMgr->m_Objects[2]->SetVel(0.f, 0.f, 0.f);
	g_ScnMgr->m_Objects[2]->SetAcc(0.0f, 0.0f, 0.0f);
	g_ScnMgr->m_Objects[2]->SetSize(1.0f, 1.0f, 1.0f);
	g_ScnMgr->m_Objects[2]->SetMass(0.15f);
	g_ScnMgr->m_Objects[2]->SetCoefFrict(0.5f);
	g_ScnMgr->m_Objects[2]->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	g_ScnMgr->m_Objects[2]->SetKind(KIND_BOSS_DOOR);
	g_ScnMgr->m_Objects[2]->SetHP(240);
	g_ScnMgr->m_Objects[2]->SetState(STATE_GROUND);
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

	//게임로직
	g_PrevTime = GetTickCount();
	g_ScnMgr = new ScnMgr();

	//객체 초기화
	Initialize_data();

	wait_Recv[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	wait_Recv[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

	wait_Logic[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	wait_Logic[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

	while (now_play == FALSE) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		if (recvHandle[0] == NULL) {
			recvHandle[0] = CreateThread(NULL, 0, RecvThread, (LPVOID)client_sock, 0, NULL);
			sendHandle[0] = CreateThread(NULL, 0, SendThread, (LPVOID)client_sock, 0, NULL);
			player1ID = GetThreadId(recvHandle[0]);
			playerS1ID = GetThreadId(sendHandle[0]);
		}
		else if (recvHandle[1] == NULL) {
			recvHandle[1] = CreateThread(NULL, 0, RecvThread, (LPVOID)client_sock, 0, NULL);
			sendHandle[1] = CreateThread(NULL, 0, SendThread, (LPVOID)client_sock, 0, NULL);
			player2ID = GetThreadId(recvHandle[1]);
			playerS2ID = GetThreadId(sendHandle[1]);
			now_play = TRUE;
		}
	}
	while (1) {
		WaitForMultipleObjects(2, wait_Recv, TRUE, INFINITE);

		// Calc Elapsed Time
		if (g_PrevTime == 0) {
			g_PrevTime = GetTickCount();
			return 0;
		}
		DWORD CurTime = GetTickCount();
		DWORD ElapsedTime = CurTime - g_PrevTime;
		g_PrevTime = CurTime;

		eTime = (float)ElapsedTime / 1000.0f;

		// Calc Shoot Delay
		DWORD ShootCurTime = GetTickCount();
		ShootElapsedTime = ShootCurTime - g_ShootStartTime;

		//Apply Force
		float forceX1 = 0.0f, forceX2 = 0.0f;
		float forceY1 = 0.0f, forceY2 = 0.0f;
		float forceZ = 0.0f;
		float amount = 1.0f;
		int bulletID1 = SHOOT_NONE;
		int bulletID2 = SHOOT_NONE;

		//1p,2p 키입력을 통한 연산
		if (player1EventSize == 8) {
			forceY1 += amount * player1Event.front();
			player1Event.pop_front();
			player1EventSize -= 1;
			forceX1 -= amount * player1Event.front();
			player1Event.pop_front();
			player1EventSize -= 1;
			forceY1 -= amount * player1Event.front();
			player1Event.pop_front();
			player1EventSize -= 1;
			forceX1 += amount * player1Event.front();
			player1Event.pop_front();
			player1EventSize -= 1;
			g_ScnMgr->ApplyForce(forceX1, forceY1, forceZ, HERO_ID, eTime);

			//총알키값
			for (int i = 0; i < 4; i++) {
				if (player1Event.front() == TRUE)
					bulletID1 = i + 1;
				player1Event.pop_front();
				player1EventSize -= 1;
			}
			if (ShootElapsedTime % 50 == 0) { // Shoot
				g_ScnMgr->Shoot(HERO_ID, bulletID1);
			}
		}
		if (player2EventSize == 8) {
			forceY2 += amount * player2Event.front();
			player2Event.pop_front();
			player2EventSize -= 1;
			forceX2 -= amount * player2Event.front();
			player2Event.pop_front();
			player2EventSize -= 1;
			forceY2 -= amount * player2Event.front();
			player2Event.pop_front();
			player2EventSize -= 1;
			forceX2 += amount * player2Event.front();
			player2Event.pop_front();
			player2EventSize -= 1;
			g_ScnMgr->ApplyForce(forceX2, forceY2, forceZ, HERO_ID2, eTime);

			//총알키값
			for (int i = 0; i < 4; i++) {
				if (player2Event.front() == TRUE)
					bulletID2 = i + 1;
				player2Event.pop_front();
				player2EventSize -= 1;
			}
			if (ShootElapsedTime % 50 == 0) { // Shoot
				g_ScnMgr->Shoot(HERO_ID2, bulletID2);
			}
		}

		g_ScnMgr->GarbageCollector();   // 화면 밖으로 나가는 오브젝트 삭제
		g_ScnMgr->DoCollisionTest();
		g_ScnMgr->Update(eTime);


		SetEvent(wait_Logic[0]);
		SetEvent(wait_Logic[1]);

	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
