#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <sdkddkver.h>
#include <conio.h>
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <thread>

#define SCK_VERSION2 0x0202

class Server
{
private:
	static void msgTransfer(SOCKET &Sender, SOCKET &Reciever, char cliName1[20], char cliName2[20]);
	static void clientGet(SOCKET &Lis);
	static void chatRoom();
public:
	Server();
	void setUp();
	~Server();
};

