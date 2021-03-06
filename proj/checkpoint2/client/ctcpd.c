/*
/ Xianxing Zhang
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
	/* Run on client side */
	if (atoi(argv[1]) == 1) {
		/* Validate input args */
		if (argc < 6) {
			fprintf(stderr, "%s\n", "There are not enough arguments. Please be sure to include Local Host, Local Troll Port, Remote Host, and Remote Port.");
			exit(1);
		}

		printf("%s\n\n", "Running on client machine...");
		node* head=create(-1);
		int count=0;			//seqNo in the list
		struct timeval Acks=0;
		struct timeval now;
		float RTT;	
		float SRTT;
		float DEV;
		float RTO;
		int troll_sock;							/* a socket for sending messages to the local troll process */
		int local_sock; 						/* a socket to communicate with the client process */
		MyMessage message; 						/* Packet sent to troll process */
		Packet packet;							/* Packet sent to server tcpd */
		tcpdHeader tcpd_head;						/* Packet type from client=1/server=0 */
		struct hostent *host; 						/* Hostname identifier */
		struct sockaddr_in trolladdr, destaddr, localaddr, clientaddr;  /* Addresses */
		fd_set selectmask; 						/* Socket descriptor for select */
		int amtFromClient, amtToTroll, total = 0; 			/* Bookkeeping vars for sending */
		
		/* TROLL ADDRESSS */
		/* this is the addr that troll is running on */

		if ((host = gethostbyname(argv[2])) == NULL) {
			printf("Unknown troll host '%s'\n",argv[2]);
			exit(1);
		}  
		bzero ((char *)&trolladdr, sizeof(trolladdr));
		trolladdr.sin_family = AF_INET;
		bcopy(host->h_addr, (char*)&trolladdr.sin_addr, host->h_length);
		trolladdr.sin_port = htons(atoi(argv[3]));

		/* DESTINATION ADDRESS */
		/* This is the destination address that the troll will forward packets to */

		if ((host = gethostbyname(argv[4])) == NULL) {
			printf("Unknown troll host '%s'\n",argv[4]);
			exit(1);
		} 
		bzero ((char *)&destaddr, sizeof(destaddr));
		destaddr.sin_family = htons(AF_INET);
    		bcopy(host->h_addr, (char*)&destaddr.sin_addr, host->h_length);
		destaddr.sin_port = htons(atoi(argv[5]));

		/* SOCKET TO TROLL */
		/* This creates a socket to communicate with the local troll process */

		if ((troll_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("totroll socket");
			exit(1);
		}
		bzero((char *)&localaddr, sizeof(localaddr));
		localaddr.sin_family = AF_INET;
		localaddr.sin_addr.s_addr = INADDR_ANY; /* let the kernel fill this in */
		localaddr.sin_port = 0;					/* let the kernel choose a port */
		if (bind(troll_sock, (struct sockaddr *)&localaddr, sizeof localaddr) < 0) {
			perror("client bind");
			exit(1);
		}

		/* SOCKET TO CLIENT */
		/* This creates a socket to communicate with the local troll process */

		if ((local_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("client socket");
			exit(1);
		}
		bzero((char *)&clientaddr, sizeof clientaddr);
		clientaddr.sin_family = AF_INET;
		clientaddr.sin_addr.s_addr = inet_addr(LOCALADDRESS); /* let the kernel fill this in */
		clientaddr.sin_port = htons(LOCALPORT);
		if (bind(local_sock, (struct sockaddr *)&clientaddr, sizeof clientaddr) < 0) {
			perror("client bind");
			exit(1);
		}

		/* SEND DATA TO TROLL */
		
		/* Prepare for select */
		FD_ZERO(&selectmask);
		FD_SET(local_sock, &selectmask);
		
		/* Begin send loop */
		for(;;) {
			
			/* Wait for data on socket from client */
			if (FD_ISSET(local_sock, &selectmask)) {
				/* Receive data from the local socket */
				amtFromClient = recvfrom(local_sock, (char *)&tcpd_head, sizeof(tcpd_head), MSG_WAITALL, NULL, NULL);

				/* Packet from client (client = 1)*/
				if (tcpd_head.flag == 1) {
					
					printf("Received message from client.\n");	
					/* Copy payload from client to tcpd packet */
					bcopy(tcpd_head.body,packet.body, tcpd_head.bytes_to_read);
				
					/* Pass along actual bytes to be read from payload */
					packet.bytes_to_read = tcpd_head.bytes_to_read;
					
					/* Prepare troll wrapper */
					message.seqNo=count;
					message.msg_pack = packet;
					message.msg_header = destaddr;
					add_node(head,count);
					count++;
					/* Send packet to troll */
					amtToTroll = sendto(troll_sock, (char *)&message, sizeof(message), 0, (struct sockaddr *)&trolladdr, sizeof trolladdr);
					printf("Sent message to troll.\n\n");
					sleep(2);
					if (amtToTroll != sizeof message) {
						perror("totroll sendto");
						exit(1);
					}

					/* For bookkeeping/debugging */
					total += amtToTroll;


				/* Packet from server (server = 0)*/
			     	} else if (tcpd_head.flag > 0) {
					if((Acks=delete_node(head,(int)tcpd_head.flag)!=0){
						gettimeofday(&now,NULL);
						RTT=1000*(now.tv_sec-Acks.tv_sec)+((float)now.tv_usec-(float)Acks.tv_usec))/1000;
				printf("RTT= %.3f ms \n",RTT);
					}
					/* Fill in later: Useful for checkpoint 2 */
				/* Unexpected packet type */
			   	} else {
					fprintf(stderr, "%s\n", "Message from unknown source");
				 	exit(1);
				}

			/* Reset socket descriptor for select */
			FD_ZERO(&selectmask);
			FD_SET(local_sock, &selectmask);
			} 
		}
		
	
