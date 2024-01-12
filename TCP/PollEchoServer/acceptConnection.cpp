#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../../utils/utils.hpp"

int	acceptConnection(int servSock) {
	int					clntSock;
	struct sockaddr_in	echoClntAddr;
	unsigned int		clntLen;

	clntLen = sizeof(echoClntAddr);

	if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0) {
		DieWithError("accept() failed");
	}
	printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
	return (clntSock);
}
