/*
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>


typedef struct MyMessage {
	struct sockaddr_in msg_header;
	long contents;
} MyMessage;

/* interval between message sends */
struct timeval timeout = {
	0L, /* seconds */
	10000L, /* microseconds */
};

struct timeval timeout_original;


main(argc,argv)
int argc;
char *argv[];
{

	int sock;	/* a socket for sending messages and receiving responses */
	MyMessage message;
	struct hostent *host;
	u_short port;
	struct sockaddr_in trolladdr, destaddr, localaddr, fromaddr;
	fd_set selectmask;
	int counter, n;
	int arg;

	/* process arguments */

	if (argc != 4) usage(argv[0]);

	/* troll ... */

	if ((host = gethostbyname(argv[arg])) == NULL) {
		Fprintf(stderr, "%s: Unknown troll host '%s'\n",argv[0],argv[arg]);
		exit(1);
	}  

	port = atoi(argv[arg+1]);
	if (port < 1024 || port > 0xffff) {
		Fprintf(stderr, "%s: Bad troll port %d (must be between 1024 and %d)\n",
			argv[0], port, 0xffff);
		exit(1);
	}

	bzero ((char *)&trolladdr, sizeof trolladdr);
	trolladdr.sin_family = AF_INET;
	bcopy(host->h_addr, (char*)&trolladdr.sin_addr, host->h_length);
	trolladdr.sin_port = htons(port);

	/* destination ... */

	if ((host = gethostbyname(argv[2])) == NULL) {
		Fprintf(stderr, "%s: Unknown troll host '%s'\n",argv[0],argv[arg+2]);
		exit(1);
	}  

	port = atoi(argv[3]);
	if (port < 1024 || port > 0xffff) {
		Fprintf(stderr, "%s: Bad troll port %d (must be between 1024 and %d)\n",
			argv[0], port, 0xffff);
		exit(1);
	}

	bzero ((char *)&destaddr, sizeof destaddr);
	destaddr.sin_family = htons(AF_INET);
	bcopy(host->h_addr, (char*)&destaddr.sin_addr, host->h_length);
	destaddr.sin_port = htons(port);

	/* create a socket... */

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("totroll socket");
		exit(1);
	}
	FD_ZERO(&selectmask);
	FD_SET(sock, &selectmask);

	/* ... and bind its local address */
	bzero((char *)&localaddr, sizeof localaddr);
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = INADDR_ANY; /* let the kernel fill this in */
	localaddr.sin_port = 0;					/* let the kernel choose a port */
	if (bind(sock, (struct sockaddr *)&localaddr, sizeof localaddr) < 0) {
		perror("client bind");
		exit(1);
	}

	/* Main loop */

	counter = 0;
	message.contents = counter++;
	message.msg_header = destaddr; /* structure copy */
	n = sendto(sock, (char *)&message, sizeof message, 0,
					(struct sockaddr *)&trolladdr, sizeof trolladdr);
	if (n!=sizeof message) {
		perror("totroll sendto");
		exit(1);
	}

	timeout_original = timeout;
	for(;;) {
		int len = sizeof fromaddr;
		fd_set rmask;

		//printf("Before timeout: %ld %ld\n", timeout.tv_sec, timeout.tv_usec);

		rmask = selectmask;
		n = select(sock+1, &rmask, 0, 0, &timeout);
		if (n < 0) {
			perror("totroll select");
			exit(1);
		}
		if (FD_ISSET(sock, &rmask)) {
			/* read in one message from the troll */
			n = recvfrom(sock, (char *)&message, sizeof message, 0,
					(struct sockaddr *)&fromaddr, &len);
			if (n<0) {
				perror("totroll recvfrom");
				exit(1);
			}
			printf("<<< %d %s\n",
				message.contents,
				message.contents==counter-1 ? "" : "???");
			continue;
		}
		/* timeout: send off another message to the troll */
		if (timeout.tv_sec == 0 &&  timeout.tv_usec==0){
			timeout = timeout_original;
			message.contents = counter++;
			message.msg_header = destaddr;
			printf(">>> %d\n",message.contents);
			n = sendto(sock, (char *)&message, sizeof message, 0,
							(struct sockaddr *)&trolladdr, sizeof trolladdr);
			if (n!=sizeof message) {
				perror("totroll sendto");
				exit(1);
			}
		}
	}
} /* main */

usage(prog)
char *prog;
{
	fprintf(stderr, "usage: %s ", prog);
	Fprintf(stderr, " <trollhost> <trollport> <desthost> <destport>\n");
	exit(1);
}


