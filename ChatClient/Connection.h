#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <sdkddkver.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>

#define SCK_VERSION2 0x0202

class Connection
{
private:
	static void messageRecv(SOCKET &Ser);
	static void messageSend(SOCKET &Ser);
	static void msgDecrypt(char name[20], char msg[100]);
	static void msgEncrypt(char name[20], char msg[100]);
public:
	Connection();
	void connectionSetup();
	~Connection();
};

