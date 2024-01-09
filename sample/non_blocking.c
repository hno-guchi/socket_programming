#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int	main(void) {
    // ファイルディスクリプタを非ブロッキングモードに設定
	int	flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

	while (1) {
		char	buffer[1024];
		ssize_t	bytesRead;
		errno = 0;

		// 非ブロッキングモードでの読み取り
		bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));

		if (bytesRead == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			perror("read");
		} else if (bytesRead > 0) {
			// データが読み取られた場合
			printf("Read %zd bytes: %.*s\n", bytesRead, (int)bytesRead, buffer);
		} else {
			// データがない場合
			printf("No data available.\n");
			break ;
		}
		sleep(3);
	}
	return (0);
}
