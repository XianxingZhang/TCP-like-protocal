/* Xianxing
/reimplement
/reference: Eric Olson and james Baker

/This file contains implementations of TCP functions using UDP
*/

#include "header_lab3.h"

/* Prototypes for each function */ 
ssize_t SEND(int socket, const void* buf, size_t len, int flags);
ssize_t RECV(int socket, void* buf, size_t length, int flags);
int BIND(int socket, struct sockaddr *my_addr, socklen_t addrlen);
int ACCEPT(int sock, struct sockaddr *address, socklen_t *address_len, 
	    int flags);
int CONNECT(int sockfd, const struct sockaddr *addr,
            socklen_t addrlen);
int SOCKET(int domain, int type, int protocol);
int LISTEN(int sockdf,int backlog);

/* Function implmentations */
int ACCEPT(int sock, struct sockaddr *address, socklen_t *address_len,int flags) {
	return 1;
}

int CONNECT(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	return 0;
}

int SOCKET(int domain, int type, int protocol) {
	return socket(domain, SOCK_DGRAM, protocol);
}

int BIND(int socket, struct sockaddr *my_addr, socklen_t addrlen) {
        return bind(socket, (struct sockaddr *)my_addr, addrlen);
}

int LISTEN(int sockfd, int backlog){
	return 0;
}

ssize_t SEND(int socket, const void* buf, size_t len, int flags) {

	/* define address of local socket to pass data to */
	struct sockaddr_in tcpd_addr;
	tcpd_addr.sin_family = AF_INET;
	tcpd_addr.sin_port = htons(LOCALPORT);
	tcpd_addr.sin_addr.s_addr = INADDR_ANY;

	/* wrapper for sending data to tcpd */
	tcpdHeader clientHeader;

	/* indicates packet is from client process */
	/* client = 1/server = 0 */
	clientHeader.flag = 1;

	/* Passes along the amount of actual bytes to be read from payload */
	/* IMPORTANT only for first two and last packet sent */		
	clientHeader.bytes_to_read = length;

	/* Copy buffer contents to payload to tcpd */
	bcopy(buffer,clientHeader.body,length);

	/* Send packet to client side tcpd */
	return sendto(socket, (char *)&clientHeader, sizeof(clientHeader), flags, 
		(struct sockaddr *)&tcpd_addr, sizeof(struct sockaddr));
}

ssize_t RECV(int socket, void* buf, size_t length, int flags) {

	/* receive data from tcpd */
	struct sockaddr_in tcpd_addr;
	
	tcpd_addr.sin_family=AF_INET;
	tcpd_addr.port=htons(LOCALPORT);
	tcpd_addr.sin_addr.s_addr=INADDR_ANY;
	return recvfrom(socket, buf, length, flags,
			   (struct sockaddr *)&tcpd_addr, 
			   sizeof(struct sockaddr));
}