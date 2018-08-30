#include "stdafx.h"
#include "Server.h"
#include "time.h"
#include <thread>
#include <fstream>
#include <vector>

static char client1[20]; //Globals
static char client2[20];
std::vector<SOCKET> Clients;
std::vector<std::string> clientNames;
SOCKADDR_IN ADDRESS;
int(AddressSize) = sizeof(ADDRESS);
int unassignedCleints = 0;
int clientCounter = 0;
std::vector<std::thread> threads;

long SUCCESSFUL; //Declaring essentials
WSAData WinSockData;
WORD DLLVERSION;

Server::Server()
{
}

void Server::setUp()
{
	DLLVERSION = MAKEWORD(2, 1);

	SUCCESSFUL = WSAStartup(DLLVERSION, &WinSockData);

	ADDRESS.sin_addr.s_addr = inet_addr("127.0.0.1");
	ADDRESS.sin_family = AF_INET;
	ADDRESS.sin_port = htons(444);

	SOCKET listenSocket; //Listeining socket and client connection sockets

	listenSocket = socket(AF_INET, SOCK_STREAM, NULL);
	bind(listenSocket, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS));
	listen(listenSocket, SOMAXCONN); //Listeing socket

	std::thread clientCollector(&Server::clientGet, listenSocket);

	
	Server::chatRoom();	
}

void Server::chatRoom()
{
	while (true)
	{
		if (unassignedCleints == 2)
		{
			int First = clientCounter - 2;
			int Second = clientCounter - 1;
			char firstName[20];
			char secondName[20];
			strcpy_s(firstName, sizeof(firstName), clientNames[First].c_str());
			strcpy_s(secondName, sizeof(secondName), clientNames[Second].c_str());
			SUCCESSFUL = send(Clients[First], secondName, 20, NULL);
			SUCCESSFUL = send(Clients[Second], firstName, 20, NULL);

			threads.push_back(std::thread(&Server::msgTransfer, Clients[First], Clients[Second], firstName, secondName));
			threads.push_back(std::thread(&Server::msgTransfer, Clients[Second], Clients[First], secondName, firstName));
			unassignedCleints = 0;
		}
	}
}

void Server::clientGet(SOCKET &Lis)
{
	while (true)
	{
		SOCKET TempClient;
		char TempName[20];
		if (TempClient = accept(Lis, (SOCKADDR*)&ADDRESS, &AddressSize))
		{
			Clients.push_back(TempClient);
			SUCCESSFUL = recv(Clients[clientCounter], TempName, sizeof(client1), NULL);
			clientNames.push_back(TempName);

			unassignedCleints++;
			clientCounter++;
			Sleep(500);
		}
	}
}

void Server::msgTransfer(SOCKET &Sender, SOCKET &Reciever, char cliName1[20], char cliName2[20])
{
	long RESULT;
	char charMessage[100];

	for (;;)
	{
		RESULT = recv(Sender, charMessage, sizeof(charMessage), NULL); //Recieves sent data

		for (int i = 0, g = 0; i < 20; i++) //Starts to decrypt sent data using sender's name as key
		{
			if (charMessage[g] == NULL)
			{
				break;
			}
			if (cliName1[i] == NULL)
			{
				i = 0;
			}
			charMessage[g] = charMessage[g] - cliName1[i];
			g++;
		}

		std::ofstream file;
		file.open("chatlog.txt", std::ios_base::app);
		file << cliName1 << ": " << charMessage << '\n'; //Saves any data sent to a text file aka chatlog

		for (int i = 0, g = 0; i < 20; i++) //Encrypts message after sending using recever's name
		{
			if (charMessage[g] == NULL)
			{
				break;
			}
			if (cliName2[i] == NULL)
			{
				i = 0;
			}
			charMessage[g] = charMessage[g] + cliName2[i];
			g++;
		}

		RESULT = send(Reciever, charMessage, sizeof(charMessage), NULL);
	}
}

Server::~Server()
{
}
