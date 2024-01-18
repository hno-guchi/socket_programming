#ifndef SERVER_HPP
# define SERVER_HPP

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <iostream>
#include <string>
#include <cstring>

class Server {
	private:
		int							socketFd_;
		socklen_t					socketAddressLen_;  // socklen_tはコンパイラによってエラー。intの場合もある。
		const int					maxClients_;
		struct sockaddr_in			socketAddress_;
		struct pollfd				fds_[7];

	public:
		explicit Server(unsigned short port);
		~Server();
		void	run(void);
};

#endif  // SERVER_HPP
