#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

#define PORT 8080
#define MAX_CLIENTS 5

int	main(void) {
	int					server_fd = 0;
	struct sockaddr_in	address;
	int					addrlen = sizeof(address);

	// Server::Server();
	// サーバーソケットの作成
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// サーバーソケットをアドレスにバインド
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	// サーバーソケットをリスニング
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	// Server::run();
	printf("サーバーが %d ポートでリスニングしています...\n", PORT);

	int	client_sockets[MAX_CLIENTS] = {-1};
	struct pollfd	fds[MAX_CLIENTS + 1];

	// サーバーソケットを初期化
	fds[0].fd = server_fd;
	fds[0].events = POLLIN;

	// クライアントソケットを初期化
	for (int i = 1; i <= MAX_CLIENTS; ++i) {
		// client_sockets[i - 1] = -1;
		fds[i].fd = -1;
		fds[i].events = POLLIN;
	}

	while (1) {
		// poll()を使用してソケットの変更を待機
		int	result = poll(fds, MAX_CLIENTS + 1, -1);
		int	new_socket = -1;

		if (result == -1) {
			perror("poll");
			exit(EXIT_FAILURE);
		}
		// サーバーソケットに新しい接続があるか確認
		if (fds[0].revents & POLLIN) {
			if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}
			// 新しいクライアントソケットを検出し、fdsおよびclient_socketsに追加
			for (int i = 1; i <= MAX_CLIENTS; ++i) {
				if (client_sockets[i - 1] == -1) {
					client_sockets[i - 1] = new_socket;
					fds[i].fd = new_socket;
					printf("新しいクライアントが接続しました。ソケット %d\n", new_socket);
					break;
				}
			}
		}
		// クライアントソケットにデータがあるか確認
		for (int i = 1; i <= MAX_CLIENTS; ++i) {
			if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
				char	buffer[1024] = {0};
				int		valread = read(fds[i].fd, buffer, sizeof(buffer));

				if (valread == 0) {
					// クライアントが切断された場合
					printf("クライアントソケット %d が切断されました。\n", fds[i].fd);
					close(fds[i].fd);
					client_sockets[i - 1] = -1;
					fds[i].fd = -1;
				} else {
					// クライアントからのデータを処理
					printf("クライアントソケット %d からのメッセージ: %s\n", fds[i].fd, buffer);
					// Send a response back to the client
					char response[] = "Server received your message.";
					send(fds[i].fd, response, strlen(response), 0);
					printf("クライアントソケット %d を切断しました。\n", fds[i].fd);
					close(fds[i].fd);
					client_sockets[i - 1] = -1;
					fds[i].fd = -1;
				}
			}
		}
	}
	return (0);
}

