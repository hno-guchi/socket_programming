#ifndef SERVER_HPP
# define SERVER_HPP

#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define MAXPENDING 5
#define RCVBUFSIZE 32

class Server {
 private:
	 int	*serverSocket_;
	 int	maxNumDescriptor_;
	 int	countPorts_;

 public:
	 explicit Server(int numPorts, char **argv);
	 ~Server();

	 void startServer(long timeout);
};

int		createSocket(unsigned short port);
void	handleClient(int clntSocket);
int		acceptConnection(int servSock);
void	DieWithError(const char *errorMessage);

#endif  // SERVER_HPP
