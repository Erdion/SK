#ifndef __CLIENT_H_INCLUDED__
#define __CLIENT_H_INCLUDED__

class Client{
private:
	int sock;
	int index;
public:
	Client(int index);

	int getSocket();
	bool isConnected();
};
#endif
