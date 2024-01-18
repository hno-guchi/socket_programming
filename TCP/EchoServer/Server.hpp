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

# define RECV_BUF_SIZE 32

class Server {
 private:
	 int		socketFd_;
	 const int	maxPending_;

 public:
	 explicit Server(unsigned short port);
	 ~Server();

	 void run();
};

#endif  // SERVER_HPP
