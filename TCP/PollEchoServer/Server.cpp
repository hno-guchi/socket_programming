#include "./Server.hpp"

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
	struct pollfd*	fds = new struct pollfd[this->countPorts_ + 1];
	int				running = 1;

	printf("Starting server: Hit return to shutdown\n");
	while (running) {
		// Initialize pollfd structures
		for (int i = 0; i < this->countPorts_; i++) {
			fds[i].fd = this->serverSocket_[i];
			fds[i].events = POLLIN;
		}
		fds[this->countPorts_].fd = STDIN_FILENO;
		fds[this->countPorts_].events = POLLIN;

		// blocking
		int result = poll(fds, this->countPorts_ + 1, timeout * 1000);

		if (result == 0) {
			printf("No echo requests for %ld secs... Server still alive\n", timeout);
		} else if (result > 0) {
			if (fds[this->countPorts_].revents & POLLIN) {
				printf("Shutting down server\n");
				getchar();
				running = 0;
			}

			for (int i = 0; i < this->countPorts_; i++) {
				if (fds[i].revents & POLLIN) {
					printf("Request on port %d: ", this->serverSocket_[i]);
					handleClient(acceptConnection(this->serverSocket_[i]));
				}
			}
		} else {
			perror("poll");
			running = 0;
		}
	}
	delete[] fds;
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
