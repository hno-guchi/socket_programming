#include "./Client.hpp"

Client::Client(const char *serverIP, unsigned short serverPort) : socket_(0) {
	this->socket_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->socket_ < 0) {
		DieWithError("socket() failed");
	}
	memset(&this->serverAddr_, 0, sizeof(this->serverAddr_));
	this->serverAddr_.sin_family = AF_INET;
	this->serverAddr_.sin_addr.s_addr = inet_addr(serverIP);
	this->serverAddr_.sin_port = htons(serverPort);
}

Client::~Client() {
	close(this->socket_);
}

void Client::connectToServer() {
	if (connect(this->socket_, reinterpret_cast<struct sockaddr *>(&this->serverAddr_), sizeof(this->serverAddr_)) < 0) {
		DieWithError("connect() failed");
	}
}

void Client::sendMessage(const char *message) {
	size_t	messageLen = strlen(message);
	if (send(this->socket_, message, messageLen, 0) != static_cast<int>(messageLen)) {
		DieWithError("send() sent a different number of bytes than expected");
	}
}

void Client::receiveMessage() {
	char	echoBuffer[RCVBUFSIZE] = {0};
	int		bytesRcvd = 0;

	std::cout << "Received: ";
	while ((bytesRcvd = recv(this->socket_, echoBuffer, RCVBUFSIZE - 1, 0)) > 0) {
		echoBuffer[bytesRcvd] = '\0';
		std::cout << echoBuffer << std::endl;
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

	const char		*serverIP = argv[1];
	const char		*echoString = argv[2];
	unsigned short	echoServPort = (argc == 4) ? atoi(argv[3]) : 12345;

	Client	client(serverIP, echoServPort);

	client.connectToServer();
	client.sendMessage(echoString);
	client.receiveMessage();

	return (0);
}
