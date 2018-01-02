#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h> 
#include <thread>
#include <unordered_set>
#include <signal.h>

#include "server.h"
#include "client.h"
#include "game.h"

#define BUFLEN 20

using namespace std;

// client sockets
std::unordered_set<int> clientFds;

// handles SIGINT
// sends data to clientFds excluding fd
void sendToAllBut(int fd, char * buffer, int count);

// converts cstring to port
uint16_t readPort(char * txt);

int main(int argc, char ** argv){
	if(argc != 2) {
		Server::init(); 
	}
	else{
		Server::init(readPort(argv[1]));
	}
	signal(SIGINT, Server::closeAllSocketsAndExit);
	signal(SIGPIPE, SIG_IGN);

	Game::init();
	Game::printBoard();
	
	Server::startListening();

	while(true) {
		Server::handleSocketEvents();
	}
		/*
		char buffer[255];
		int count = read(clientFd, buffer, 255);
		
		if(count < 1) {
			printf("removing %d\n", clientFd);
			clientFds.erase(clientFd);
			close(clientFd);
			continue;
		} else {
			// broadcast the message
			sendToAllBut(clientFd, buffer, count);
		}
	*/	
	
	
}

uint16_t readPort(char * txt){
	char * ptr;
	auto port = strtol(txt, &ptr, 10);
	if(*ptr!=0 || port<1 || (port>((1<<16)-1))) error(1,0,"illegal argument %s", txt);
	return port;
}
