ssize_t SEND(int sockfd, void *buf, size_t len, int flags) {



}



ssize_t RECV(int sockfd, void *buf, size_t len, int flags) {
	struct sockaddr_in client_addr;
	int addr_len = sizeof(client_addr);
	return recvfrom(sockfd, buf, len, flags, (struct sockaddr*)&client_addr, &addr_len); 

}



int BIND(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {

	return bind(sockfd, addr, addrlen);

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



}
