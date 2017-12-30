#ifndef __SERVER_H_INCLUDED__
#define __SERVER_H_INCLUDED__

#include <poll.h>
#include <list>

class Client;

class Server{
private:
	static int sock;
	static int port;
	static pollfd whatToWaitFor[10];
	static int numberOfSockets;
	static std::list<Client*> clients;

	static void setServerSocket();
	static void setReuseAddr();
	static void bindSocket();
	static void addNewClient();
	static void readMessage(int clientSocket);
	static void checkIfDeadClient(int clientSocket);
public:
	static void setUp(int port = 56789);
	static void startListening();
	static void handleSocketEvents();
	static int getSocket();
};
#endif
