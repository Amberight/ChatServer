#include "stdafx.h"
#include "Connection.h"
#include <thread>
#include <Windows.h>

std::string otherClient = "Other client";
char clientName[20];

Connection::Connection()
{
	printf("\n\tSpecify your name to be used in chat: "); //Establishes client's name on startup and launches server
	std::cin.getline(clientName, 20);
	connectionSetup();
}

void Connection::connectionSetup()
{
	long SUCCESSFUL; //Declaring variables
	WSAData WinSockData;
	WORD DLLVERSION;
	char otherChar[20];

	DLLVERSION = MAKEWORD(2, 1);
	SUCCESSFUL = WSAStartup(DLLVERSION, &WinSockData);

	SOCKADDR_IN ADDRESS;
	int(AddressSize) = sizeof(ADDRESS);

	SOCKET sock;

	sock = socket(AF_INET, SOCK_STREAM, NULL);
	ADDRESS.sin_addr.s_addr = inet_addr("127.0.0.1");
	ADDRESS.sin_family = AF_INET;
	ADDRESS.sin_port = htons(444);

	connect(sock, (SOCKADDR*)&ADDRESS, sizeof(ADDRESS)); //Sends client Name to the servers and recieves other client's name.
	SUCCESSFUL = send(sock, clientName, 20, NULL);
	char answer;
	std::cout << "wanna join group chat? y/n: ";
	std::cin >> answer;
	if (answer == 'y')
	{
		printf("\n");
		//SUCCESSFUL = recv(sock, otherChar, sizeof(otherChar), NULL);
		//otherClient = otherChar;

		std::thread reciever(&Connection::messageRecv, sock); //Starts recievung messages
		std::thread sender(&Connection::messageSend, sock); //Starts sending messages

		reciever.join();
		sender.join();
	}
	else
	{
		printf("\nOkay, exiting app... ");
	}
}

void Connection::messageRecv(SOCKET &Ser) // Message reciever, ecieves data on socket "Ser"
{
	for (;;)
	{
		long RESULT;
		char charMessage[100];

		RESULT = recv(Ser, charMessage, 130, NULL); // Assigns recieved data to "charMessage" array
		Connection::msgDecrypt(clientName, charMessage); // Decrypts recieved data using clientName as key
		std::cout << charMessage << std::endl;
		std::cout << std::endl;
	}
}

void Connection::messageSend(SOCKET &Ser) // Message sender, sends data through a "Ser" socket
{
	for (;;)
	{
		long RESULT;
		char charMessage[100];
		std::cin.getline(charMessage, 100); // Gets client message
		std::cout << std::endl;

		Connection::msgEncrypt(clientName, charMessage); // Encrypts client's message "charMessage" using "clientName" as key

		RESULT = send(Ser, charMessage, 100, NULL);
	}
}

void Connection::msgEncrypt(char name[20], char msg[100])
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

void Connection::msgDecrypt(char name[20], char msg[100])
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

Connection::~Connection()
{
}