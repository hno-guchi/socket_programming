#include "./Server.hpp"
#include "./utils.hpp"

Server::Server(unsigned short port) : socketFd_(0) {
	this->socketFd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->socketFd_ < 0) {
		DieWithError("socket() failed");
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

Server::~Server() {
	close(this->socketFd_);
}

void Server::startServer() {
	while (1) {
		struct sockaddr_in	echoClntAddr;
		unsigned int		clntLen(sizeof(echoClntAddr));
		int					clntSock(0);

		memset(&echoClntAddr, 0, sizeof(echoClntAddr));
		// block until receive client request
		clntSock = accept(this->socketFd_, reinterpret_cast<struct sockaddr *>(&echoClntAddr), &clntLen);
		if (clntSock < 0) {
			DieWithError("accept() failed.");
		}
		std::cout << "Handling client " << inet_ntoa(echoClntAddr.sin_addr) << std::endl;
		handleTCPClient(clntSock);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <Server Port>\n";
		exit(1);
	}

	unsigned short	echoServPort = atoi(argv[1]);

	Server	server(echoServPort);
	// std::cout << "listen() success" << std::endl;
	server.startServer();

	return (0);
}
