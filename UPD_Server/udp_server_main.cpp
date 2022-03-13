#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#pragma comment (lib,"Ws2_32.lib")

#   include <WinSock2.h>
#   include <WS2tcpip.h>

/*
1. Тип

SOCK_STREAM — упорядоченный байтовый поток с гарантированной доставкой, основанный на соединении.
К этому типу относят сокеты, использующие TCP, но не любой сокет этого типа является TCP - сокетом.

SOCK_DGRAM — отправка датаграмм без соединения, гарантии доставки или сохранения порядка.
UDP может работать как протокол для сокета этого типа.

SOCK_RAW — сырые сокеты, данные передаются поверх канального уровня OSI.
В большинстве операционных систем была реализована их поддержка, включая Windows, начиная с версии XP.

SOCK_RDM — сокеты, ориентированные на надёжную доставку сообщений.Нужны для реализации протоколов 4 уровня.
Это «надёжнее UDP и отзывчивее, чем TCP».Решение ориентировано НЕ на не поток, а на сообщения.

SOCK_SEQPACKET — обеспечивает последовательную, надёжную, двунаправленную передачу записей в режиме соединения.

SOCK_DCCP — сокет минимального транспортного протокола общего назначения.
Управляет созданием и завершением ненадёжного потока пакетов и отслеживанием перегрузок.
Заменяет UDP при реализации протоколов трансляции потоковых данных в реальном времени.

SOCK_PACKET — специфический для Linux способ получения пакетов.
Используется разработчиками сетевого стека для написания служебных протоколов, таких как RARP и подобных, на уровне пользователя.
*/

/*

2. Семейство протоколов или коммуникационный домен
	Всего — более 30 семейств, перечисленных в файле socket.h.
	Путь к файлу в Linux : / usr / include / sys / socket.h.Примеры :

		PF_INET — Internet Protocol версии 4. Это обычный TCP / IP на IPv4.
		PF_UNIX — локальные сокеты или Unix sockets.
		PF_INET6 — Internet Protocol версии 6 или IPv6.
		PF_BLUETOOTH — сокеты для работы со стеком протоколов Bluetooth.
		PF_IPX — протоколы IPX / SPX Novell, для локальных сетей 90 - х.


3. Семейство адресов
	Семейства адресов определяют формат адреса и преобразования формата.
	Они связаны с семейством протоколов и, как правило, имеют те же значения, что и PF - макросы:

		AF_INET соответствует PF_INET.
		AF_UNIX соответствует PF_UNIX.
		AF_INET6 соответствует PF_INET6.
		AF_BLUETOOTH соответствует PF_BLUETOOTH.
		AF_IPX соответствует PF_IPX.

*/

