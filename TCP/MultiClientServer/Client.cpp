#include "./Client.hpp"
#include "../utils/utils.hpp"

Client::Client(const std::string& serverIP, unsigned short serverPort) :
	socket_(0), messageSize_(0) {
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
	this->messageSize_ = message.size();
}

void Client::receiveMessage() {
	size_t	totalBytesRecved = 0;

	std::cout << "Received: ";
	// blocking
	while (totalBytesRecved < this->messageSize_) {
		char	echoBuffer[RCVBUFSIZE] = {0};
		int		bytesRecved = 0;
		if ((bytesRecved = recv(this->socket_, echoBuffer, RCVBUFSIZE - 1, 0)) < 0) {
			fatalError("recv");
		}
		echoBuffer[bytesRecved] = '\0';
		std::cout << echoBuffer << std::flush;
		totalBytesRecved += bytesRecved;
	}
	// while ((bytesRecved = recv(this->socket_, echoBuffer, RCVBUFSIZE - 1, 0)) > 0) {
	// 	echoBuffer[bytesRecved] = '\0';
	// 	std::cout << echoBuffer << std::flush;
	// 	memset(echoBuffer, 0, sizeof(echoBuffer));
	// 	bytesRecved = 0;
    // }
	// if (bytesRecved < 0) {
	// 	fatalError("recv");
	// }
	std::cout << std::endl;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <Echo Word>\n";
		exit(1);
	}

	const std::string	message = argv[1];

	Client	client("127.0.0.1", 8080);

	client.connectToServer();
	while (1) {
		client.sendMessage(message);
		client.receiveMessage();
		sleep(5);
	}
	return (0);
}
