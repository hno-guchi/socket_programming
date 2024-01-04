#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int	main(void) {
	struct sockaddr		sa;
	struct sockaddr_in	sa_in;

	bzero(&sa, sizeof(sa));
	bzero(&sa_in, sizeof(sa_in));

	dprintf(STDOUT_FILENO, "[sockaddr]______________________________\n");
	dprintf(STDOUT_FILENO, "sizeof(sockaddr)                 : [%ld]\n", sizeof(sa));
	dprintf(STDOUT_FILENO, "sizeof(sockaddr->sa_len)         : [%ld]\n", sizeof(sa.sa_len));
	dprintf(STDOUT_FILENO, "sizeof(sockaddr->sa_family)      : [%ld]\n", sizeof(sa.sa_family));
	dprintf(STDOUT_FILENO, "sizeof(sockaddr->sa_data[14])    : [%ld]\n", sizeof(sa.sa_data));
	dprintf(STDOUT_FILENO, "________________________________________\n\n");

	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(4242);
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);

	dprintf(STDOUT_FILENO, "[sockaddr_in]___________________________\n");
	dprintf(STDOUT_FILENO, "sockaddr_in              | sizeof() [%ld]\n", sizeof(sa_in));
	dprintf(STDOUT_FILENO, "sockaddr_in->sin_len     | sizeof() [%ld] | value [%d]\n", sizeof(sa_in.sin_len), sa_in.sin_len);
	dprintf(STDOUT_FILENO, "sockaddr_in->sin_family  | sizeof() [%ld] | value [%d]\n", sizeof(sa_in.sin_family), sa_in.sin_family);
	dprintf(STDOUT_FILENO, "sockaddr_in->sin_port    | sizeof() [%ld] | value [%d]\n", sizeof(sa_in.sin_port), sa_in.sin_port);
	dprintf(STDOUT_FILENO, "sockaddr_in->sin_addr    | sizeof() [%ld] | value [%d]\n", sizeof(sa_in.sin_addr), sa_in.sin_addr.s_addr);
	dprintf(STDOUT_FILENO, "sockaddr_in->sin_zero[8] | sizeof() [%ld] | value [%s]\n", sizeof(sa_in.sin_zero), sa_in.sin_zero);
	dprintf(STDOUT_FILENO, "________________________________________\n");

	struct sockaddr	*sa2;

	sa2 = (struct sockaddr *)&sa_in;
	dprintf(STDOUT_FILENO, "sa2->sa_family [%d]\n", sa2->sa_family);
	// write(STDOUT_FILENO, sa2->sa_data, 2);
	return (0);
}
