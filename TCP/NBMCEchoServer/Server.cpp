#include "./Server.hpp"
#include "../utils/utils.hpp"

Server::Server(unsigned short port) :
	socketFd_(0), socketAddressLen_(sizeof(this->socketAddress_)), maxClients_(5) {
	// サーバーソケットの作成
	this->socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketFd_ < 0) {
		fatalError("socket");
	}

	int	flags = 0;
	if ((flags = fcntl(this->socketFd_, F_GETFL, 0)) < 0) {
		fatalError("fcntl");
	}
	flags |= O_NONBLOCK;
	if (fcntl(this->socketFd_, F_SETFL, flags) < 0) {
		fatalError("fcntl");
	}

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

	// サーバーソケットを初期化
	this->fds_[0].fd = this->socketFd_;
	this->fds_[0].events = POLLIN;
	this->fds_[0].revents = 0;
	// 各クライアントに対応したサーバーソケットを保存する配列を初期化
	for (int i = 1; i <= this->maxClients_; ++i) {
		this->fds_[i].fd = -1;
		this->fds_[i].events = POLLIN;
		this->fds_[i].revents = 0;
	}
	// STDIN_FILENOは、ノンブロッキングにすべき？
	this->fds_[this->maxClients_].fd = STDIN_FILENO;
	this->fds_[this->maxClients_].events = POLLIN;
	this->fds_[this->maxClients_].revents = 0;
}

Server::~Server() {
	close(this->socketFd_);
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
		int	newSocket = -1;
		// サーバーソケットに新しい接続があるか確認
		if (this->fds_[0].revents & POLLIN) {
			newSocket = accept(this->socketFd_, reinterpret_cast<struct sockaddr *>(&this->socketAddress_), &this->socketAddressLen_);
			if (newSocket < 0) {
					close(newSocket);
					fatalError("accept");
				}
			int	flags = 0;
			if ((flags = fcntl(newSocket, F_GETFL, 0)) < 0) {
				fatalError("fcntl");
			}
			flags |= O_NONBLOCK;
			if (fcntl(newSocket, F_SETFL, flags) < 0) {
				fatalError("fcntl");
			}
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

					while (1) {
						sleep(2);
						recvMsgSize = recv(this->fds_[i].fd, buffer, sizeof(buffer), MSG_DONTWAIT);
						if (recvMsgSize < 0) {
							if (errno == EAGAIN || errno == EWOULDBLOCK) {
								std::cout << "No data received." << std::endl;
								errno = 0;
								recvMsgSize = 0;
								continue;
							} else {
								// std::cerr << errno << std::endl;
								if (errno == ECONNRESET) {
									recvMsgSize = 0;
									break;
								}
								fatalError("recv");
							}
						} else {
							break;
						}
					}
					// クライアントが切断された場合
					if (recvMsgSize == 0) {
						std::cout << "クライアントソケット " << this->fds_[i].fd << " が切断されました。" << std::endl;
						close(this->fds_[i].fd);
						this->fds_[i].fd = -1;
						break;
					}
					// クライアントからのデータを処理
					std::cout << "クライアントソケット " << this->fds_[i].fd << " からのメッセージ: " << buffer << std::endl;
					while (1) {
						// クライアントに応答
						ssize_t	sendMsgSize = 0;

						sendMsgSize = send(this->fds_[i].fd, buffer, strlen(buffer), 0);
						if (sendMsgSize < 0) {
							if (errno == EAGAIN || errno == EWOULDBLOCK) {
								std::cout << "No data sent." << std::endl;
								errno = 0;
								sendMsgSize = 0;
								continue;
							} else {
								fatalError("send");
							}
						} else if (sendMsgSize != recvMsgSize) {
							fatalError("send");
						} else {
							break;
						}
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
