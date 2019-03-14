//Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"ws_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>

SOCKET Connection;

enum Packet //Typ wiadomosci
{
	P_ChatMessage,
	P_Test
	

};

bool SendInt(int _int)
{
	int RetnCheck = send(Connection, (char*)&_int, sizeof(int), NULL); // przesyla _int
	if (RetnCheck == SOCKET_ERROR) // jesli zawiedziedzie int z przesylaniem poprzez problem z polaczeniem wtedy wraca : F lub T 
		return false;
	return true;
}

bool GetInt(int &_int)
{
	int RetnCheck = recv(Connection, (char*)&_int, sizeof(int), NULL); // dostaje int
	if (RetnCheck == SOCKET_ERROR) // problem z polaczeniem
		return false;
	return true;
}
bool SendPacketType(Packet _packettype)
{
	int RetnCheck = send(Connection, (char*)&_packettype, sizeof(Packet), NULL); //przesyla pakiet
	if (RetnCheck == SOCKET_ERROR) //problem z przeslaniem pakietu
		return false;
	return true;
}
bool GetPacketType(Packet & _packettype)
{
	int RetnCheck = recv(Connection, (char*)&_packettype, sizeof(Packet), NULL); //odbiera pakiet
	if (RetnCheck == SOCKET_ERROR) //problem z przeslaniem pakietu
		return false;
	return true;
}
bool SendString(std::string & _string)
{
	if (!SendPacketType(P_ChatMessage))
		return false;
	int bufferlength = _string.size(); //znajdluje dlugosc buffera
	if (!SendInt(bufferlength))
		return false;
	int RetnCheck = send(Connection, _string.c_str(), bufferlength, NULL);// przesyla string buffer
	if (RetnCheck == SOCKET_ERROR)// blad w przesylaniu buffera
		return false;
	return true;
 }

bool GetString(std::string &_string)
{
	int bufferlength;
	if (!GetInt(bufferlength))
		return false;
	char * buffer = new char[bufferlength + 1]; //przydziela buffer
	buffer[bufferlength] = '\0';
	int RetnCheck = recv(Connection, buffer, bufferlength, NULL);
	_string = buffer;
	delete[] buffer; //zwalnia buffer
	if (RetnCheck == SOCKET_ERROR) //problem z przeslaniem pakietu
		return false;
	return true;
}

bool ProcessPacket(Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		std::string Message;
		if (!GetString(Message))
			return false;
		std::cout << Message << std::endl;
		break;
	}
	
	default: // nierozpoznany pakiet
		std::cout << "Unrecognized packet: " << packettype << std::endl;
		break;
	}
	return true;
}

void ClientThread()
{
	Packet packettype;
	while (true)
	{
		if (!GetPacketType(packettype)) //pobierz pakiet
			break;
			if (!ProcessPacket(packettype))//jesli  pakiet nie przejdzie prawidlowo
				break; //wyjdz z kilenta		
	}
		std::cout << "Lost connection to the server." << std::endl;
		closesocket(Connection);
}
	

int main() 
{
	//Winscok uruchomienie 
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) // Jeœli WSAStartup wróci cokolwiek ni¿ 0, oznacza to ze tego tu nie ma tu 
	{
		MessageBoxA(NULL, "Winsock start up failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	SOCKADDR_IN addr; //Adres bd wi¹za³ nasze gniazdo
	int sizeofaddr = sizeof(addr); //d³ugoœæ adresu
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //transmisja lokalna
	addr.sin_port = htons(1111); //Port
	addr.sin_family = AF_INET; //IPv4 gniazdo

	 Connection = socket(AF_INET, SOCK_STREAM, NULL); //ustawienie polaczenia
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //jesli nie mozemy sie polaczyc
	{
		MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return 0; //bledne polaczenie
	}
	std::cout << "Connected!" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);


	std::string userinput;
	while (true)
	{
		std::getline(std::cin, userinput); // pobiera linie jesli uzytkownik nacisnal enter
		if (!SendString(userinput))
			break;
		Sleep(10);
	}
	system("pause");
	return 0;
}