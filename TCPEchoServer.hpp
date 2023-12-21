#ifndef TCPECHOSERVER_HPP
# define TCPECHOSERVER_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define MAXPENDING 5
#define RCVBUFSIZE 32

class TCPServer {
 private:
    int servSock;

 public:
    explicit TCPServer(unsigned short port);
    ~TCPServer();

    void startServer();
};

void DieWithError(const char *errorMessage);

#endif  // TCPECHOSERVER_HPP
