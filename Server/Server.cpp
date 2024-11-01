#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main(void)
{
    const char IP_SERV[] = "127.0.0.1";
    const int PORT_NUM = 1234;
    const short BUFF_SIZE = 1024;
    int erStat;
    in_addr ip_to_num;
    erStat = inet_pton(AF_INET, IP_SERV, &ip_to_num);
    if (erStat <= 0)
    {
        cout << "Error in IP translation to special numeric format" << endl;
        return 1;
    }
    WSADATA wsData;
    erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (erStat != 0)
    {
        cout << "Error WinSock version initialization #";
        cout << WSAGetLastError();
        return 1;
    }
    else
        cout << "WinSock is initialized" << endl;
    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ServSock == INVALID_SOCKET)
    {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Server socket is initialized" << endl;
    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));
    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(PORT_NUM);
    erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
    if (erStat != 0)
    {
        cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Socket binding to server infornation completed" << endl;
    erStat = listen(ServSock, SOMAXCONN);
    if (erStat != 0)
    {
        cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Listening" << endl;
    sockaddr_in clientInfo;
    ZeroMemory(&clientInfo, sizeof(clientInfo));
    int clientInfo_size = sizeof(clientInfo);
    SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);
    if (ClientConn == INVALID_SOCKET)
    {
        cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
    {
        cout << "Connection to a client established" << endl;
        char clientIP[22];
        inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);
        cout << "Client connected with IP address " << clientIP << endl;
    }

    vector<char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);
    short packet_size = 0;
    while (true)
    {
        packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
        cout << "Client equation coefficients: " << servBuff.data() << endl;
        cout << "Server answer: ";

        string coef(servBuff.data());
        size_t end = coef.find(' ');
        float a = atof(coef.substr(0, end).c_str());
        coef = coef.substr(end + 1, coef.length() - 1);
        end = coef.find(' ');
        float b = atof(coef.substr(0, end).c_str());
        coef = coef.substr(end + 1, coef.length() - 1);
        float c = atof(coef.c_str());

        float x = 0;
        if (a != 0)
            x = (c - b) / a;
        else
        {
            cout << "Can't divide by 0." << endl;
            closesocket(ServSock);
            closesocket(ClientConn);
            WSACleanup();
            return 1;
        }

        string answer = "x is equal " + to_string(x);
        cout << answer << endl;

        if (clientBuff[0] == 'e' && clientBuff[1] == 'x' && clientBuff[2] == 'i' && clientBuff[3] == 't')
        {
            shutdown(ClientConn, SD_BOTH);
            closesocket(ServSock);
            closesocket(ClientConn);
            WSACleanup();
            return 0;
        }
        packet_size = send(ClientConn, answer.c_str(), answer.size(), 0);
        if (packet_size == SOCKET_ERROR)
        {
            cout << "Can't send answer to client. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            closesocket(ClientConn);
            WSACleanup();
            return 1;
        }
    }
    closesocket(ServSock);
    closesocket(ClientConn);
    WSACleanup();
    return 0;
}