#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#pragma comment (lib,"Ws2_32.lib")

#   include <WinSock2.h>
#   include <WS2tcpip.h>

/*
1. ���

SOCK_STREAM � ������������� �������� ����� � ��������������� ���������, ���������� �� ����������.
� ����� ���� ������� ������, ������������ TCP, �� �� ����� ����� ����� ���� �������� TCP - �������.

SOCK_DGRAM � �������� ��������� ��� ����������, �������� �������� ��� ���������� �������.
UDP ����� �������� ��� �������� ��� ������ ����� ����.

SOCK_RAW � ����� ������, ������ ���������� ������ ���������� ������ OSI.
� ����������� ������������ ������ ���� ����������� �� ���������, ������� Windows, ������� � ������ XP.

SOCK_RDM � ������, ��������������� �� ������� �������� ���������.����� ��� ���������� ���������� 4 ������.
��� �������� UDP � ����������, ��� TCP�.������� ������������� �� �� �� �����, � �� ���������.

SOCK_SEQPACKET � ������������ ����������������, �������, ��������������� �������� ������� � ������ ����������.

SOCK_DCCP � ����� ������������ ������������� ��������� ������ ����������.
��������� ��������� � ����������� ���������� ������ ������� � ������������� ����������.
�������� UDP ��� ���������� ���������� ���������� ��������� ������ � �������� �������.

SOCK_PACKET � ������������� ��� Linux ������ ��������� �������.
������������ �������������� �������� ����� ��� ��������� ��������� ����������, ����� ��� RARP � ��������, �� ������ ������������.
*/

/*

2. ��������� ���������� ��� ���������������� �����
	����� � ����� 30 ��������, ������������� � ����� socket.h.
	���� � ����� � Linux : / usr / include / sys / socket.h.������� :

		PF_INET � Internet Protocol ������ 4. ��� ������� TCP / IP �� IPv4.
		PF_UNIX � ��������� ������ ��� Unix sockets.
		PF_INET6 � Internet Protocol ������ 6 ��� IPv6.
		PF_BLUETOOTH � ������ ��� ������ �� ������ ���������� Bluetooth.
		PF_IPX � ��������� IPX / SPX Novell, ��� ��������� ����� 90 - �.


3. ��������� �������
	��������� ������� ���������� ������ ������ � �������������� �������.
	��� ������� � ���������� ���������� �, ��� �������, ����� �� �� ��������, ��� � PF - �������:

		AF_INET ������������� PF_INET.
		AF_UNIX ������������� PF_UNIX.
		AF_INET6 ������������� PF_INET6.
		AF_BLUETOOTH ������������� PF_BLUETOOTH.
		AF_IPX ������������� PF_IPX.

*/

int main(int argc, char const* argv[])
{

	/*������

			���������� �������, ���������������� �� ����� ������������(��������, UDP) :

			1.������� UDP - ����� ������� ������� socket().
			2.��������� ����� � ��������������� ����� ������� ������� bind().
			3.� ����� ������� ������ �� �������, ��������, �������� �� ����� ����������� recvfrom().
			4.����� ��������� ������ ������� ����� ����� ����� ������������ �� �������.
			5.��������� ������� ����� ����� sendto().
			6.������� �����, ��������� close() ��� closesocket().*/


	int result = 0;

	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;

	// ����� ������������� ���������� ������� ���������
	// (������������ Ws2_32.dll)
	result = WSAStartup(wVersionRequested, &wsaData);

	// ���� ��������� ������ ��������� ����������
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
		WSACleanup(); // �������� ���������� Ws2_32.dll
		return -1;
	}

	struct addrinfo* addr = NULL; // ���������, �������� ����������
	// �� IP-������  ���������� ������

	// ������ ��� ������������� ��������� ������
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // AF_INET ����������, ��� ������������ ���� ��� ������ � �������
	hints.ai_socktype = SOCK_STREAM; // ������ ��������� ��� ������
	hints.ai_protocol = IPPROTO_TCP; // ���������� �������� TCP

	// ����� �������� �� �����, ����� ��������� �������� ����������
	hints.ai_flags = AI_PASSIVE;



	// ��� ��������� ������
	enum class SocketStatus : uint8_t {
		connected = 0,
		err_socket_init = 1,
		err_socket_bind = 2,
		err_socket_connect = 3,
		disconnected = 4
	};

	// ������ ������ ���� � ��� ����� ����������� ������ �� ������ �������
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
	// � ��������� ������ ���������� typedef �� std::vector<uint8_t>

	// ��� ������
	enum class SocketType : uint8_t {
		client_socket = 0,
		server_socket = 1
	};

	// ������� ����� TCP �������
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

	// �������������� ���������, �������� ����� ������ - addr.
	// HTTP-������ ����� ������ �� 8000-� ����� ����������
	result = getaddrinfo("127.0.0.1", "8000", &hints, &addr);

	// ���� ������������� ��������� ������ ����������� � �������,
	// ������� ���������� �� ���� � �������� ���������� ��������� 
	if (result != 0) {
		std::cerr << "getaddrinfo failed: " << result << "\n";
		WSACleanup(); // �������� ���������� Ws2_32.dll
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

	// ������� �� �����
	closesocket(UDPSoket);
	freeaddrinfo(addr);

	WSACleanup();

	return EXIT_SUCCESS;
}