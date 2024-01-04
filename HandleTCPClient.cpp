#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "./utils.hpp"

#define RCVBUFSIZE 32

void	handleTCPClient(int clntSocket) {
	char	echoBuffer[RCVBUFSIZE] = {0};
	int		recvMsgSize = 0;

	if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
		DieWithError("recv() failed");
	}
	while (recvMsgSize > 0) {
		if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize) {
			DieWithError("send() failed");
		}
		memset(echoBuffer, 0, RCVBUFSIZE);
		if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
			DieWithError("recv() failed");
		}
	}
	close(clntSocket);
}
