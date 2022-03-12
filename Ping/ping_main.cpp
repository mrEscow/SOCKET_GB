#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>
#include <cassert>


#ifdef _WIN32
#   include <process.h>
#   include <WinSock2.h>
#   include <WS2tcpip.h>
#   include <ws2def.h>
#   ifndef getpid
#       define getpid _getpid
#   endif
#else
#   include <unistd.h>
#endif



//int print_ips_with_getaddrinfo(const std::string& host_name)
//{
//    std::cout
//        << "Getting name for \"" << host_name << "\"...\n"
//        << "Using getaddrinfo() function." << std::endl;
//
//    addrinfo hints;
//
//    hints.ai_flags = AI_CANONNAME;
//    // Неважно, IPv4 или IPv6.
//    hints.ai_family = AF_UNSPEC;
//    // TCP stream-sockets.
//    hints.ai_socktype = SOCK_STREAM;
//    // Any protocol.
//    hints.ai_protocol = 0;
//
//
//    // Results.
//    addrinfo* servinfo = nullptr;
//    int status = 0;
//
//
//    if ((status = getaddrinfo(host_name.c_str(), host_name.c_str(), &hints, &servinfo)) != 0)
//    {
//        std::cerr << "getaddrinfo error: " << *gai_strerror(status) << std::endl;
//        return EXIT_FAILURE;
//    }
//
//    for (auto const* s = servinfo; s != nullptr; s = s->ai_next)
//    {
//        std::cout << "Canonical name: ";
//        if (s->ai_canonname)
//            std::cout << s->ai_canonname;
//        std::cout << "\n";
//
//        assert(s->ai_family == s->ai_addr->sa_family);
//        std::cout << "Address type: ";
//
//        if (AF_INET == s->ai_family)
//        {
//            char ip[INET_ADDRSTRLEN];
//
//            std::cout << "AF_INET\n";
//            //sockaddr_in const* const sin = reinterpret_cast<const sockaddr_in* const>(s->ai_addr);
//            sockaddr_in *  sin = (sockaddr_in*)s->ai_addr;
//            std::cout << "Address length: " << sizeof(sin->sin_addr) << "\n";
//
//            in_addr  addr;
//            addr.s_addr = sin->sin_addr.S_un.S_addr;
//
//
//            std::cout << "IP Address: " << inet_ntop(AF_INET, &addr, ip, INET_ADDRSTRLEN) << "\n";
//        }
//        else if (AF_INET6 == s->ai_family)
//        {
//            char ip6[INET6_ADDRSTRLEN];
//
//            std::cout << "AF_INET6\n";
//            // Для хранения адреса IPv6 используется sockaddr_in6.
//            sockaddr_in6 const* const sin = reinterpret_cast<const sockaddr_in6* const>(s->ai_addr);
//            std::cout << "Address length: " << sizeof(sin->sin6_addr) << "\n";
//            std::cout << "IP Address: " << inet_ntop(AF_INET6, &(sin->sin6_addr), ip6, INET6_ADDRSTRLEN) << "\n";
//        }
//        else
//        {
//            std::cout << s->ai_family << "\n";
//        }
//        std::cout << std::endl;
//    }
//    std::cout << std::endl;
//
//    freeaddrinfo(servinfo);
//
//    return EXIT_SUCCESS;
//}


