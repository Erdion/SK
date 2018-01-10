#ifndef __SERVER_H_INCLUDED__
#define __SERVER_H_INCLUDED__

#include <poll.h>
#include <list>
#include <string>
#include <algorithm>

class Server{
private:
	static int sock;
	static int port;
	static pollfd whatToWaitFor[10];
	static int numberOfSockets;

	static bool listContains(std::list<int> list, int item); //C++ lists don't have contains method :C

	static void setServerSocket();
	static void setReuseAddr();
	static void bindSocket();
	static void addNewClient();
	static std::string readMessage(int clientSocket);
	static void sendMessage(std::string message, int clientSocket);
	static void broadcastMessage(std::string message, std::list<int> ignoredSockets);
	static void removeDeadSockets(std::list<int> failedSocketIndexes);

	class DeadSocketException{
	private:
		std::string message;
	public:
		DeadSocketException(std::string message);
	};
public:
	static void init(int port = 56789);
	static void startListening();
	static void handleSocketEvents();
	static int getSocket();
	static void closeAllSocketsAndExit(int);
};
#endif
