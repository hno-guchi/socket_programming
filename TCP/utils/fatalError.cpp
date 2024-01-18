#include <iostream>

void fatalError(const std::string& message) {
	std::perror(message.c_str());
    exit(EXIT_FAILURE);
}

// int main() {
// 	fatalError("This is a fatal error");
// }