int main(int argc, const char* argv[])
{
    if (argc == 0)
        return 0;

    const std::string host_name = { argv[1] };

    std::cout << host_name << std::endl;

    WSADATA wsaData;
    //SOCKET SendRecvSocket;

    //sockaddr_in ServerAddr;
    //int  err, maxlen = 512;

    //char* recvbuf = new char[maxlen];
    //char* query = new char[maxlen];

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //SendRecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);




    struct addrinfo* servinfo = { 0 };
    int status{ 0 };
    struct addrinfo hints = { 0 };
        // Неважно, IPv4 или IPv6.
    hints.ai_family = AF_UNSPEC;
        // TCP stream-sockets.
    hints.ai_socktype = SOCK_STREAM;
        // "Заполните мой IP-адрес за меня".
    //hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(host_name.c_str(), nullptr, &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo error: " << *gai_strerror(status) << std::endl;
        exit(EXIT_FAILURE);
    }

    for (auto const* s = servinfo; s != nullptr; s = s->ai_next)
    {
        std::cout << "Canonical name: ";
        if (s->ai_canonname)
            std::cout << s->ai_canonname;
        std::cout << "\n";

        assert(s->ai_family == s->ai_addr->sa_family);
        std::cout << "Address type: ";

        if (AF_INET == s->ai_family)
        {
            char ip[INET_ADDRSTRLEN];

            std::cout << "AF_INET\n";
            sockaddr_in const* const sin = reinterpret_cast<const sockaddr_in* const>(s->ai_addr);
            std::cout << "Address length: " << sizeof(sin->sin_addr) << "\n";
            //in_addr addr = { .s_addr = sin->sin_addr };

            std::cout << "IP Address: " << inet_ntop(AF_INET, &sin->sin_addr, ip, INET_ADDRSTRLEN) << "\n";
        }
        else if (AF_INET6 == s->ai_family)
        {
            char ip6[INET6_ADDRSTRLEN];

            std::cout << "AF_INET6\n";
            // Для хранения адреса IPv6 используется sockaddr_in6.
            sockaddr_in6 const* const sin = reinterpret_cast<const sockaddr_in6* const>(s->ai_addr);
            std::cout << "Address length: " << sizeof(sin->sin6_addr) << "\n";
            std::cout << "IP Address: " << inet_ntop(AF_INET6, &(sin->sin6_addr), ip6, INET6_ADDRSTRLEN) << "\n";
        }
        else
        {
            std::cout << s->ai_family << "\n";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    freeaddrinfo(servinfo);
    //char* name = new char[10];

    ////struct hostent* he;

    //sockaddr* addr = nullptr;

    ////addr->sa_family = AF_INET;
    ////addr->sa_data = 

    //socklen_t addrlen = 80;
    //char hbuf[NI_MAXHOST];

    //if (getnameinfo(addr, addrlen, hbuf, sizeof(hbuf), nullptr, 0, NI_NAMEREQD))
    //    std::cerr << "could not resolve hostname" << std::endl;
    //else
    //    std::cout << "host" << hbuf << std::endl;


    //std::cout << gethostname(host_name.c_str(), strlen(host_name.c_str())) << std::endl;

    //std::cout << gethostbyname(host_name.c_str())->h_addr_list << std::endl;

    //unsigned char a = 127;
    //unsigned char b = 0;
    //unsigned char c = 0;
    //unsigned char d = 1;

    //unsigned int destination_address = (a << 24) | (b << 16) | (c << 8) | d;


    //ServerAddr.sin_family = AF_INET;
    //ServerAddr.sin_addr.s_addr = htonl(destination_address);
    //ServerAddr.sin_port = htons(12345);

    //bool is_new_message{ false };

    //std::thread ForCin([&query, &is_new_message]() {
    //    while (true)
    //    {
    //        std::cin >> query;
    //        is_new_message = true;
    //    }
    //    }
    //);

    //std::thread ForRecvfrom([&]() {
    //    while (true)
    //    {
    //        err = recvfrom(SendRecvSocket, recvbuf, maxlen, 0, 0, 0);
    //        if (err > 0) {
    //            recvbuf[err] = '\0';
    //            std::cout << recvbuf << " \n";
    //        }
    //        else {
    //            //closesocket(SendRecvSocket);
    //            //WSACleanup();
    //            //return 1;
    //        }
    //    }
    //    }
    //);


    //std::thread ForMessage([&]() {
    //    while (true)
    //    {
    //        if (is_new_message) {
    //            sendto(SendRecvSocket, host_name.c_str(), strlen(host_name.c_str()), 0, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
    //            int size = sendto(SendRecvSocket, query, strlen(query), 0, (sockaddr*)&ServerAddr, sizeof(ServerAddr));
    //            is_new_message = false;
    //        }
    //    }
    //    }
    //);

    //ForRecvfrom.join();
    //ForCin.join();
    //ForMessage.join();

    //closesocket(SendRecvSocket);
    return 0;
}