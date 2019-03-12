#pragma comment(lib,"ws_32.lib")

#include <WinSock2.h>
#include <iostream>


SOCKET Connections[100];
int ConnectionCounter = 0;

void ClientHandlerThread(int index)
{
	char buffer[256];
	while (true)
	{
		recv(Connections[index], buffer, sizeof(buffer), NULL);
	 	std::cout << buffer << std::endl;
		for (int i = 0; i < ConnectionCounter; i++)
		{
			if (i == index)
				continue;
			send(Connections[i], buffer, sizeof(buffer), NULL);
		}
	}
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
			char MOTD[256] = "Welcome ! This is the Message of the Day";
			send(newConnection, MOTD, sizeof(MOTD), NULL);
			Connections[i] = newConnection;
			ConnectionCounter = + 1;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ClientHandlerThread, (LPVOID)(i), NULL, NULL);
		}
	}
	system("pause");
	return 0;
}