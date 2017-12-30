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
#include <list>

#include "server.h"
#include "client.h"

#define BUFLEN 20

int Server::sock;
int Server::port;
pollfd Server::whatToWaitFor[10]{};
int Server::numberOfSockets;
std::list<Client*> Server::clients;

void Server::setServerSocket(){
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1) {
		error(1, errno, "socket failed");
	}
}

void Server::setReuseAddr(){
	const int one = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	if(res){
		error(1,errno, "setsockopt failed");
	}
}

void Server::bindSocket(){
	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	int res = bind(sock, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res){
		error(1, errno, "bind failed");
	}
}

void Server::addNewClient(){
	int index = numberOfSockets;
	numberOfSockets++;
	Client* client = new Client(index);
	whatToWaitFor[index].fd = client->getSocket();
	whatToWaitFor[index].events = POLLIN;
	clients.push_back(client);
}


void Server::readMessage(int clientSocket){
	char buffer[BUFLEN]{};
	int rec = read(clientSocket, buffer, 255);
	write(0, buffer, rec);
}

void Server::checkIfDeadClient(int clientSocket){
	char buffer[BUFLEN]{};
	if(write(clientSocket, buffer, BUFLEN) == -1){ //TODO, nie mam pojecia jak to ogarnac na razie :P
		write(0, "dead", 4); 
	}
}

void Server::setUp(int port){
	Server::port = port;
	setServerSocket();
	setReuseAddr();
	bindSocket();
}

void Server::startListening(){
	int res = listen(sock, 1);
	if(res){
		error(1, errno, "listen failed");
	}
	for(pollfd &desc: whatToWaitFor){
		desc.fd = 0;
		desc.events = 0;
	}
	whatToWaitFor[0].fd = sock;
	whatToWaitFor[0].events = POLLIN;
	numberOfSockets = 1;
}

void Server::handleSocketEvents(){
	// read a message
	int ready = poll(whatToWaitFor, numberOfSockets, -1);
	for(int i = 0; i < numberOfSockets; i++){
		pollfd desc = whatToWaitFor[i];
		if(desc.fd == sock && desc.revents == POLLIN){
			write(0, "a", 1);
			addNewClient();
		}
		else if(desc.revents == POLLIN) {
			write(0, "b", 1);
			readMessage(desc.fd);
			checkIfDeadClient(desc.fd);
		}
		else {
			write(0, "c", 1);
			checkIfDeadClient(desc.fd);
		}			
	}
}

int Server::getSocket(){
	return sock;
}