int main(int argc, char const* argv[])
{

	/*Сервер

			Реализация сервера, ориентированного на обмен датаграммами(например, UDP) :

			1.Создать UDP - сокет вызовом функции socket().
			2.Привязать сокет к прослушиваемому порту вызовом функции bind().
			3.В цикле ожидать данных от клиента, например, принимая их через блокирующий recvfrom().
			4.После получения данных функция также вернёт адрес отправившего их клиента.
			5.Отправить клиенту ответ через sendto().
			6.Закрыть сокет, используя close() или closesocket().*/


	int result = 0;

	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;

	// старт использования библиотеки сокетов процессом
	// (подгружается Ws2_32.dll)
	result = WSAStartup(wVersionRequested, &wsaData);

	// Если произошла ошибка подгрузки библиотеки
	if (result != 0) {
		std::cerr << "WSAStartup failed: " << result << "\n";
		return result;
	}

	SOCKET UDPSoket;


	sockaddr_in  ClientAddr;

	UDPSoket = socket(
		AF_INET,
		SOCK_DGRAM,
		IPPROTO_UDP
	);

	// IP ANY
	uint32_t ANY_IP = INADDR_ANY;
	// IP 127.0.0.1
	uint32_t LOCALHOST_IP = 0x0100007f;


	sockaddr_in ServerAddr;

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = LOCALHOST_IP;
	ServerAddr.sin_port = htons(9999);


	// Descriptor + IP + Port
	result = bind(UDPSoket, (sockaddr*)&ServerAddr, sizeof(ServerAddr));

	if (result != 0) {
		std::cout << "bind failed: " << WSAGetLastError() << std::endl;
		closesocket(UDPSoket);
		WSACleanup(); // выгрузка библиотеки Ws2_32.dll
		return -1;
	}

	struct addrinfo* addr = NULL; // структура, хранящая информацию
	// об IP-адресе  слущающего сокета

	// Шаблон для инициализации структуры адреса
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // AF_INET определяет, что используется сеть для работы с сокетом
	hints.ai_socktype = SOCK_STREAM; // Задаем потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP; // Используем протокол TCP

	// Сокет биндится на адрес, чтобы принимать входящие соединения
	hints.ai_flags = AI_PASSIVE;



	// Код состояния сокета
	enum class SocketStatus : uint8_t {
		connected = 0,
		err_socket_init = 1,
		err_socket_bind = 2,
		err_socket_connect = 3,
		disconnected = 4
	};

	// Буффер данных куда у нас будет приниматься данные от другой стороны
	struct DataBuffer {
		int size = 0;
		void* data_ptr = nullptr;

		DataBuffer() = default;
		DataBuffer(int size, void* data_ptr) : size(size), data_ptr(data_ptr) {}
		DataBuffer(const DataBuffer& other) : size(other.size), data_ptr(malloc(size)) { memcpy(data_ptr, other.data_ptr, size); }
		DataBuffer(DataBuffer&& other) : size(other.size), data_ptr(other.data_ptr) { other.data_ptr = nullptr; }
		~DataBuffer() { if (data_ptr) free(data_ptr); data_ptr = nullptr; }

		bool isEmpty() { return !data_ptr || !size; }
		operator bool() { return data_ptr && size; }
	};
	// в последней версии библиотеки typedef от std::vector<uint8_t>

	// Тип сокета
	enum class SocketType : uint8_t {
		client_socket = 0,
		server_socket = 1
	};

	// Базовый класс TCP клиента
	class TcpClientBase {
	public:
		typedef SocketStatus status;
		virtual ~TcpClientBase() {};
		virtual status disconnect() = 0;
		virtual status getStatus() const = 0;
		virtual bool sendData(const void* buffer, const size_t size) const = 0;
		virtual DataBuffer loadData() = 0;
		virtual uint32_t getHost() const = 0;
		virtual uint16_t getPort() const = 0;
		virtual SocketType getType() const = 0;
	};

	// Инициализируем структуру, хранящую адрес сокета - addr.
	// HTTP-сервер будет висеть на 8000-м порту локалхоста
	result = getaddrinfo("127.0.0.1", "8000", &hints, &addr);

	// Если инициализация структуры адреса завершилась с ошибкой,
	// выведем сообщением об этом и завершим выполнение программы 
	if (result != 0) {
		std::cerr << "getaddrinfo failed: " << result << "\n";
		WSACleanup(); // выгрузка библиотеки Ws2_32.dll
		return 1;
	}


	const int maxlen = 1024;

	char* recvbuf{ 0 };

	socklen_t client_address_len = sizeof(sockaddr_in);

	struct MyObject
	{
		int x;
		int y;
	};

	MyObject first{ 100,500 };
	int SIZE = sizeof(first);

	DataBuffer dataBuffer(SIZE, &first);

/*
	ssize_t sendto(
	int								socket, 
	const void*						message, 
	size_t							length,
	int								flags, 
	const struct sockaddr*			address,
	socklen_t						address_len
	);

	ssize_t recvfrom(
	int								socket, 
	void* restrict					message, 
	size_t							length,
	int								flags, 
	struct sockaddr* restrict		address,
	socklen_t* restrict				address_len
	);
*/

	while (true)
	{

		std::cout << "WAIT" << std::endl;

		result = recvfrom(
			UDPSoket,
			recvbuf,
			maxlen,
			0,
			(sockaddr*)&ClientAddr,
			&client_address_len
		);

		if (result > 0) {
			std::cout << "New buffer!" << std::endl;
			recvbuf[result] = '\0';
			std::cout << recvbuf << std::endl;
			sendto(
				UDPSoket,
				recvbuf,
				maxlen,
				0,
				(sockaddr*)&ClientAddr,
				client_address_len
			);
		}


	}

	// Убираем за собой
	closesocket(UDPSoket);
	freeaddrinfo(addr);

	WSACleanup();

	return EXIT_SUCCESS;
}