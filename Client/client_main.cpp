#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>

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

#define BUFLEN 512      //Max length of buffer
#define PORT 8888       //The port on which to listen for incoming data



int main(int argc, const char* argv[])
{
    //const std::string host_name = { argv[0] };

    WSADATA wsaData;
    SOCKET SendRecvSocket;


    int  err=0, maxlen = BUFLEN;

    char* recvbuf = new char[maxlen];
    char* query = new char[maxlen];


    WSAStartup(MAKEWORD(2, 2), &wsaData);

    char* host_name = new char[maxlen];
    std::cout << gethostname(host_name, 0) << std::endl;
    std::cout << strlen(host_name) << std::endl;
    std::cout << WSAGetLastError() << std::endl;

    SendRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    //unsigned char a = 127;
    //unsigned char b = 0;
    //unsigned char c = 0;
    //unsigned char d = 1;

    unsigned char a = 192;
    unsigned char b = 168;
    unsigned char c = 1;
    unsigned char d = 175;

    unsigned int destination_address = (a << 24) | (b << 16) | (c << 8) | d;

    sockaddr_in ServerAddr;
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = htonl(destination_address);
    ServerAddr.sin_port = htons(PORT);
    int ServerSize = sizeof(ServerAddr);

    bool is_new_message{ false };

    std::thread ForCin([&query,&is_new_message]() {
            while (true)
            {
                std::cin >> query;
                is_new_message = true;
            }
        }
    );



    std::thread ForRecvfrom([&]() {
            while (true)
            {

                err = recvfrom(
                    
                    SendRecvSocket, 

                    recvbuf, 
                    maxlen, 

                    0, 

                    (sockaddr*)&ServerAddr,
                    &ServerSize

                );

                if (err > 0) {
                    recvbuf[err] = '\0';
                    std::cout << "server:" << std::endl;
                    std::cout << recvbuf << " \n";
                }
                else {
                    //closesocket(SendRecvSocket);
                    //WSACleanup();
                    //return 1;
                }
            }
        }
    );


    std::thread ForMessage([&]() {
            while (true)
            {
                if (is_new_message) {
                    //sendto(SendRecvSocket, host_name, strlen(host_name), 0, (sockaddr*)&ServerAddr, sizeof(ServerAddr));

                    int size = sendto(

                        SendRecvSocket, 

                        query, 
                        strlen(query), 

                        0, 

                        (sockaddr*)&ServerAddr, 
                        sizeof(ServerAddr)
                    );


                    std::cout << "Query go!" << std::endl;
                    is_new_message = false;
                }
            }
        }
    );

    ForRecvfrom.join();
    ForCin.join();
    ForMessage.join();

    closesocket(SendRecvSocket);
    return 0;
}