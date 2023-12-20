###########
# General #
###########

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP
DEBUGFLAGS = -g -fsanitize=undefined -fsanitize=integer -fsanitize=address -DDEBUG

#################
# General Rules #
#################

echo:
	$(CXX) $(CXXFLAGS) TCPEchoServer.cpp DieWithError.cpp -o server
	$(CXX) $(CXXFLAGS) TCPEchoClient.cpp DieWithError.cpp -o client
	./server 12345&
	./client 127.0.0.1 "Hello World" 12345
