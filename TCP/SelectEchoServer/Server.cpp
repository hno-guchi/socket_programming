#include "./Server.hpp"
#include <sys/time.h>
#include <fcntl.h>

Server::Server(int numPorts, char **argv) :
	serverSocket_(NULL), maxNumDescriptor_(-1), countPorts_(numPorts) {
	this->serverSocket_ = new int[this->countPorts_];

	for (int i = 0; i < this->countPorts_; i++) {
		unsigned short	port = atoi(argv[i]);
		this->serverSocket_[i] = createSocket(port);

		if (this->serverSocket_[i] > this->maxNumDescriptor_) {
			this->maxNumDescriptor_ = this->serverSocket_[i];
		}
	}
}

Server::~Server() {
	for (int i = 0; i < this->countPorts_; i++) {
		close(this->serverSocket_[i]);
	}
	delete[] this->serverSocket_;
}

void	Server::startServer(long timeout) {
	fd_set			sockSet = {0};
	struct timeval	selTimeout;
	int				running = 1;

	printf("Starting server: Hit return to shutdown\n");
	while (running) {
		FD_ZERO(&sockSet);
		FD_SET(STDIN_FILENO, &sockSet);
		for (int i = 0; i < this->countPorts_; i++) {
			FD_SET(this->serverSocket_[i], &sockSet);
		}
		selTimeout.tv_sec = timeout;
		selTimeout.tv_usec = 0;

		// blocking
		if (select(this->maxNumDescriptor_ + 1, &sockSet, NULL, NULL, &selTimeout) == 0) {
			printf("No echo requests for %ld secs... Server still alive\n", timeout);
		} else {
			if (FD_ISSET(STDIN_FILENO, &sockSet)) {
				printf("Shutting down server\n");
				getchar();
				running = 0;
			}

			for (int i = 0; i < this->countPorts_; i++) {
				if (FD_ISSET(this->serverSocket_[i], &sockSet)) {
					printf("Request on port %d: ", this->serverSocket_[i]);
					handleClient(acceptConnection(this->serverSocket_[i]));
				}
			}
		}
	}
}

int	main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Usage:  %s <Timeout (secs.)> <Port 1> ... \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	Server	server(argc - 2, &argv[2]);
	server.startServer(atol(argv[1]));

	return (0);
}
