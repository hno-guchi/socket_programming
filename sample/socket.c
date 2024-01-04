#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
// #include <sys/types.h>

/*
 * PF_INET: IPv4
 * SOCK_STREAM: ストリーム型ソケット
 */
int	main(void) {
	int	sock_fd;

	sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		dprintf(STDERR_FILENO, "socket error\n");
		return (1);
	}
	dprintf(STDOUT_FILENO, "sock_fd : [%d]\n", sock_fd);
	close(sock_fd);
	return (0);
}
