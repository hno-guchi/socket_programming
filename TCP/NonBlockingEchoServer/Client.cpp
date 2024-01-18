#include "./Client.hpp"
#include "../utils/utils.hpp"

Client::Client(const std::string& serverIP, unsigned short serverPort) : socket_(0) {
	this->socket_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->socket_ < 0) {
		fatalError("socket");
	}
	memset(&this->serverAddr_, 0, sizeof(this->serverAddr_));
	this->serverAddr_.sin_family = AF_INET;
	this->serverAddr_.sin_addr.s_addr = inet_addr(serverIP.c_str());
	this->serverAddr_.sin_port = htons(serverPort);
}

Client::~Client() {
	close(this->socket_);
}

void Client::connectToServer() {
	if (connect(this->socket_, reinterpret_cast<struct sockaddr *>(&this->serverAddr_), sizeof(this->serverAddr_)) < 0) {
		fatalError("connect");
	}
}

void Client::sendMessage(const std::string& message) {
	if (send(this->socket_, message.c_str(), message.size(), 0) != static_cast<ssize_t>(message.size())) {
		fatalError("send");
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
		fatalError("recv");
	}
	std::cout << std::endl;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <Echo Word>" << std::endl;
		exit(1);
	}

	const std::string	message = argv[1];

	Client	client("127.0.0.1", 8080);

	client.connectToServer();
	client.sendMessage(message);
	client.receiveMessage();

	return (0);
}
