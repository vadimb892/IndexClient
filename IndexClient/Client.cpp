#include "Client.h"

Client::Client() {}

int Client::connect_socket_client(const char* destination_ip, const char* port) {
	struct addrinfo peer = {0}, * result = {0};
	ZeroMemory(&peer, sizeof(peer));
	peer.ai_family = AF_INET;
	peer.ai_socktype = SOCK_STREAM;
	peer.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(destination_ip, port, &peer, &result)) {
		printf("C: getaddrinfo failed!");
		return -1;}

	ClientSocket = socket(result->ai_family, result->ai_socktype,
		result->ai_protocol);
	while (std::find(reccentSockets.begin(), reccentSockets.end(), ClientSocket) 
		!= reccentSockets.end()) {
		ClientSocket = socket(result->ai_family, result->ai_socktype,
			result->ai_protocol);
	}
	if (ClientSocket != INVALID_SOCKET) 
		reccentSockets.push_back(ClientSocket);
	else{
		printf("C: Calling socket error!\n");
		return -1;}

	if (connect(ClientSocket, result->ai_addr, (int)result->ai_addrlen)) {
		printf("C: Calling connect error!\n");
		return -1;}
	return 0;
}

void Client::check_net_error(int code, const char* message) {
	if (code == -1) {
		if (ClientSocket != INVALID_SOCKET)
			closesocket(ClientSocket);
		WSACleanup();
		throw std::runtime_error(message);}
}

int Client::send_(std::wstring message) {
	std::string smessage = ws2s(message);
	smessage += std::string(BUFFER_SIZE - (smessage.size() % BUFFER_SIZE), ' ');
	long long dataLength = smessage.size();
	char lenBuf[sizeof(long long)];
	memcpy(&lenBuf[0], &dataLength, sizeof(long long));
	if (send(ClientSocket, &lenBuf[0], sizeof(long long), 0) == -1) {
		printf("C) Calling send error!\n");
		return -1;
	}
	char sndBuf[BUFFER_SIZE];
	long long i = 0;
	while (i < dataLength) {
		memcpy(&sndBuf[0], &smessage[i], BUFFER_SIZE);
		if (send(ClientSocket, &sndBuf[0], BUFFER_SIZE, 0) == -1) {
			printf("C) Calling send error!\n");
			return -1;
		}
		i += BUFFER_SIZE;
	}
	return 0;
}

int Client::receive(std::wstring* answer) {
	long long dataLength = 0;
	char lenBuf[sizeof(long long)];
	std::string receivedString;

	if (recv(ClientSocket, &lenBuf[0], sizeof(long long), 0) == -1) {
		printf("C) Calling recv error!\n");
		return -1;
	}
	memcpy(&dataLength, lenBuf, sizeof(long long));
	receivedString.resize(dataLength);
	char rcvBuf[BUFFER_SIZE];
	long long i = 0;
	while (i < dataLength) {
		if (recv(ClientSocket, &rcvBuf[0], BUFFER_SIZE, 0) == -1) {
			printf("C) Calling recv error!\n");
			return -1;
		}
		memcpy(&receivedString[i], &rcvBuf[0], BUFFER_SIZE);
		i += BUFFER_SIZE;
	}
	std::string::size_type pos = receivedString.find_last_not_of(' ') + 1;
	receivedString = std::string(receivedString.begin(), receivedString.begin() + pos);
	*answer = s2ws(receivedString);
	return dataLength;
}

int Client::run() {
	char destination_ip[] = "127.0.0.1";
	const char* port = "1111";
	std::wstring buffer;
	unsigned char buf[sizeof(struct in6_addr)];
	auto start = std::chrono::steady_clock::now();
	std::chrono::microseconds RTT;
	WSADATA wsd;

	int socksize = sizeof(sockaddr), *sockSizePnt = &socksize, dataAmount;
	struct sockaddr_in server_addr = { 0 };
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = (unsigned short)strtoul(port, NULL, 0);
	server_addr.sin_addr.s_addr = inet_pton(AF_INET, destination_ip, buf);

	check_net_error(WSAStartup(MAKEWORD(2, 2), &wsd), 
			"S: Failed to load Winsock library!\n");
	while (true) {
		ClientSocket = INVALID_SOCKET;
		std::wcout << L"U) Input:";
		std::getline(std::wcin, buffer);
		if (buffer == L"exit")
			break;
		check_net_error(connect_socket_client(destination_ip, port), "conn_error");
		start = std::chrono::steady_clock::now();
		check_net_error(send_(L"{\"query\":"+ buffer + L"}"), "send_error");
		check_net_error(dataAmount = receive(&buffer), "recv_error");
		RTT = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now() - start);
		std::wcout << L"A) Server[RTT: " << RTT.count() <<
			L" qs, msg size: " << dataAmount << L" B]: " << buffer << std::endl;
		closesocket(ClientSocket);
	}
	WSACleanup();
	return 0;
}

int Client::testPerfomance(int querySize, int queryCount) {
	char destination_ip[] = "127.0.0.1";
	const char* port = "1111";
	std::wstring buffer;
	unsigned char buf[sizeof(struct in6_addr)];
	auto start = std::chrono::steady_clock::now();
	std::chrono::microseconds RTT;
	WSADATA wsd;

	int socksize = sizeof(sockaddr), * sockSizePnt = &socksize, dataAmount;
	struct sockaddr_in server_addr = { 0 };
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = (unsigned short)strtoul(port, NULL, 0);
	server_addr.sin_addr.s_addr = inet_pton(AF_INET, destination_ip, buf);

	check_net_error(WSAStartup(MAKEWORD(2, 2), &wsd),
		"S: Failed to load Winsock library!\n");
	int i = 0;
	while (i < queryCount) {
		ClientSocket = INVALID_SOCKET;
		buffer = randomString(querySize);
		std::wcout << L"U) Input:" << buffer << std::endl;
		check_net_error(connect_socket_client(destination_ip, port), "conn_error");
		start = std::chrono::steady_clock::now();
		check_net_error(send_(L"{\"query\":\"" + buffer + L"\"}"), "send_error");
		check_net_error(dataAmount = receive(&buffer), "recv_error");
		RTT = std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now() - start);
		std::wcout << L"A) Server[RTT: " << RTT.count() <<
			L" qs, msg size: " << dataAmount << L" B]: " << buffer << std::endl;
		closesocket(ClientSocket);
		i++;
	}
	WSACleanup();
	return 0;
}

std::wstring randomString(std::size_t length) {
	const std::wstring CHARACTERS = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	std::wstring string;
	std::random_device randomDevice;
	std::mt19937 generator(randomDevice());
	std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

	for (std::size_t i = 0; i < length; ++i)
	{
		string += CHARACTERS[distribution(generator)];
	}
	return string;
}