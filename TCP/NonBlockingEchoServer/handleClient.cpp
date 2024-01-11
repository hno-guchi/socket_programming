#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "../../utils/utils.hpp"

#define RCVBUFSIZE 32

void	handleClient(int clntSocket) {
	while (1) {
		char	echoBuffer[RCVBUFSIZE] = {0};
		ssize_t	recvMsgSize = 0;

		while (1) {
			sleep(2);
			recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0);
			if (recvMsgSize < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					std::cout << "No data received" << std::endl;
					errno = 0;
					recvMsgSize = 0;
					continue;
				} else {
					DieWithError("recv() failed 1");
				}
			} else {
				break;
			}
		}
		if (recvMsgSize == 0) {
			break;
		}
		while (1) {
			ssize_t	sendMsgSize = 0;
			sendMsgSize = send(clntSocket, echoBuffer, recvMsgSize, 0);
			if (sendMsgSize < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					std::cout << "send() timeout" << std::endl;
					errno = 0;
					sendMsgSize = 0;
					continue;
				} else {
					DieWithError("send() failed");
				}
			} else if (sendMsgSize != recvMsgSize) {
				DieWithError("send() failed");
			} else {
				break;
			}
		}
	}
	std::cout << "Client disconnected" << std::endl;
	close(clntSocket);
}
