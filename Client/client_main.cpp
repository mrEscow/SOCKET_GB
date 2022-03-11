#include <iostream>
#include <stdio.h>

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

    sockaddr_in ServerAddr;
    int  err, maxlen = 512;

    char* recvbuf = new char[maxlen];
    char* query = new char[maxlen];

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SendRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    unsigned int a = 127;
    unsigned int b = 0;
    unsigned int c = 0;
    unsigned int d = 1;
    unsigned int destination_address = (a << 24) | (b << 16) | (c << 8) | d;

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = htonl(destination_address);
    ServerAddr.sin_port = htons(12345);


    while (true)
    {
        std::cin >> query;
        int size = sendto(SendRecvSocket, query, strlen(query), 0, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
        std::cout << size << std::endl;
        err = recvfrom(SendRecvSocket, recvbuf, maxlen, 0, 0, 0);
        if (err > 0) {
            recvbuf[err] = '\0';
            std::cout << recvbuf << " \n";
        }
        else {
            closesocket(SendRecvSocket);
            WSACleanup();
            return 1;
        }
    }

    closesocket(SendRecvSocket);
    return 0;
}