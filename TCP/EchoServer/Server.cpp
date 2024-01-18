#include "./Server.hpp"
#include "../utils/utils.hpp"

Server::Server(unsigned short port) :
	socketFd_(0), maxPending_(5) {
	this->socketFd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->socketFd_ < 0) {
		fatalError("socket");
	}

	struct sockaddr_in	echoServAddr;
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddr.sin_port = htons(port);

	if (bind(this->socketFd_, reinterpret_cast<struct sockaddr *>(&echoServAddr), sizeof(echoServAddr)) < 0) {
		fatalError("bind");
	}
	if (listen(this->socketFd_, this->maxPending_) < 0) {
		fatalError("listen");
	}
}

Server::~Server() {
	close(this->socketFd_);
}

static void	handleClient(int clientSocket) {
	while (1) {
		char	buf[RECV_BUF_SIZE] = {0};
		int		recvMsgSize = 0;

		// blocking
		if ((recvMsgSize = recv(clientSocket, buf, RECV_BUF_SIZE, 0)) < 0) {
			fatalError("recv");
		}
		if (recvMsgSize == 0) {
			break;
		}
		if (send(clientSocket, buf, recvMsgSize, 0) != recvMsgSize) {
			fatalError("send");
		}
	}
	close(clientSocket);
}

void	Server::run() {
	while (1) {
		struct sockaddr_in	clientAddr;
		unsigned int		clientLen(sizeof(clientAddr));
		int					clientSocket(0);

		memset(&clientAddr, 0, sizeof(clientAddr));
		// block until receive client request
		clientSocket = accept(this->socketFd_, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientLen);
		if (clientSocket < 0) {
			fatalError("accept");
		}
		std::cout << "Handling client " << inet_ntoa(clientAddr.sin_addr) << std::endl;
		handleClient(clientSocket);
	}
}

int main() {
	Server	server(8080);

	server.run();

	return (0);
}
