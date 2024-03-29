#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include "Packet.h"

#pragma comment(lib, "ws2_32")

using namespace std;

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));

	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSockAddr.sin_port = htons(5001);

	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	while (true)
	{
		char Buffer[1024] = { 0, };

		int RecvByte = recv(ServerSocket, Buffer, 4, MSG_WAITALL);
		if (RecvByte <= 0)
			break;

		Header PacketHeader;
		memcpy(&PacketHeader, Buffer, sizeof(Header));

		PacketHeader.PacketType = ntohs(PacketHeader.PacketType);
		PacketHeader.Length = ntohs(PacketHeader.Length);

		if (PacketHeader.PacketType == (unsigned short)(EPacketType::Caculate))
		{
			char* DataBuffer = new char[PacketHeader.Length];
			int RecvByte = recv(ServerSocket, DataBuffer, PacketHeader.Length, MSG_WAITALL);
			if (RecvByte <= 0)
				break;

			Data Packet;
			memcpy(&Packet, DataBuffer, sizeof(Packet));

			long long Result = 0;
			Packet.FirstNumber = ntohl(Packet.FirstNumber);
			Packet.SecondNumber = ntohl(Packet.SecondNumber);

			cout << Packet.FirstNumber << " ";

			switch (Packet.Operator)
			{
			case 0:
				Result = Packet.FirstNumber + Packet.SecondNumber;
				cout << " + ";
				break;
			case 1:
				Result = Packet.FirstNumber - Packet.SecondNumber;
				cout << " - ";
				break;
			case 2:
				Result = Packet.FirstNumber * Packet.SecondNumber;
				cout << " * ";
				break;
			case 3:
				Result = Packet.FirstNumber / Packet.SecondNumber;
				cout << " / ";
				break;
			case 4:
				Result = Packet.FirstNumber % Packet.SecondNumber;
				cout << " % ";
				break;
			default:
				Result = Packet.FirstNumber + Packet.SecondNumber;
				cout << " + ";
				break;
			}
			cout << Packet.SecondNumber << " = ";
			cout << Result << endl;

			char Message[8] = { 0, };
			Result = htonll(Result);
			memcpy(Message, &Result, sizeof(Result));

			send(ServerSocket, Message, (u_int)sizeof(Message), 0);
			delete[] DataBuffer;
		}
	}
	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}