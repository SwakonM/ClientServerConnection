#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"ws_32.lib")

#include <WinSock2.h>
#include <iostream>


SOCKET Connections[100];
int TotalConnections = 0;
enum Packet //Typ wiadomosci
{
	P_ChatMessage,
	P_Test

};
bool SendInt(int ID, int _int)
{
	int RetnCheck = send(Connections[ID], (char*)&_int, sizeof(int), NULL); // przesyla _int
	if (RetnCheck == SOCKET_ERROR) // jesli zawiedziedzie int z przesylaniem poprzez problem z polaczeniem wtedy wraca : F lub T 
		return false;
	return true;
}
bool GetInt(int ID, int &_int)
{
	int RetnCheck = recv(Connections[ID], (char*)&_int, sizeof(int), NULL); // dostaje int
	if (RetnCheck == SOCKET_ERROR) // problem z polaczeniem
		return false;
	return true;
}
bool SendPacketType(int ID, Packet _packettype)
{
	int RetnCheck = send(Connections[ID], (char*)&_packettype, sizeof(Packet), NULL); //przesyla pakiet
	if (RetnCheck == SOCKET_ERROR) //problem z przeslaniem pakietu
		return false;
	return true;
}
bool GetPacketType(int ID, Packet & _packettype)
{
	int RetnCheck = recv(Connections[ID], (char*)&_packettype, sizeof(Packet), NULL); //odbiera pakiet
	if (RetnCheck == SOCKET_ERROR) //problem z przeslaniem pakietu
		return false;
	return true;
}
bool SendString(int ID, std::string & _string)
{
	if (!SendPacketType(ID, P_ChatMessage))
		return false;
	int bufferlength = _string.size(); //znajdluje dlugosc buffera
	if (!SendInt(ID, bufferlength))
		return false;
	int RetnCheck = send(Connections[ID], _string.c_str(), bufferlength, NULL);// przesyla string buffer
	if (RetnCheck == SOCKET_ERROR)// blad w przesylaniu buffera
		return false;
	return true;
}
bool GetString(int ID, std::string &_string)
{
	int bufferlength;
	if (!GetInt(ID, bufferlength))
		return false;
	char * buffer = new char[bufferlength + 1]; //przydziela buffer
	buffer[bufferlength] = '\0';
	int RetnCheck = recv(Connections[ID], buffer, bufferlength, NULL);
	_string = buffer;
	delete[] buffer; //zwalnia buffer
	if (RetnCheck == SOCKET_ERROR) //problem z przeslaniem pakietu
		return false;
	return true;
}

bool ProcessPacket(int ID, Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		std::string Message;
		if (!GetString(ID, Message))
			return false;

		for (int i = 0; i < TotalConnections; i++)
		{
			if (i == ID)
				continue;
			if (!SendString(i, Message))
			{
				std::cout << "Failed to send message from client ID: " << ID << " to cliend ID: " << i << std::endl;
			}
		}

		std::cout << "Proccesed chat message packet form user ID: " << ID << std::endl;
		break;

	}
	default:
		std::cout << "Unrecognized packet: " << packettype << std::endl;
		break;
	}
	return true;
}

void ClientHandlerThread(int ID)
{

	Packet PacketType;
	while (true)
	{
		if (!GetPacketType(ID, PacketType)) //pobierz pakiet
			break;
		if (!ProcessPacket(ID, PacketType))//jesli  pakiet nie przejdzie prawidlowo
			break; //wyjdz z kilenta		
	}
	std::cout << "Lost connection to the server." << std::endl;
	closesocket(Connections[ID]);
}

int main()
{
	//Winscok uruchomienie 
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //Jeœli WSAStartup wróci cokolwiek ni¿ 0, oznacza to ze tego tu nie ma tu 
	{
		MessageBoxA(NULL, "Winsock start up failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	SOCKADDR_IN addr; //Adres bd wi¹za³ nasze gniazdo
	int addrlen = sizeof(addr); //d³ugoœæ adresu
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //transmisja lokalna
	addr.sin_port = htons(1111); //Port
	addr.sin_family = AF_INET; //IPv4 gniazdo

	SOCKET  sListen = socket(AF_INET, SOCK_STREAM, NULL); //Stworzenie gniazda dla nowych polaczen
	bind(sListen, (SOCKADDR*)& addr, sizeof(addr)); //wi¹¿ê adres z gniazdem
	listen(sListen, SOMAXCONN); //nas³uchuje pochodzenia przychodz¹ce

	SOCKET newConnection;
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //akceptuje nowe po³¹czenia
		if (newConnection == 0) //bledne polaczenie
		{
			std::cout << "Failed to accept the client's connection." << std::endl;
		}
		else // poprawne polaczenie
		{
			std::cout << "Client Connected" << std::endl;
			Connections[i] = newConnection;
			TotalConnections = +1; //Inkrementuje wszystki liczbe polaczonych klientow
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL);
			std::string MOTD = "MOTD: Witamy ! It's daily message.";
			SendString(i, MOTD);
		}
	}
	system("pause");
	return 0;
}