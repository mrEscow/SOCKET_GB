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

    WSADATA wsaData;
    SOCKET SendRecvSocket;

    sockaddr_in ServerAddr, ClientAddr;
    int  err, maxlen = 512, ClientAddrSize = sizeof(ClientAddr);

    char* recvbuf = new char[maxlen];
    char* name = new char[maxlen];
    char* message = new char[maxlen];

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

    bool is_name{ false };
    size_t size_name{ 0 };
    bool is_message{ false };
    size_t size_message{ 0 };

    bool is_exit{ 0 };



    while (true)
    {

        err = recvfrom(SendRecvSocket, recvbuf, maxlen, 0, (sockaddr*)&ClientAddr, &ClientAddrSize);
        std::cout << "SIZE: "<<err << std::endl;
        if (err > 0) {


            recvbuf[err] = '\0';


            if (!is_name) {
                std::cout << recvbuf << std::endl;
                size_name = err;
                for (size_t i = 0; i < size_name; i++)
                    name[i] = recvbuf[i];
                is_name = true;
            }
            else {
                std::cout << recvbuf << std::endl;
                size_message = err;
                for (size_t i = 0; i < size_message; i++)
                    message[i] = recvbuf[i];
                is_message = true;

                std::string exit;

                for (size_t i = 0; i < 4; i++)
                    exit += message[i];

                if (exit == "exit") {
                    is_exit = true;
                    std::cout << "EXIT" << std::endl;
                }
            }

            if (is_exit) {
                sendto(SendRecvSocket, "SERVER_IS_EXIT! GOODBAY!", strlen("SERVER_IS_EXIT! GOODBAY!"), 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));
                closesocket(SendRecvSocket);
                WSACleanup();
                return 0;
            }

            if (is_name && is_message) {

                std::string frame;
                for (size_t i = 0; i <  (size_name > size_message ? size_name : size_message); i++)
                    frame += "*";
                
                sendto(SendRecvSocket, frame.c_str(), strlen(frame.c_str()), 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));

                sendto(SendRecvSocket, "NAME:", strlen("NAME:"), 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));
                sendto(SendRecvSocket, name, size_name, 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));
                sendto(SendRecvSocket, "MASSEGE:", strlen("MASSEGE:"), 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));
                sendto(SendRecvSocket, message, size_message, 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));

                sendto(SendRecvSocket, frame.c_str(), strlen(frame.c_str()), 0, (sockaddr*)&ClientAddr, sizeof(ClientAddr));

                is_name = false;
                is_message = false;

            }

        }
        else
        {
            std::cout << "recv failed: " << WSAGetLastError() << std::endl;
            //closesocket(SendRecvSocket);
            //WSACleanup();
            //return 1;
        }


    }

    return 0;
}