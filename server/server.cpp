#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
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
#include "game.h"

#define BUFLEN 256
#define TIMEOUT 34

int Server::sock;
int Server::port;
pollfd Server::whatToWaitFor[10]{};
int Server::numberOfSockets;

bool Server::listContains(std::list<int> list, int item){
	return std::find(list.begin(), list.end(), item) != list.end();
}

void Server::setServerSocket(){
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1) {
		error(1, errno, "socket failed");
	}
	struct timeval tv;
	tv.tv_sec = 2; //timeout
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
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
}


std::string Server::readMessage(int clientSocket){
	char buffer[BUFLEN]{};
	int rec;
	std::string message;
	while((rec = recv(clientSocket, buffer, BUFLEN, 0)) > 0){
		if(rec <= 0){
			throw DeadSocketException("Read failed");
		}
		
		std::string temp(buffer);
		message += temp;
		if(message[message.length() - 1] == 'E')
			break;
	}
	return message;
}

void Server::sendMessage(std::string message, int clientSocket){
	char buffer[BUFLEN]{};
	while(message.length() > 0){
		int lengthToSend = message.length();
		if(lengthToSend > BUFLEN - 1){
			lengthToSend = BUFLEN - 1;
		}
		strcpy(buffer, message.substr(0, lengthToSend).c_str());
		message = message.substr(lengthToSend);
		buffer[lengthToSend + 1] = '\0';
		int rec = write(clientSocket, buffer, lengthToSend);
		if(rec == -1){
			throw DeadSocketException("Write failed");
		}
		if(rec != lengthToSend){
			throw DeadSocketException("Written less than requested");
		}
	}
}

void Server::broadcastMessage(std::string message, std::list<int> ignoredSockets){
	for(int i = 0; i < numberOfSockets; i++){
		pollfd desc = whatToWaitFor[i];
		if(!listContains(ignoredSockets, desc.fd)){
			sendMessage(message, desc.fd);
		}
	}
}

void Server::removeDeadSockets(std::list<int> failedSocketIndexes){
	failedSocketIndexes.sort();
	int countRemovedIndexes = 0;
	for(int &index: failedSocketIndexes){
		Game::removeParticipant(index - countRemovedIndexes);
		close(whatToWaitFor[index - countRemovedIndexes].fd);
		whatToWaitFor[index - countRemovedIndexes].fd = 0;
		whatToWaitFor[index - countRemovedIndexes].events = 0;

		for(int i = index; i < numberOfSockets - 1; i++){
			int prev = i - countRemovedIndexes + 1;
			int next = i - countRemovedIndexes;
			whatToWaitFor[next].fd = whatToWaitFor[prev].fd; 
			whatToWaitFor[next].events = whatToWaitFor[prev].events; 
			whatToWaitFor[prev].fd = 0;
			whatToWaitFor[prev].events = 0;
			Game::changeIndex(prev, next);
		}
		countRemovedIndexes++;
	}
	numberOfSockets -= countRemovedIndexes;
}

Server::DeadSocketException::DeadSocketException(std::string message) : message(message){
	
}

void Server::init(int port){
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
	std::list<int> failedSocketIndexes;

	int timeout = Game::timeUntilPerish();
	int ready = poll(whatToWaitFor, numberOfSockets, timeout);
	
	if(ready == -1){
		error(1, errno, "poll failed");
	}
	
	int numberOfSocketsBeforeEvents = numberOfSockets;
	for(int i = 0; i < numberOfSocketsBeforeEvents; i++){
		try {
			pollfd desc = whatToWaitFor[i];
			printf("%d", desc.fd);
			if(desc.fd == sock && desc.revents == POLLIN){
				write(0, "a", 1);
				addNewClient();
				Game::initParticipant(numberOfSockets - 1);
			}
			else if(desc.revents == POLLIN) {
				write(0, "b", 1);
				std::string message = readMessage(desc.fd);
				Game::interpretMessage(message, i);
			}
			else if(desc.revents & POLLERR){ //disconnected socket sometimes returns not only POLLERR, but a value where other bits are also set (very often POLLHUP & POLLERR)
				write(0, "d", 1);
				throw DeadSocketException("Dead socket");
			}
			else {
				write(0, "c", 1);
			}			
		}
		catch(const DeadSocketException &e){
			failedSocketIndexes.push_back(i);
			write(0, "dead", 4);
		}
		fflush(stdout);
	}
	printf("\n");

	Game::explodeDueBombs();
	Game::extinguishDueFlames();

	removeDeadSockets(failedSocketIndexes);

	Game::handleGameEnd();

	std::list<int> ignoredBroadcastSockets;
	ignoredBroadcastSockets.push_back(sock);

	broadcastMessage(Game::getBoardString(), ignoredBroadcastSockets); 

	fflush(stdout);
}

int Server::getSocket(){
	return sock;
}

void Server::closeAllSocketsAndExit(int){
	for(int i = 0; i < numberOfSockets; i++){
		close(whatToWaitFor[i].fd);
		whatToWaitFor[i].fd = 0;
		whatToWaitFor[i].events = 0;
	}
	close(sock);
	numberOfSockets = 0;
	printf("Server closed\n");
	exit(0);
}
