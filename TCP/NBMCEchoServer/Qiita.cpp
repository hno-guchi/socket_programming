#include "./Server.hpp"

static void fatalError(const std::string& message) {
	std::perror(message.c_str());
    exit(EXIT_FAILURE);
}

static void	setFdFlags(const int &fd, int setFlags) {
	int	flags = 0;

	if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
		fatalError("fcntl");
	}
	flags |= setFlags;
	if (fcntl(fd, F_SETFL, flags) < 0) {
		fatalError("fcntl");
	}
}

Server::Server(unsigned short port) :
	socketFd_(0), socketAddressLen_(sizeof(this->socketAddress_)), maxClients_(5) {
	// サーバーソケットの作成
	this->socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketFd_ < 0) {
		fatalError("socket");
	}

	setFdFlags(this->socketFd_, O_NONBLOCK);

	memset(&this->socketAddress_, 0, sizeof(this->socketAddress_));
	this->socketAddress_.sin_family = AF_INET;
	this->socketAddress_.sin_addr.s_addr = INADDR_ANY;
	this->socketAddress_.sin_port = htons(port);

	// サーバーソケットをアドレスにバインド
	if (bind(this->socketFd_, reinterpret_cast<struct sockaddr *>(&this->socketAddress_), sizeof(this->socketAddress_)) < 0) {
		fatalError("bind");
	}
	// サーバーソケットをリスニング
	if (listen(this->socketFd_, 3) < 0) {
		fatalError("listen");
	}
	std::cout << "サーバーが " << port << " ポートでリスニングしています..." << std::endl;

	// 各ソケットを初期化
	this->fds_[0].fd = this->socketFd_;
	for (int i = 1; i < this->maxClients_; ++i) {
		this->fds_[i].fd = -1;
	}
	// STDIN_FILENOは、ノンブロッキングにすべき？
	this->fds_[this->maxClients_].fd = STDIN_FILENO;
	for (int i = 0; i <= this->maxClients_; ++i) {
		this->fds_[i].events = POLLIN;
		this->fds_[i].revents = 0;
	}
}

Server::~Server() {
	close(this->socketFd_);
}

static bool	isNonBlockingError() {
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		// std::cout << "No data received." << std::endl;
		errno = 0;
		return (true);
	}
	return (false);
}

static ssize_t	recvNonBlocking(const int fd, char *buffer, size_t bufSize) {
	ssize_t		recvMsgSize = 0;

	while (1) {
		// sleep(2);
		recvMsgSize = recv(fd, buffer, bufSize, MSG_DONTWAIT);
		if (recvMsgSize >= 0) {
			break;
		}
		if (isNonBlockingError()) {
			recvMsgSize = 0;
			continue;
		} else if (errno == ECONNRESET) {
			recvMsgSize = 0;
			break;
		} else {
			// std::cerr << errno << std::endl;
			fatalError("recv");
		}
	}
	return (recvMsgSize);
}

static ssize_t	sendNonBlocking(const int fd, char *buffer, size_t bufSize) {
	ssize_t		sendMsgSize = 0;

	while (1) {
		// sleep(2);
		sendMsgSize = send(fd, buffer, bufSize, MSG_DONTWAIT);
		if (sendMsgSize >= 0) {
			break;
		}
		if (isNonBlockingError()) {
			sendMsgSize = 0;
			continue;
		} else if (errno == ECONNRESET) {
			sendMsgSize = 0;
			break;
		} else {
			// std::cerr << errno << std::endl;
			fatalError("recv");
		}
	}
	return (sendMsgSize);
}

void	Server::run() {
	while (1) {
		// poll()を使用して待機
		int	result = poll(this->fds_, this->maxClients_ + 1, 3 * 1000);
		if (result == -1) {
			fatalError("poll");
		}
		if (result == 0) {
			std::cout << "poll: Timeout 3 seconds..." << std::endl;
			continue;
		}
		// サーバーソケットに新しい接続があるか確認
		if (this->fds_[0].revents & POLLIN) {
			// createNewSocket(this->socketFd_, &this->socketAddress_, &this->socketAddressLen_));
			int	newSocket = -1;
			newSocket = accept(this->socketFd_, reinterpret_cast<struct sockaddr *>(&this->socketAddress_), &this->socketAddressLen_);
			if (newSocket < 0) {
					close(newSocket);
					fatalError("accept");
			}
			setFdFlags(newSocket, O_NONBLOCK);
			// 新しいクライアントソケットを検出し、fdsに追加
			for (int i = 1; i <= this->maxClients_; ++i) {
				if (this->fds_[i].fd == -1) {
					this->fds_[i].fd = newSocket;
					std::cout << "新しいクライアントが接続しました。ソケット " << newSocket << std::endl;
					break;
				}
			}
		} else if (this->fds_[this->maxClients_].revents & POLLIN) {
			std::string	input;
			std::getline(std::cin, input);
			if (input == "exit") {
				std::cout << "See You..." << std::endl;
				break;
			}
		}
		// クライアントソケットにデータがあるか確認
		for (int i = 1; i <= this->maxClients_; ++i) {
			if (this->fds_[i].fd != -1 && (this->fds_[i].revents & POLLIN)) {
				while (1) {
					char		buffer[1024] = {0};

					ssize_t		recvMsgSize = 0;
					recvMsgSize = recvNonBlocking(this->fds_[i].fd, buffer, sizeof(buffer));
					// クライアントとの通信が切断された場合
					if (recvMsgSize == 0) {
						// disconnectedFd(this->fds_[i].fd);
						std::cout << "クライアントソケット " << this->fds_[i].fd << " が切断されました。" << std::endl;
						close(this->fds_[i].fd);
						this->fds_[i].fd = -1;
						break;
					}
					// クライアントからのデータを処理
					std::cout << "クライアントソケット " << this->fds_[i].fd << " からのメッセージ: " << buffer << std::endl;

					ssize_t	sendMsgSize = 0;
					sendMsgSize = sendNonBlocking(this->fds_[i].fd, buffer, strlen(buffer));
					if (recvMsgSize != sendMsgSize) {
						fatalError("send");
					}
					// クライアントとの通信が切断された場合
					if (sendMsgSize == 0) {
						// disconnectedFd(this->fds_[i].fd);
						std::cout << "クライアントソケット " << this->fds_[i].fd << " が切断されました。" << std::endl;
						close(this->fds_[i].fd);
						this->fds_[i].fd = -1;
					}
					break;
				}
			}
		}
	}
}

int	main() {
	Server	Server(8080);

	Server.run();
	return (0);
}
