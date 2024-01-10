###########
# General #
###########

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP
DEBUGFLAGS = -g -fsanitize=undefined -fsanitize=integer -fsanitize=address -DDEBUG

#################
# General Rules #
#################

.PHONY: clean
clean:
	$(RM) -r server.d
	$(RM) -r client.d

.PHONY: fclean
fclean: clean
	$(RM) -r server
	$(RM) -r client

.PHONY: echo
echo:
	$(CXX) $(CXXFLAGS) ./TCP/EchoServer/Server.cpp ./TCP/EchoServer/handleClient.cpp ./utils/DieWithError.cpp -o server
	$(CXX) $(CXXFLAGS) ./TCP/EchoServer/Client.cpp ./utils/DieWithError.cpp -o client
	./server 12345 &
	./client 127.0.0.1 "Hello World" 12345 &
