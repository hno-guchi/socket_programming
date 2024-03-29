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
		int		recvMsgSize = 0;

		if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
			DieWithError("recv() failed 1");
		}
		if (recvMsgSize == 0) {
			break;
		}
		if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize) {
			DieWithError("send() failed");
		}
		break;
	}
	close(clntSocket);
}
