#include "./TCPEchoServer.hpp"
#include "../../utils.hpp"

TCPServer::TCPServer(unsigned short port) : socketFd_(0) {
	this->socketFd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->socketFd_ < 0) {
		DieWithError("socket() failed");
	}

	int	flags = 0;
	if ((flags = fcntl(this->socketFd_, F_GETFL, 0)) < 0) {
		DieWithError("fcntl() failed");
	}
	flags |= O_NONBLOCK;
	if (fcntl(this->socketFd_, F_SETFL, flags) < 0) {
		DieWithError("fcntl() failed");
	}

	struct sockaddr_in	echoServAddr;
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddr.sin_port = htons(port);

	if (bind(this->socketFd_, reinterpret_cast<struct sockaddr *>(&echoServAddr), sizeof(echoServAddr)) < 0) {
		DieWithError("bind() failed");
	}
	if (listen(this->socketFd_, MAXPENDING) < 0) {
		DieWithError("listen() failed");
	}
}

TCPServer::~TCPServer() {
	close(this->socketFd_);
}

void TCPServer::startServer() {
	while (1) {
		sleep(3);
		struct sockaddr_in	echoClntAddr;
		unsigned int		clntLen(sizeof(echoClntAddr));
		int					clntSock(0);

		memset(&echoClntAddr, 0, sizeof(echoClntAddr));
		errno = 0;
		// block until receive client request
		clntSock = accept(this->socketFd_, reinterpret_cast<struct sockaddr *>(&echoClntAddr), &clntLen);
		if (clntSock < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				std::cout << "accept() timeout." << std::endl;
				continue;
			} else {
				DieWithError("accept() failed.");
			}
		} else {
			std::cout << "Handling client " << inet_ntoa(echoClntAddr.sin_addr) << std::endl;
			int	flags = 0;
			if ((flags = fcntl(clntSock, F_GETFL, 0)) < 0) {
				DieWithError("fcntl() failed");
			}
			flags |= O_NONBLOCK;
			if (fcntl(clntSock, F_SETFL, flags) < 0) {
				DieWithError("fcntl() failed");
			}
			handleTCPClient(clntSock);
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <Server Port>\n";
		exit(1);
	}

	unsigned short	echoServPort = atoi(argv[1]);

	TCPServer	server(echoServPort);
	// std::cout << "listen() success" << std::endl;
	server.startServer();

	return (0);
}
