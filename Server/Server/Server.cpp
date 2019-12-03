#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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
#define BUFSIZE 1024

ScnMgr* g_ScnMgr = NULL;

std::list<int> player1Event;
std::list<int> player2Event;

HANDLE recvsendHandle[2];
HANDLE gameLogicHandle;

HANDLE wait_Recv[2];
HANDLE wait_Send[2];

DWORD g_PrevTime = 0;
DWORD g_ShootStartTime = 0;
DWORD ShootElapsedTime = 0;
float eTime;

int player1ID;
int player2ID;



// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

// ����� ���� ������ ���� �Լ�
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

//�۽� ������
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
		player_num = HERO_ID;
	}
	else if (GetCurrentThreadId() == player2ID) {
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
		player_num = HERO_ID2;
	}

	while (1) {
		//data �ޱ�
		retval = recvn(client_sock, (char *)& len, sizeof(int), 0);
		retval = recvn(client_sock, buf, len, 0);

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
		WaitForSingleObject(wait_Send[player_num], INFINITE);

		int read_data = 0;
		SendData sData;
		float temp;
		for(int i=0; i< MAX_OBJECTS; ++i) {
			if (g_ScnMgr->m_Objects[i] != NULL) {
				g_ScnMgr->m_Objects[i]->GetPos(&sData.posX, &sData.posY, &temp);
				g_ScnMgr->m_Objects[i]->GetKind(&sData.type);
				g_ScnMgr->m_Objects[i]->GetHP(&sData.hp);
				sData.idx_num = i;

				memcpy(buf + read_data, (void*)&sData, sizeof(SendData));
				read_data += sizeof(SendData);
				//�׽�Ʈ�� �ڵ�
				//SendData temp;
				//memcpy((void*)& temp, buf + read_data - sizeof(SendData), sizeof(SendData));
				//printf("%f %f, %d, %d, %d \n", temp.posX, temp.posY, temp.idx_num, temp.type, temp.hp);
			}
		}
		ZeroMemory(buf, sizeof(buf));

		retval = send(client_sock, (char*)& read_data, sizeof(int), 0);
		retval = send(client_sock, buf, len, 0);

	}
	return 0;
}

DWORD WINAPI GameLogicThread(LPVOID arg)
{
	g_PrevTime = GetTickCount();
	g_ScnMgr = new ScnMgr();

	//�������Ź� �浹üũ
	while (1) {
		//Recv�� ������ ����
		WaitForMultipleObjects(2, wait_Recv, TRUE, INFINITE);
		//WaitForSingleObject(wait_Recv[0], INFINITE);

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
		int bulletID = SHOOT_NONE;

		//WASD���ʿ��������Ʒ�
		if (player1Event.size() != 0) {
			forceY1 += amount * player1Event.front();
			player1Event.pop_front();
			forceX1 -= amount * player1Event.front();
			player1Event.pop_front();
			forceY1 -= amount * player1Event.front();
			player1Event.pop_front();
			forceX1 += amount * player1Event.front();
			player1Event.pop_front();
			g_ScnMgr->ApplyForce(forceX1, forceY1, forceZ, HERO_ID, eTime);

			printf("player1 : %d, %d \n", forceX1, forceY1);

			//�Ѿ�Ű��
			for (int i = 0; i < 4; i++) {
				if (player1Event.front() == TRUE)
					bulletID = i + 1;
				player1Event.pop_front();
			}
			if (ShootElapsedTime % 50 == 0) { // Shoot
				g_ScnMgr->Shoot(HERO_ID, bulletID);
			}
		}
		if (player2Event.size() != 0) {
			forceY2 += amount * player2Event.front();
			player2Event.pop_front();
			forceX2 -= amount * player2Event.front();
			player2Event.pop_front();
			forceY2 -= amount * player2Event.front();
			player2Event.pop_front();
			forceX2 += amount * player2Event.front();
			player2Event.pop_front();
			g_ScnMgr->ApplyForce(forceX2, forceY2, forceZ, HERO_ID2, eTime);

			printf("player2 : %d, %d \n", forceX2, forceY2);

			//�Ѿ�Ű��
			for (int i = 0; i < 4; i++) {
				if (player2Event.front() == TRUE)
					bulletID = i + 1;
				player2Event.pop_front();
			}
			if (ShootElapsedTime % 50 == 0) { // Shoot
				g_ScnMgr->Shoot(HERO_ID2, bulletID);
			}
		}


		g_ScnMgr->GarbageCollector();   // ȭ�� ������ ������ ������Ʈ ����
		g_ScnMgr->DoCollisionTest();
		g_ScnMgr->Update(eTime);

		//����� Send ����
		SetEvent(wait_Send[0]);
		SetEvent(wait_Send[1]);
	}

	return 0;
}



int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	gameLogicHandle = CreateThread(NULL, 0, GameLogicThread, NULL, 0, NULL);

	wait_Recv[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	wait_Recv[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

	wait_Send[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	wait_Send[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

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

	// ���� ����
	WSACleanup();
	return 0;
}
