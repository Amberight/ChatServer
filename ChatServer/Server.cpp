#include "stdafx.h"
#include "Server.h"
#include "time.h"
#include <thread>
#include <fstream>
#include <vector>

static char client1[20]; //Essential globals
static char client2[20];
std::vector<SOCKET> Clients;
std::vector<std::string> clientNames;
SOCKADDR_IN ADDRESS;
int(AddressSize) = sizeof(ADDRESS);
int unassignedCleints = 0;
int clientCounter = 0;
std::vector<std::thread> threads;

long SUCCESSFUL;
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

	SOCKET listenSocket; //Listeining socket
	listenSocket = socket(AF_INET, SOCK_STREAM, NULL);
	bind(listenSocket, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS));
	listen(listenSocket, SOMAXCONN); //Listeing socket is opem to maximum connections

	std::thread clientCollector(&Server::clientGet, listenSocket); //Starts a thread with clientGet function on listeing socket


	Server::chatGroup();
	//Server::chatSession();
}

void Server::clientGet(SOCKET &Lis) //Takes a listening socket 
{
	while (true)
	{
		SOCKET TempClient;
		char TempName[20];
		if (TempClient = accept(Lis, (SOCKADDR*)&ADDRESS, &AddressSize)) //Accepts an incoming connection from "Lis" socket to a "TempClient" socket
		{
			Clients.push_back(TempClient); //Adding "TempClient" socket to a "Clients" vector array
			SUCCESSFUL = recv(Clients[clientCounter], TempName, 20, NULL); 
			clientNames.push_back(TempName); //Adding recieved client name to "clientNames" vector array 

			unassignedCleints++;
			clientCounter++;
			Sleep(500);
		}
	}
}

void Server::chatGroup()
{
	int counter = 0;
	while (true)
	{
		if (clientCounter > counter)
		{
			threads.push_back(std::thread(&Server::msgMultiTransfer, Clients[counter], clientNames[counter]));
			counter++;
		}
	}
}

void Server::msgMultiTransfer(SOCKET &Sender, std::string recvName) // Takes Client socket and string name as parameters to send data to all other connected clients
{
	long RESULT;
	char charMessage[100];
	char tempRecName[20];
	strcpy_s(tempRecName, sizeof(tempRecName), recvName.c_str());
	while (true)
	{
		RESULT = recv(Sender, charMessage, sizeof(charMessage), NULL); //Recieves sent data from connected socket "Sender" to a "charMessage" array
		if (charMessage[0] != NULL)
		{
			Server::msgDecrypt(tempRecName, charMessage);

			for (int p = 0; p < clientCounter; p++)
			{
				if (recvName == clientNames[p])
				{

				}
				else
				{
					Server::msgSend(Clients[p], clientNames[p], charMessage);
				}
			}
		}
	}
}

void Server::msgSend(SOCKET &Reciever, std::string recvName, char msg[100]) //Recieves client socket from "Clients" array, client name and Message to be sent
{
	long RESULT; 
	char tempName[20];
	char tempMsg[130];
	strcpy_s(tempName, (sizeof(tempName) / sizeof(tempName[0])), recvName.c_str());
	strcpy_s(tempMsg, (sizeof(tempMsg) / sizeof(tempMsg[0])), recvName.c_str());
	strcat_s(tempMsg, " says: ");
	strcat_s(tempMsg, msg);

	Server::msgEncrypt(tempName, tempMsg);

	RESULT = send(Reciever, tempMsg, 100, NULL);
}

void Server::msgEncrypt(char name[20], char msg[100])
{
	for (int i = 0, g = 0; i < 20; i++) //Encrypts message array using client name as key by adding consecutive chars from "recvName" to "msg" chars
	{
		if (msg[g] == NULL)
		{
			break;
		}
		if (name[i] == NULL)
		{
			i = 0;
		}
		msg[g] = msg[g] + name[i];
		g++;
	}
}

void Server::msgDecrypt(char name[20], char msg[100])
{
	for (int i = 0, g = 0; i < 20; i++) //Decrypts message array using client name as key by subtracting consecutive "tempName" chars from "charMessage"
	{
		if (msg[g] == NULL)
		{
			break;
		}
		if (name[i] == NULL)
		{
			i = 0;
		}
		msg[g] = msg[g] - name[i];
		g++;
	}
}

void Server::msgTransfer(SOCKET &Sender, SOCKET &Reciever, char cliName1[20], char cliName2[20]) // Takes 2 sockets as reciever and sender from "Clients" and takes 2 client names
{
	long RESULT;
	char charMessage[100];

	while (true)
	{
		RESULT = recv(Sender, charMessage, sizeof(charMessage), NULL); //Recieves sent data

		for (int i = 0, g = 0; i < 20; i++) //Decrypts "charMessage" using char array "cliName1" as a key
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

void Server::chatSession()
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
			//SUCCESSFUL = send(Clients[First], secondName, 20, NULL);
			//SUCCESSFUL = send(Clients[Second], firstName, 20, NULL);

			threads.push_back(std::thread(&Server::msgTransfer, Clients[First], Clients[Second], firstName, secondName));
			threads.push_back(std::thread(&Server::msgTransfer, Clients[Second], Clients[First], secondName, firstName));
			unassignedCleints = 0;
		}
	}
}

Server::~Server()
{
}
