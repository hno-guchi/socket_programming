###########
# General #
###########

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP
DEBUGFLAGS = -g -fsanitize=undefined -fsanitize=integer -fsanitize=address -DDEBUG

#################
# General Rules #
#################

.PHONY: all
all:
	$(CXX) $(CXXFLAGS) ./Server.cpp ../utils/fatalError.cpp -o server
	$(CXX) $(CXXFLAGS) ./Client.cpp ../utils/fatalError.cpp -o client

.PHONY: clean
clean:
	$(RM) -r server.d
	$(RM) -r server.dSYM
	$(RM) -r client.d
	$(RM) -r client.dSYM

.PHONY: fclean
fclean: clean
	$(RM) -r server
	$(RM) -r client

.PHONY: qiita
qiita: fclean
qiita:
	$(CXX) $(CXXFLAGS) ./Qiita.cpp -o server
	$(CXX) $(CXXFLAGS) ./Client.cpp ../utils/fatalError.cpp -o client
