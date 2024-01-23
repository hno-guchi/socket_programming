#include "./Client.hpp"
#include "../utils/utils.hpp"

Client::Client(const std::string& serverIP, unsigned short serverPort) :
	socketFd_(0), messageSize_(0) {
	this->socketFd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->socketFd_ < 0) {
		fatalError("socket");
	}
	int	flags = 0;
	if ((flags = fcntl(this->socketFd_, F_GETFL, 0)) < 0) {
		fatalError("fcntl");
	}
	flags |= O_NONBLOCK;
	if (fcntl(this->socketFd_, F_SETFL, flags) < 0) {
		fatalError("fcntl");
	}

	memset(&this->serverSocketAddress_, 0, sizeof(this->serverSocketAddress_));
	this->serverSocketAddress_.sin_family = AF_INET;
	this->serverSocketAddress_.sin_addr.s_addr = inet_addr(serverIP.c_str());
	this->serverSocketAddress_.sin_port = htons(serverPort);
}

Client::~Client() {
	close(this->socketFd_);
}

void Client::connectToServer() {
	struct pollfd	pollFd;

	pollFd.fd = this->socketFd_;
	pollFd.events = POLLOUT;

	if (connect(this->socketFd_, reinterpret_cast<struct sockaddr *>(&this->serverSocketAddress_), sizeof(this->serverSocketAddress_)) < 0) {
		if (errno == EINPROGRESS) {
			std::cout << "Connecting..." << std::endl;
			while (1) {
				int		pollRet = 0;

				pollRet = poll(&pollFd, 1, 1000);
				if (pollRet < 0) {
					fatalError("poll");
				} else if (pollRet == 0) {
					std::cout << "No data received." << std::endl;
					continue;
				} else {
					if (pollFd.revents & POLLOUT) {
						std::cout << "Connection established!" << std::endl;
						break;
					}
				}
			}
		} else {
			fatalError("connect");
		}
	} else {
		std::cout << "Connection established!" << std::endl;
	}
}

void Client::sendMessage(const std::string& message) {
	if (send(this->socketFd_, message.c_str(), message.size(), 0) != static_cast<ssize_t>(message.size())) {
		fatalError("send");
	}
	this->messageSize_ = message.size();
}

void Client::receiveMessage() {
	size_t	totalSizeRecved = 0;

	std::cout << "Received: ";
	while (totalSizeRecved < this->messageSize_) {
		while (1) {
			char	echoBuffer[RCVBUFSIZE] = {0};
			int		sizeRecved = 0;

			// Non blocking
			sizeRecved = recv(this->socketFd_, echoBuffer, RCVBUFSIZE - 1, MSG_DONTWAIT);
			if (sizeRecved < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					// std::cout << "No data received." << std::endl;
					errno = 0;
					continue;
				} else {
					fatalError("recv");
				}
			} else {
				echoBuffer[sizeRecved] = '\0';
				std::cout << echoBuffer << std::flush;
				totalSizeRecved += sizeRecved;
				break;
			}
		}
	}
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
