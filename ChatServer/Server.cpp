#include "stdafx.h"
#include "Server.h"
#include "time.h"
#include <thread>
#include <fstream>

static char client1[20]; //Globals
static char client2[20];

Server::Server()
{
}

void Server::setUp()
{
	long SUCCESSFUL; //Declaring essentials
	WSAData WinSockData;
	WORD DLLVERSION;

	DLLVERSION = MAKEWORD(2, 1);

	SUCCESSFUL = WSAStartup(DLLVERSION, &WinSockData);

	SOCKADDR_IN ADDRESS;
	int(AddressSize) = sizeof(ADDRESS);

	ADDRESS.sin_addr.s_addr = inet_addr("127.0.0.1");
	ADDRESS.sin_family = AF_INET;
	ADDRESS.sin_port = htons(444);

	SOCKET listenSocket; //Listeining socket and client connection sockets
	SOCKET connection1;
	SOCKET connection2;

	listenSocket = socket(AF_INET, SOCK_STREAM, NULL);
	bind(listenSocket, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS));
	listen(listenSocket, 2); //Listeing to 2 connections



	for (;;)
	{
		printf("\n\tWaiting for both clients to connect.. ");
		if (connection1 = accept(listenSocket, (SOCKADDR*)&ADDRESS, &AddressSize)); //Waits for first client to connect and recieves
		{
			SUCCESSFUL = recv(connection1, client1, sizeof(client1), NULL);
			std::string tempClient = client1;

			printf("\nFirst client named %s %s", client1, "is present, waiting for client two, to connect...");

			for (;;)
			{
				if (connection2 = accept(listenSocket, (SOCKADDR*)&ADDRESS, &AddressSize)); //Waits for second client
				{
					SUCCESSFUL = recv(connection2, client2, sizeof(client2), NULL); //Recieves and saves name of the second client
					std::string tempClient = client2;

					printf("\nSecond client named %s %s", client2, "is present, continuing... ");

					SUCCESSFUL = send(connection1, client2, 20, NULL); //Sends names to clients
					SUCCESSFUL = send(connection2, client1, 20, NULL);

					
					std::thread transfer1(&Server::msgTransfer, connection1, connection2, client1, client2); //Starts main message exchange function
					std::thread transfer2(&Server::msgTransfer, connection2, connection1, client2, client1);
					
					transfer1.join();
					transfer2.join();
				}
				break;
			}
			break;
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
