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
echo: fclean
echo:
	$(CXX) $(CXXFLAGS) ./TCP/EchoServer/Server.cpp ./TCP/utils/fatalError.cpp -o server
	$(CXX) $(CXXFLAGS) ./TCP/EchoServer/Client.cpp ./TCP/utils/fatalError.cpp -o client
	./server
	# ./client "Hello World"

.PHONY: non_block
non_block:
	$(CXX) $(CXXFLAGS) ./TCP/NonBlockingEchoServer/Server.cpp ./TCP/utils/fatalError.cpp -o server
	$(CXX) $(CXXFLAGS) ./TCP/NonBlockingEchoServer/Client.cpp ./TCP/utils/fatalError.cpp -o client
	./server
	# ./client "Hello World" &

.PHONY: select
select: fclean
select:
	$(CXX) $(CXXFLAGS) ./TCP/SelectEchoServer/Server.cpp ./TCP/SelectEchoServer/acceptConnection.cpp ./TCP/SelectEchoServer/createSocket.cpp ./TCP/SelectEchoServer/handleClient.cpp ./utils/DieWithError.cpp -o server
	$(CXX) $(CXXFLAGS) ./TCP/SelectEchoServer/Client.cpp ./utils/DieWithError.cpp -o client
	./server 7 3 4 5 6

.PHONY: poll
poll: fclean
poll:
	$(CXX) $(CXXFLAGS) ./TCP/PollEchoServer/Server.cpp ./TCP/PollEchoServer/acceptConnection.cpp ./TCP/PollEchoServer/createSocket.cpp ./TCP/PollEchoServer/handleClient.cpp ./utils/DieWithError.cpp -o server
	$(CXX) $(CXXFLAGS) ./TCP/PollEchoServer/Client.cpp ./utils/DieWithError.cpp -o client
	./server 7 3 4 5 6
