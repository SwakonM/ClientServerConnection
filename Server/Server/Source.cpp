#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
bool ProcessPacket(int ID, Packet packettype)
{
	switch (packettype)
	{
		case P_ChatMessage:
		{
			int bufferlength;
			int result = recv(Connections[ID], (char*)&bufferlength, sizeof(int), NULL); // pobiera dlugosc buffora
			if (result > 0)
			{
				char * buffer = new char[bufferlength]; //rozdziela pameic
				recv(Connections[ID], buffer, bufferlength, NULL); //odbiera wiadmosci od klienta
				for (int i = 0; i < TotalConnections; i++) // dla kazdego klienta
				{
					if (i == ID)
					continue; // pomija uzytkowniak
					Packet chatmessagepacket = P_ChatMessage;
					send(Connections[i], (char *)&chatmessagepacket, sizeof(Packet), NULL);
					send(Connections[i], (char *)&bufferlength, sizeof(int), NULL); //wysyla dlugosc buffera do clienta
					send(Connections[i], buffer, bufferlength, NULL); // przesyla wiadomosc
				}
				delete[] buffer;
			}
		
		}
		break;
	default:
		std::cout << "Unrecognized packet: " << packettype << std::endl;
		break;
	}
	return true;
}

void ClientHandlerThread(int ID)
{
	
	Packet packettype;
	while (true)
	{
		recv(Connections[ID], (char*)&packettype, sizeof(Packet), NULL);
		
		if (!ProcessPacket(ID, packettype))
			break;
	}
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
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL);
			TotalConnections = +1; //Inkrementuje wszystki liczbe polaczonych klientow
			Packet chatmessagepacket = P_ChatMessage; //tworzy typ pakirtu
			send(Connections[i], (char*)&chatmessagepacket, sizeof(Packet), NULL);// wysyla typ pakietu
			std::string buftest = "MOTD: Witamy ! Have a nice day";
			int size = buftest.size();
			send(Connections[i], (char*)&size, sizeof(int), NULL);
			send(Connections[i], buftest.c_str(),buftest.size(), NULL);
			
			Packet testpacket = P_Test;
			send(Connections[i], (char*)&testpacket, sizeof(Packet), NULL);
			
			
		}
	}
	system("pause");
	return 0;
}