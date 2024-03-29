#include "./Server.hpp"
#include "../utils/utils.hpp"

Server::Server(unsigned short port) :
	socketFd_(0), socketAddressLen_(sizeof(this->socketAddress_)), maxClients_(5) {
	// サーバーソケットの作成
	this->socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketFd_ < 0) {
		fatalError("socket");
	}

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

	// 各クライアントに対応したサーバーソケットを保存する配列を初期化
	for (int i = 1; i <= this->maxClients_; ++i) {
		this->fds_[i].fd = -1;
		this->fds_[i].events = POLLIN;
	}
}

Server::~Server() {
	close(this->socketFd_);
}

void	Server::run() {
	while (1) {
		// poll()を使用してソケットの変更を待機
		int	result = poll(this->fds_, this->maxClients_ + 1, -1);
		int	newSocket = -1;

		if (result == -1) {
			fatalError("poll");
		}
		if (result == 0) {
			continue;
		}
		// サーバーソケットに新しい接続があるか確認
		if (this->fds_[0].revents & POLLIN) {
			if ((newSocket = accept(this->socketFd_, reinterpret_cast<struct sockaddr *>(&this->socketAddress_), &this->socketAddressLen_)) < 0) {
				close(newSocket);
				fatalError("accept");
			}
			// 新しいクライアントソケットを検出し、fdsに追加
			for (int i = 1; i <= this->maxClients_; ++i) {
				if (this->fds_[i].fd == -1) {
					this->fds_[i].fd = newSocket;
					std::cout << "新しいクライアントが接続しました。ソケット " << newSocket << std::endl;
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
