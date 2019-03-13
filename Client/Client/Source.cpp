//Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
bool ProcessPacket(Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		int bufferlength;
		recv(Connection, (char*)&bufferlength, sizeof(int), NULL); // odbiera wiadomsoc o rozmairze
		char * buffer = new char[bufferlength + 1];
		buffer[bufferlength] = '\0';
		recv(Connection, buffer, bufferlength, NULL);
		std::cout << buffer << std::endl; //wypsuje buffer
		delete[] buffer; //uwalnia buffer
		break;
	}
	case P_Test:
		std::cout << "You have recived the test packet" << std::endl;
		break;
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
		int result = recv(Connection, (char*)&packettype, sizeof(Packet), NULL); // odbiera typ packietu
		if (result > 0) 
		{
			if (!ProcessPacket(packettype))//jesli  pakiet nie przejdzie prawidlowo
				break; //wyjdz z kilenta		
		}
	}
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
		int bufferlenght = userinput.size();// znajdz rozmiar buffora
		Packet packettype = P_ChatMessage; //tworzy typ pakirtu
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);// wysyla typ pakietu
		send(Connection, (char*)&bufferlenght, sizeof(int), NULL); //wysyla wielkosc buffera
		send(Connection, userinput.c_str(), bufferlenght, NULL);  //wysyla wiadomosc
		Sleep(10);
	}
	system("pause");
	return 0;
}