#include "stdafx.h"
#include "Connection.h"
#include <thread>
#include <Windows.h>

std::string otherClient = "NULL";
std::string clientName;

Connection::Connection()
{
	printf("\n\tSpecify your name to be used in chat: "); //Establishes client's name on startup and launches server
	std::getline(std::cin, clientName);
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
	SUCCESSFUL = send(sock, clientName.c_str(), 20, NULL);
	char answer;
	std::cout << "wanna chat? y/n: ";
	std::cin >> answer;
	if (answer == 'y')
	{
		printf("\n");
		SUCCESSFUL = recv(sock, otherChar, sizeof(otherChar), NULL);
		otherClient = otherChar;

		std::thread reciever(&Connection::messageRecv, sock); //Starts recievung messages
		std::thread sender(&Connection::messageSend, sock); //Starts sending messages

		reciever.join();
		sender.join();
	}
	else
	{
		printf("\nOkay, exiting app... ");
	}
	

	std::getchar();
}

void Connection::messageRecv(SOCKET &Ser) //Message reciever
{
	for (;;)
	{
		long RESULT;
		std::string message;
		char charMessage[100];

		RESULT = recv(Ser, charMessage, sizeof(charMessage), NULL); //Recieves a message from server
		for (int i = 0, g = 0; i < 20; i++) //Starts decrypting a message from server, using client's name as Key
		{
			if (charMessage[g] == NULL)
			{
				break;
			}
			if (clientName[i] == NULL)
			{
				i = 0;
			}
			charMessage[g] = charMessage[g] - clientName[i];
			g++;
		}
		message = charMessage;

		std::cout << std::endl;
		std::cout << otherClient << ": " << message << std::endl; //Prints out othr client's name and messagee
	}
}

void Connection::messageSend(SOCKET &Ser)//Message sender
{
	for (;;)
	{
		long RESULT;
		std::string message;
		char charMessage[100];
		std::cin.getline(charMessage, 100); //Gets client message
		//std::getline(std::cin, message);

		for (int i = 0, g = 0; i < 20; i++) //Encrypts client's messsage, using client's name as Key
		{
			if (charMessage[g] == NULL)
			{
				break;
			}
			if (clientName[i] == NULL)
			{
				i = 0;
			}
			charMessage[g] = charMessage[g] + clientName[i];
			g++;
		}

		RESULT = send(Ser, charMessage, 100, NULL);
		//RESULT = send(Ser, message.c_str(), 100, NULL);
	}
}

Connection::~Connection()
{
}