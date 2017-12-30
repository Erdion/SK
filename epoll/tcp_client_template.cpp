#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h> 
#include <thread>

ssize_t readData(int fd, char * buffer, ssize_t buffsize){
	auto ret = read(fd, buffer, buffsize);
	if(ret==-1) error(1,errno, "read failed on descriptor %d", fd);
	return ret;
}

void writeData(int fd, char * buffer, ssize_t count){
	auto ret = write(fd, buffer, count);
	if(ret==-1) error(1, errno, "write failed on descriptor %d", fd);
	if(ret!=count) error(0, errno, "wrote less than requested to descriptor %d (%ld/%ld)", fd, count, ret);
}



int MAX = 255;

int main(int argc, char ** argv){
	if(argc!=3) error(1,0,"Need 2 args");
	
	// Resolve arguments to IPv4 address with a port number
	addrinfo *resolved, hints={.ai_flags=0, .ai_family=AF_INET, .ai_socktype=SOCK_STREAM};
	int res = getaddrinfo(argv[1], argv[2], &hints, &resolved);
	if(res || !resolved) error(1, errno, "getaddrinfo");
	
	// create socket
	int sock = socket(resolved->ai_family, resolved->ai_socktype, 0);
	if(sock == -1) error(1, errno, "socket failed");
	
	// attept to connect
	res = connect(sock, resolved->ai_addr, resolved->ai_addrlen);
	if(res) error(1, errno, "connect failed");
	
	// free memory
	freeaddrinfo(resolved);

	pollfd nacoczekac[2]{};
	nacoczekac[0].fd=0;
	nacoczekac[0].events=POLLIN;
	nacoczekac[1].fd=sock;
	nacoczekac[1].events=POLLIN;
	
	while(1)
	{
		int gotowe = poll(nacoczekac, 2, -1);
	
		for(pollfd & opis : nacoczekac)
       		{
			if(opis.revents == POLLIN)
			{
				char buf1[MAX];
				int n = readData(opis.fd, buf1, MAX);
				writeData(opis.fd == 0 ? sock : 1,  buf1, n);
	
			}
		}
	}
			 
/****************************/
/*	
	// read from socket, write to stdout
	ssize_t bufsize1 = 255, received1;
	char buffer1[bufsize1];
	received1 = readData(sock, buffer1, bufsize1);
	writeData(1, buffer1, received1);
	
/****************************/
	/*
	// read from stdin, write to socket
	ssize_t bufsize2 = 255, received2;
	char buffer2[bufsize2];
	received2 = readData(0, buffer2, bufsize2);
	writeData(sock, buffer2, received2);
	
/****************************/
	
	close(sock);
	
	return 0;
}
