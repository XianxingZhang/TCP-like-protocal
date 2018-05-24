/*
/ Jacob Sage
/ CSE 5462 Network Programming
/ 
*/

#include "header.h"


/* for lint */
void bzero(), bcopy(), exit(), perror();
double atof();
#define Printf if (!qflag) (void)printf
#define Fprintf (void)fprintf

/* main */
int main(int argc, char *argv[])
{
	/* Validate initial args */
	if (argc < 1) {
		fprintf(stderr, "%s\n", "There are not enough arguments.");
		exit(1);
	}

	/* Run on server side */
	if (atoi(argv[1]) == 0) {
		/* Validate args */
		if (argc < 2) {
			fprintf(stderr, "%s\n", "There are not enough arguments. Please be sure to include the local port.");
			exit(1);
		}
		printf("%s\n\n", "Running on server machine...");		

		int troll_sock;						/* a socket for sending messages and receiving responses */
		int local_sock; 					/* a socket to communicate with the client process */
		MyMessage message; 					/* recieved packet from remote troll process */
		MyMessage Acks;						/* reply to the client */
		struct sockaddr_in trolladdr, localaddr, serveraddr;    /* Addresses */
		struct hostent *host; 					/* Hostname identifier */
		fd_set selectmask;					/* Socket descriptor for select */
		int amtFromTcpd, amtToServer, len, total = 0; 		/* Bookkeeping vars */

		
		/* SOCKET FROM TROLL */
		/* This is the socket to recieve from the troll running on the client machine */
		if ((troll_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("fromtroll socket");
			exit(1);
		}
		/* This is the socket to send to the troll running on the 
		bzero((char *)&localaddr, sizeof localaddr);
		localaddr.sin_family = AF_INET;
		localaddr.sin_addr.s_addr = INADDR_ANY; /* let the kernel fill this in */
		localaddr.sin_port = htons(atoi(argv[2]));
		if (bind(troll_sock, (struct sockaddr *)&localaddr, sizeof localaddr) < 0) {
			perror("client bind");
			exit(1);
		}
		
		/* SEND ACKS TO TROLL */
		/* This is the client address */	
		Acks.msg_header.sin_family=AF_INET;
		Acks.msg_header.sin_port= htons(CLOCALPORT);
		Acks.msg_header.sin_addr.s_addr=inet_addr(CLOCALADDRESS);

		/* SOCKET TO SERVER */
		/* This creates a socket to communicate with the local troll process */
		if ((local_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("client socket");
			exit(1);
		}
		
		/* ADDRESS TO CONNECT WITH THE SERVER */
		struct sockaddr_in destaddr;
		destaddr.sin_family = AF_INET;
		destaddr.sin_port = htons(LOCALPORT);
		destaddr.sin_addr.s_addr = inet_addr(LOCALADDRESS);

		/* RECEIVE DATA */


		/* Prepare descriptor*/
		FD_ZERO(&selectmask);
		FD_SET(troll_sock, &selectmask);

		/* Begin recieve loop */
		for(;;) {
		
			/* If data is ready to be recieved from troll on client machien */
			if (FD_ISSET(troll_sock, &selectmask)) {
				
				/* length of addr for recieve call */
				len = sizeof trolladdr;
	
				/* read in one packet from the troll */
				amtFromTcpd = recvfrom(troll_sock, (char *)&message, sizeof message, MSG_WAITALL,
					(struct sockaddr *)&trolladdr, &len);
				if (amtFromTcpd < 0) {
					perror("fromtroll recvfrom");
					exit(1);
				}
				
				printf("Recieved message from troll.\n\n");
				
				/* Forward Acks to tcpd in client machine */
				Acks.flag=message.SeqNo;
				if((sendto(troll_sock,(char *)&Acks, sizeof(Acks), 0, (struct sockaddr *)&trolladdr, sizeof(trolladdr)))<0){
						printf("Send Acks Failed!\n");
						exit(1);
				}
	
				
				/* Forward packet body to server */
				amtToServer = sendto(local_sock, (char *)&message.msg_pack.body, message.msg_pack.bytes_to_read, 0, (struct sockaddr *)&destaddr, sizeof destaddr);
				if (amtToServer < 0) {
					perror("totroll sendto");
					/* To keep daemon running for grable demo */
					//exit(1);
				}
				printf("Sent message to server.\n\n");

				/* Bookkeeping/Debugging */
				total += amtFromTcpd;
			}
			
			/* Reset decriptor */
			FD_ZERO(&selectmask);
			FD_SET(troll_sock, &selectmask);
		}
	}
}
