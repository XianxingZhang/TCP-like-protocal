ssize_t SEND(int sockfd, void *buf, size_t len, int flags) {
	/* sendto buffer to tcpd */

	struct sockaddr_in localtcpd_addr;

	localtcpd_addr.sin_family = AF_INET;
	localtcpd_addr.port = htons(2000);
	localtcpd_addr.sin_addr.s_addr = INADDR_ANY;  //fill it by kernel
	int bind_val = bind(sockfd, (struct sockaddr *)&localtcpd_addr, 
			   sizeof(struct sockaddr_in));
	/* sendto addr to tcpd */
	
	return sendto(sockfd, buf, len, 0, (struct sockaddr *)&localtcpd_addr, sizeof(struct sockaddr);

}



ssize_t RECV(int sockfd, void *buf, size_t len, int flags) {

	struct sockaddr_in localtcpd_addr;

	localtcpd_addr.sin_family = AF_INET;
	localtcpd_addr.port = htons(2000);
	localtcpd_addr.sin_addr.s_addr = INADDR_ANY;
	int bind_val = bind(sockfd, (struct sockaddr *)&localtcpd_addr,sizeof(struct sockaddr);
	/* recvfrom addr from tcpd */

	return recvfrom(sockfd, buf, len, 0, (struct sockaddr *)&localtcpd_addr, sizeof(struct sockaddr);

}



int BIND(int sockfd, const (struct sockaddr *addr)&my_addr, sizeof(struct sockaddr)) {

	/* define the port for tcpd and return that instead */

	struct sockaddr_in localtcpd_addr;

	localtcpd_addr.sin_family = AF_INET;
	localtcpd_addr.port = htons(2000);
	localtcpd_addr.sin_addr.s_addr = INADDR_ANY;
	int bind_val = bind(sockfd, (struct sockaddr *)&localtcpd_addr,sizeof(struct sockaddr);
	/* sendto addr to tcpd */

	sendto(sockfd, addr, sizeof(struct sockaddr_in), 0, (struct sockaddr *)&localtcpd_addr, sizeof(struct sockaddr);

	return bind_val;
}



int ACCEPT(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags) {

return 1;

}



int CONNECT(int sockfd, conststruct sockaddr *addr, socklen_t addrlen) {

return 0;

}



int SOCKET(int domain, in type, int protocol) {

	return socket(AF_INET, SOCK_DGRAM, 0);

}



int LISTEN(int sockfd, int backlog) {

	return 0;

}
