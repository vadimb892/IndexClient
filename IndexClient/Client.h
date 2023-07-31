#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include "StringFunctions.h" 
#include <random>

#define BUFFER_SIZE 4096

class Client {
public:
	Client();
	int run();
	int testPerfomance(int querySize, int queryCount);
private:
	SOCKET ClientSocket = INVALID_SOCKET;
	struct sockaddr_in* server_addr = {0};
	std::vector<SOCKET> reccentSockets;

	int connect_socket_client(const char* destination_ip, const char* port);
	void check_net_error(int code, const char* message);
	int send_(std::wstring message);
	int receive(std::wstring* answer);
};

std::wstring randomString(std::size_t length);