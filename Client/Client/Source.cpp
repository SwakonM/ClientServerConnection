//Client
#pragma comment(lib,"ws_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>

SOCKET Connection;

void ClientThread()
{
	char buffer[256];
	while (true)
		recv(Connection, buffer, sizeof(buffer), NULL);
	std::cout << buffer << std::endl;
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

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL); //ustawienie polaczenia
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //jesli nie mozemy sie polaczyc
	{
		MessageBoxA(NULL, "Failed to Coonnect", "Error", MB_OK | MB_ICONERROR);
		return 0; //bledne polaczenie
	}
	std::cout << "Connected!" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);


	std::string buffer;
	while (true)
	{
		std::getline(std::cin, buffer); // pobiera linie jesli uzytkownik nacisnal enter
		int bufferlenght = buffer.size();
		send(Connection, (char*)&bufferlenght, sizeof(int), NULL); //wysyla wielkosc buffera
		send(Connection, buffer.c_str(), bufferlenght, NULL);  //wysyla wiadomsoc
		Sleep(10);
	}
	system("pause");
	return 0;
}