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


int main(int argc, const char* argv[])
{
    const std::string host_name = { argv[0] };

    WSADATA wsaData;
    SOCKET SendRecvSocket;

    sockaddr_in ServerAddr;
    int  err, maxlen = 512;

    char* recvbuf = new char[maxlen];
    char* query = new char[maxlen];

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SendRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    unsigned char a = 127;
    unsigned char b = 0;
    unsigned char c = 0;
    unsigned char d = 1;

    unsigned int destination_address = (a << 24) | (b << 16) | (c << 8) | d;
 
 

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = htonl(destination_address);
    ServerAddr.sin_port = htons(12345);


    bool is_new_message{ false };

    std::thread ForCin([&query,&is_new_message]() {
        while (true)
        {
            //std::cout << "message:" << std::endl;
            std::cin >> query;
            is_new_message = true;
            //std::cout << "true:" << std::endl;
        }

    });

    std::thread ForRecvfrom([&]() {
        while (true)
        {
            err = recvfrom(SendRecvSocket, recvbuf, maxlen, 0, 0, 0);

            if (err > 0) {
                recvbuf[err] = '\0';
                std::cout << recvbuf << " \n";
                //std::cout << "err > 0" << std::endl;
                //err = 0;

            }
            else {
                //std::cout << "else" << std::endl;
                //closesocket(SendRecvSocket);
                //WSACleanup();
                //return 1;
            }
        }
        });



    std::thread ForMessage([&]() {
        while (true)
        {
            //std::cout << "ForMessage:" << std::endl;


            if (is_new_message) {

                sendto(SendRecvSocket, host_name.c_str(), strlen(host_name.c_str()), 0, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
                int size = sendto(SendRecvSocket, query, strlen(query), 0, (sockaddr*)&ServerAddr, sizeof(ServerAddr));

                //std::cout << size << std::endl;
                std::cout << "new_message" << std::endl;
                is_new_message = false;
            }


        }
        });

    //while (true)
    //{

    ForRecvfrom.join();
    ForCin.join();
    ForMessage.join();

    //}


    closesocket(SendRecvSocket);
    return 0;
}