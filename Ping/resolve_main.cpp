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



int main(int argc, const char* argv[])
{
    if (argc == 0)
        return 0;

    const std::string host_name = { argv[1] };


    WSADATA wsaData;

    WSAStartup(MAKEWORD(2, 2), &wsaData);


    addrinfo* servinfo = { 0 };
    int status{ 0 };
    addrinfo hints = { 0 };
        // Неважно, IPv4 или IPv6.
    hints.ai_family = AF_UNSPEC;
        // TCP stream-sockets.
    hints.ai_socktype = SOCK_STREAM;
        // "Заполните мой IP-адрес за меня".
    //hints.ai_flags = AI_PASSIVE;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;


 
    if ((status = getaddrinfo(host_name.c_str(), nullptr, &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo error: " << *gai_strerror(status) << std::endl;
        exit(EXIT_FAILURE);
    }

    for (auto const* s = servinfo; s != nullptr; s = s->ai_next)
    {
        std::cout << host_name << std::endl;

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

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return 0;
}