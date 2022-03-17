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


    char* Tstruct = new char[maxlen];

    struct TestStruct
    {
        float x;
        float y;
    };

    TestStruct testStructOut{-1000,1000};


    std::thread ForCin([&]() {
            while (true)
            {
                std::cin >> Tstruct;
                //std::cin >> query;

                query = (char*)&testStructOut;
                std::cout << "\nclient:" << std::endl;
                std::cout << "Strlen(query): " << strlen(query) << std::endl;
                std::cout << "TestStructOut:" << std::endl;
                std::cout << "sizeof: " << sizeof(testStructOut) << std::endl;
                std::cout << "sizeof: " << sizeof(query) << std::endl;
                std::cout 
                    << " X " << testStructOut.x
                    << " Y " << testStructOut.y 
                    << std::endl;
                std::cout
                    << "query:\n" 
                    << " X " << ((TestStruct*)query)->x
                    << " Y " << ((TestStruct*)query)->y
                    << std::endl;

                is_new_message = true;

                if (is_new_message) {
                    //sendto(SendRecvSocket, host_name, strlen(host_name), 0, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
                    
                    int size = sendto(

                        SendRecvSocket,

                        query,
                        sizeof(query),
                        //8,
                        //strlen(query),

                        0,

                        (sockaddr*)&ServerAddr,
                        sizeof(ServerAddr)
                    );

                    is_new_message = false;
                }
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

                    std::cout << "\nserver:" << std::endl;
                    std::cout << "SizeBuff: " << err << " \n";
                    std::cout << "SizeofBuff: " << sizeof(recvbuf) << std::endl;
                    std::cout << recvbuf << " \n";

                    std::cout << "TestStructIn:" << std::endl;
                    TestStruct testStructIn;
                    testStructIn = *(TestStruct*)recvbuf;
                    std::cout << "SizeofBuff: " << sizeof(recvbuf) << std::endl;
                    std::cout << "SizeofStruct: " << sizeof(testStructIn) << std::endl;
                    std::cout 
                        << " X " << testStructIn.x
                        << " Y " << testStructIn.y 
                        << std::endl;
                    std::cout
                        << " X " << ((TestStruct*)recvbuf)->x
                        << " Y " << ((TestStruct*)recvbuf)->y
                        << std::endl;
                }
                else {
                    //closesocket(SendRecvSocket);
                    //WSACleanup();
                    //return 1;
                }
            }
        }
    );


    ForRecvfrom.join();
    ForCin.join();

    // clean 
    closesocket(SendRecvSocket);
    WSACleanup();

    return 0;
}