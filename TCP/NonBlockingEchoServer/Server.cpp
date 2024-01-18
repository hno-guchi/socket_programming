#include "./Server.hpp"
#include "../utils/utils.hpp"

Server::Server(unsigned short port) : socketFd_(0) {
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

	struct sockaddr_in	echoServAddr;
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddr.sin_port = htons(port);

	if (bind(this->socketFd_, reinterpret_cast<struct sockaddr *>(&echoServAddr), sizeof(echoServAddr)) < 0) {
		fatalError("bind");
	}
	if (listen(this->socketFd_, MAXPENDING) < 0) {
		fatalError("listen");
	}
}

Server::~Server() {
	close(this->socketFd_);
}

static void	handleClient(int clntSocket) {
	while (1) {
		char	echoBuffer[RCVBUFSIZE] = {0};
		ssize_t	recvMsgSize = 0;

		while (1) {
			sleep(2);
			recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0);
			if (recvMsgSize < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					std::cout << "No data received" << std::endl;
					errno = 0;
					recvMsgSize = 0;
					continue;
				} else {
					fatalError("recv");
				}
			} else {
				break;
			}
		}
		if (recvMsgSize == 0) {
			break;
		}
		while (1) {
			ssize_t	sendMsgSize = 0;
			sendMsgSize = send(clntSocket, echoBuffer, recvMsgSize, 0);
			if (sendMsgSize < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					std::cout << "send() timeout" << std::endl;
					errno = 0;
					sendMsgSize = 0;
					continue;
				} else {
					fatalError("send");
				}
			} else if (sendMsgSize != recvMsgSize) {
				fatalError("send");
			} else {
				break;
			}
		}
	}
	std::cout << "Client disconnected" << std::endl;
	close(clntSocket);
}

void Server::run() {
	while (1) {
		sleep(3);
		struct sockaddr_in	echoClntAddr;
		unsigned int		clntLen(sizeof(echoClntAddr));
		int					clntSock(0);

		memset(&echoClntAddr, 0, sizeof(echoClntAddr));
		// block until receive client request
		clntSock = accept(this->socketFd_, reinterpret_cast<struct sockaddr *>(&echoClntAddr), &clntLen);
		if (clntSock < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				std::cout << "accept() timeout." << std::endl;
				errno = 0;
				continue;
			} else {
				fatalError("accept");
			}
		} else {
			std::cout << "Handling client " << inet_ntoa(echoClntAddr.sin_addr) << std::endl;
			int	flags = 0;
			if ((flags = fcntl(clntSock, F_GETFL, 0)) < 0) {
				fatalError("fcntl");
			}
			flags |= O_NONBLOCK;
			if (fcntl(clntSock, F_SETFL, flags) < 0) {
				fatalError("fcntl");
			}
			handleClient(clntSock);
		}
	}
}

int main() {
	Server	server(8080);
	// std::cout << "listen() success" << std::endl;
	server.run();

	return (0);
}
