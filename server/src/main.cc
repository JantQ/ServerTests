#include <iostream>
#include <print>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main() {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(55555);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	
	listen(serverSocket, 5);

	while (true) {
		int clientSocket = accept(serverSocket, nullptr, nullptr);

		char buffer[1024] = {0};
		recv(clientSocket, buffer, sizeof(buffer), 0);
		std::println("Message from client: {}", buffer);

		std::string response = "Hello from the server! I received your message.\n";
		send(clientSocket, response.c_str(), response.length(), 0);
		close(clientSocket);
	}
	close(serverSocket);
	return 0;
}
