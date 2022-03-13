#pragma comment (lib,"Ws2_32.lib")

#   include <WinSock2.h>
#   include <WS2tcpip.h>


#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>


#include <iostream>
#include <cstdlib>

#include <string>


int main(int argc, char const* argv[]) {

	SOCKET UDPSoket;

	UDPSoket = socket(
		AF_INET,
		SOCK_DGRAM,
		IPPROTO_UDP
	);

	const int BUFF_SIZE = 1024;
	char* BUFF = new char[BUFF_SIZE];


	sockaddr_in		myAddr;
	socklen_t		client_address_len = sizeof(sockaddr_in);


	struct SPAM
	{
		int x;
		int y;
	};

	SPAM spam{ 100,500 };


	char f[8];

	int a = 10;
	int b = 20;

	std::string str;
	str = std::to_string(a) + std::to_string(b);

	std::cout << str << std::endl;

	//str.toCharArray(b, 2);

	//char* testBuff = new char[255];    //наш массив символов для хранения числа
	//int testInt = 372;                 //само число
	//_itoa(testInt, testBuff, 10);            //функция преобразования числа в строку, где
	//								//argc - число, argv - массив символов, 10 - основание, т.е. система счисления.
	//std::cout << testBuff << '\n';
	//delete testBuff;                    //освобождаем память, выделенную под наш массив

	while (true)
	{
		sendto(

			UDPSoket,

			BUFF,
			BUFF_SIZE,

			0,

			(sockaddr*)&myAddr,
			client_address_len

		);
	}


	return EXIT_SUCCESS;
}