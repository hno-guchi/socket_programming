#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define RCVBUFSIZE 32

class Client {
 private:
	 int					socket_;
	 struct sockaddr_in		serverAddr_;

 public:
	 Client(const char *serverIP, unsigned short serverPort);
	 ~Client();

	 void	connectToServer();
	 void	sendMessage(const char *message);
	 void	receiveMessage();
};

void	DieWithError(const char *errorMessage);

#endif  // CLIENT_HPP
