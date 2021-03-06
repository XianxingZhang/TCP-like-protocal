#ifndef HEADER_H_  
#define HEADER_H_

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Define constants */
#define MSS 1000
#define LOCALPORT 	2222
#define LOCALADDRESS "127.0.0.1"

#define S_LOCALADDRESS ""
#define S_LOCALPORT		3333

/* Structs used */

/* tcpd to tcpd packet */
typedef struct Packet {
	char body[MSS];
	int bytes_to_read;	
	//add additional vars here
} Packet;

/* tcpd to troll packet */
typedef struct MyMessage {
	int seqNo;
	struct sockaddr_in msg_header;
	struct Packet msg_pack;
} MyMessage;

/* client/server to tcpd packet */
typedef struct tcpdHeader {
	int flag;
	size_t maxData;
	char body[MSS];
	int bytes_to_read;
} tcpdHeader;

#endif
