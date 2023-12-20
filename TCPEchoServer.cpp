#include "TCPEchoServer.hpp"

TCPServer::TCPServer(unsigned short port) {
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        DieWithError("socket() failed");
    }

    struct sockaddr_in echoServAddr;
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(port);

    if (bind(servSock, reinterpret_cast<struct sockaddr *>(&echoServAddr), sizeof(echoServAddr)) < 0) {
        DieWithError("bind() failed");
    }

    if (listen(servSock, MAXPENDING) < 0) {
        DieWithError("listen() failed");
    }
}

TCPServer::~TCPServer() {
    close(servSock);
}

void TCPServer::handleClient(int clntSocket) {
    char echoBuffer[RCVBUFSIZE] = {0};
    int recvMsgSize = 0;

    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
        DieWithError("recv() failed");
    }

	// std::cout << "recved1...(server)" << std::endl;
    while (recvMsgSize > 0) {
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize) {
            DieWithError("send() failed");
        }
		// std::cout << "sent...(server)" << std::endl;
        memset(echoBuffer, 0, RCVBUFSIZE);
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
            DieWithError("recv() failed");
        }
		// std::cout << "recved2...(server)" << std::endl;
    }
    close(clntSocket);
}

void TCPServer::startServer() {
    struct sockaddr_in echoClntAddr;
    unsigned int clntLen = 0;

	memset(&echoClntAddr, 0, sizeof(echoClntAddr));
    for (;;) {
        clntLen = sizeof(echoClntAddr);
        int clntSock = 0;
        std::cout << "listen...\n";
        if ((clntSock = accept(servSock, reinterpret_cast<struct sockaddr *>(&echoClntAddr), &clntLen)) < 0) {
            DieWithError("accept() failed");
        }
        std::cout << "Handling client " << inet_ntoa(echoClntAddr.sin_addr) << std::endl;
        handleClient(clntSock);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <Server Port>\n";
        exit(1);
    }

    unsigned short echoServPort = atoi(argv[1]);

    TCPServer server(echoServPort);
    server.startServer();

    return 0;
}
