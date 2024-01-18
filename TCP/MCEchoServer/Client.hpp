#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define RCVBUFSIZE 32

class Client {
 private:
	 int					socket_;
	 size_t					messageSize_;
	 struct sockaddr_in		serverAddr_;

 public:
	 Client(const std::string& serverIP, unsigned short serverPort);
	 ~Client();

	 void	connectToServer();
	 void	sendMessage(const std::string& message);
	 void	receiveMessage();
};

#endif  // CLIENT_HPP
