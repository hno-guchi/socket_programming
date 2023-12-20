#include "TCPEchoClient.hpp"

TCPClient::TCPClient(const char *serverIP, unsigned short serverPort) {
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		DieWithError("socket() failed");
	}
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	serverAddr.sin_port = htons(serverPort);
}

TCPClient::~TCPClient() {
    close(sock);
}

void TCPClient::connectToServer() {
    if (connect(sock, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        DieWithError("connect() failed");
    }
}

void TCPClient::sendMessage(const char *message) {
    size_t messageLen = strlen(message);
    if (send(sock, message, messageLen, 0) != static_cast<int>(messageLen)) {
        DieWithError("send() sent a different number of bytes than expected");
    }
}

void TCPClient::receiveMessage() {
    char echoBuffer[RCVBUFSIZE] = {0};
    int bytesRcvd = 0;

    std::cout << "Received: ";
    while ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) > 0) {
        echoBuffer[bytesRcvd] = '\0';
        std::cerr << echoBuffer << std::endl;
		memset(echoBuffer, 0, sizeof(echoBuffer));
    }

    if (bytesRcvd < 0) {
        DieWithError("recv() failed or connection closed prematurely");
    }

    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <Server IP> <Echo Word> [<Echo Port>}\n";
        exit(1);
    }

    const char *serverIP = argv[1];
    const char *echoString = argv[2];
    unsigned short echoServPort = (argc == 4) ? atoi(argv[3]) : 12345;

    TCPClient client(serverIP, echoServPort);
    client.connectToServer();
	// std::cout << "Connected...(client)" << std::endl;
    client.sendMessage(echoString);
	// std::cout << "Sent...(client)" << std::endl;
    client.receiveMessage();
	// std::cout << "Received...(client)" << std::endl;

    return (0);
}
