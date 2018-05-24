/*
 * Program fromtroll.c
 *
 * Testing program to test the "troll" (q.v.)
 * Listens for messages and attempts to echo them back to the sender via
 * the troll.  See also totroll.c.
 */
/*#ifndef lint
static char *rcsid = "$Header: /var/home/solomon/640/troll/RCS/fromtroll.c,v 3.2 1991/11/07 18:11:43 solomon Exp solomon $";
#endif
*/

#include <sys/param.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1000
extern int errno;

typedef struct MyMessage {
	struct sockaddr_in msg_header;
	char contents[BUFFER_SIZE];
} ;

/* for lint */
void bzero(), bcopy(), exit(), perror();
#define Printf (void)printf
#define Fprintf (void)fprintf

main(argc,argv)
int argc;
char *argv[];
{

	int sock;	/* a socket for sending messages and receiving responses */
	int msssock;	/* a socket for sending messages to server	*/
	MyMessage message;
	u_short port;
	struct sockaddr_in trolladdr, localaddr;
	int n,m;
	//int lastseq = -1;

	/* process arguments */
	if (argc != 2) {
		printf("command format is: tcpd <port.NO>\n");
		//Fprintf(stderr, "usage: %s port\n", argv[0]);
		exit(1);
	}

	port = atoi(argv[1]);
	if (port < 1024 || port > 0xffff) {
		Fprintf(stderr, "%s: Bad troll port %d (must be between 1024 and %d)\n",
			argv[0], port, 0xffff);
		exit(1);
	}

	/* create a socket... */

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("tcpd socket");
		exit(1);
	}
	if ((msssock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("tcpd socket");
		exit(1);
	}

	/* ... and bind its local address */
	bzero((char *)&localaddr, sizeof localaddr);
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = INADDR_ANY; /* let the kernel fill this in */
	localaddr.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&localaddr, sizeof localaddr) < 0) {
		perror("client bind");
		exit(1);
	}

	/* Main loop */

	for(;;) {
		int len = sizeof trolladdr;

		/* read in one message from the troll */
		n = recvfrom(sock, (char *)&message, sizeof message, 0,
				(struct sockaddr *)&trolladdr, &len);
		if (n<0) {
			perror("tcpd recvfrom");
			exit(1);
		}
		printf("size of message received: %d\n",n);
		m=sendto(sock,(char *)&message, sizeof message,0,(struct sockaddr *)&message.msg_header, len);
		if (m<0) 
		{
			perror("tcpd sendto");
			exit(1);
		}
		if(m==n) {
			printf("all message sent to server!\n");
		}
		if(m<n) {
			printf("%d bytes message missed\n",n-m);
		}
		//n = message.contents - (lastseq+1);
		//Printf(">>> message content=%d\n", message.contents);
	//	if (n == 0)
	//		Printf(">>> %d\n", message.contents);
			
	//	else if (n > 0)
	//		Printf(">>> %d (%d missing)\n", message.contents, n);
	//	else
	//		Printf(">>> %d (duplicate)\n", message.contents);
	//	lastseq = message.contents;
				
	//	errno = 0;
	/*	n = sendto(sock, (char *)&message, sizeof message, 0,
						(struct sockaddr *)&trolladdr, len);
		if (n!=sizeof message) {
			perror("fromtroll sendto");
			exit(1);
	*/
		}
	}
} /* main */


