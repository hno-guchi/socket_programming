#ifndef TCPECHOCLIENT_HPP
# define TCPECHOCLIENT_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define RCVBUFSIZE 32

class TCPClient {
 private:
    int					sock;
    struct sockaddr_in	serverAddr;

 public:
    TCPClient(const char *serverIP, unsigned short serverPort);
    ~TCPClient();

    void	connectToServer();
    void	sendMessage(const char *message);
    void	receiveMessage();
};

void	DieWithError(const char *errorMessage);

#endif  // TCPECHOCLIENT_HPP
