#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <poll.h> 

#include "server.h"
#include "client.h"

#define BUFLEN 20

using namespace std;

Client::Client(int index){
	sockaddr_in clientAddr{0};
	socklen_t clientAddrSize = sizeof(clientAddr);
		
	// accept new connection
	sock = accept(Server::getSocket(), (sockaddr*) &clientAddr, &clientAddrSize);
	if(sock == -1){
		error(1, errno, "accept failed");
	}
}

int Client::getSocket(){
	return sock;
}

bool Client::isConnected(){
	char buf[BUFLEN] = "ControlMessage";
	if(write(sock, buf, BUFLEN) == -1){
		return false;
	}
	return true;
}
