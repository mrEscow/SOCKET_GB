#include <iostream>
#include <stdio.h>
#include <string>

#ifdef _WIN32
#   include <process.h>
#   include <WinSock2.h>
#   include <WS2tcpip.h>
#   ifndef getpid
#       define getpid _getpid
#   endif
#else
#   include <unistd.h>
#endif


int main(int argc, const char* argv[])
{
    //const std::string host_name = { argv[1] };

    WSADATA wsaData;
    SOCKET SendRecvSocket;

    sockaddr_in ServerAddr, ClientAddr;
    int  err, maxlen = 512, ClientAddrSize = sizeof(ClientAddr);

    char* recvbuf = new char[maxlen];
    //char* result_string = new char[maxlen];
    char* name = new char[maxlen];

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SendRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    ServerAddr.sin_port = htons(12345);
    err = bind(SendRecvSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));


    if (err == SOCKET_ERROR) {
        std::cout << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(SendRecvSocket);
        WSACleanup();
        return -1;
    }

    while (true)
    {
        err = recvfrom(SendRecvSocket, recvbuf, maxlen, 0, (sockaddr*)&ClientAddr, &ClientAddrSize);
        if (err > 0) {

            recvbuf[err] = '\0';
            std::cout << recvbuf << " " << std::endl;

            if (recvbuf[1] == ':') {
                std::cout << "good" << std::endl;
                name =  recvbuf;
                sendto(SendRecvSocket, "NAME:", strlen("NAME:"), 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));
                sendto(SendRecvSocket, name, strlen(name), 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));
            }

            sendto(SendRecvSocket, recvbuf, strlen(recvbuf), 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));

        }
        else
        {
            std::cout << "recv failed: " << WSAGetLastError() << std::endl;
            closesocket(SendRecvSocket);
            WSACleanup();
            return 1;
        }
    }

    return 0;
}