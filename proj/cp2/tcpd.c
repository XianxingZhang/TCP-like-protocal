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

/* crc function */
unsigned char crc(char * data, char check, int size) {
	const unsigned char polynomial= 0x1d;
	unsigned char remainder = 0;

	char temp[size + 1];
	memcpy(temp, data, size);
	temp[size- 1] = check;
	if (check != '\0') {
			
		size++; 
		
	}
	temp[size] = check;
	int i;
	for (i = 0; i < size - 1; i++) {
		remainder ^= temp[i];
		int j;
		for (j = 0; j < 8; j++) {
			if ((remainder & 0x80) != 0) {
				remainder = (unsigned char)((remainder << 1) ^ polynomial);
			} else {
				remainder <<= 1;
			}
		}
	}
	return remainder; 
}

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
		if (argc < 5) {
			fprintf(stderr, "%s\n", "There are not enough arguments. Please be sure to include Local Host, Local Troll Port, Remote Host, and Remote Port.");
			exit(1);
		}

		printf("%s\n\n", "Running on client machine...");

		int troll_sock;							/* a socket for sending messages to the local troll process */
		int local_sock; 						/* a socket to communicate with the client process */
		MyMessage message; 						/* Packet sent to troll process */
		Packet packet;							/* Packet sent to server tcpd */
		tcpdHeader tcpd_head;						/* Packet type from client */
		struct hostent *host; 						/* Hostname identifier */
		struct sockaddr_in trolladdr, destaddr, localaddr, clientaddr;  /* Addresses */
		fd_set selectmask, readfds, writefds;				/* Socket descriptor for select */
		int amtFromClient, amtToTroll, total = 0; 			/* Bookkeeping vars for sending */
		int chksum = 0;							/* Checksum */
		struct Packet * packetbuffer;					/* Packet buffer */
		int bufferwindowstart;						/* Start of buffer window */
		int sequencenumber;						/* Packet Sequence number */
		struct AckMessage ack;						/* Sequence number being acked */
		int len;							/* Length of destination address */
		struct timeval now;
		struct timeval send_time;
		float RTT=0;
		float SRTT=0;
		float DEV=0;
		float RTO=0;
		
		/* LOCAL TROLL ADDRESSS */
		/* this is the addr that local troll is running on */

		if ((host = gethostbyname(argv[2])) == NULL) {
			printf("Unknown troll host '%s'\n",argv[2]);
			exit(1);
		}  
		bzero ((char *)&trolladdr, sizeof trolladdr);
		trolladdr.sin_family = AF_INET;
		bcopy(host->h_addr, (char*)&trolladdr.sin_addr, host->h_length);
		trolladdr.sin_port = htons(atoi(argv[3]));


		/* DESTINATION ADDRESS */
		/* This is the destination address that the local troll will forward packets to */

		if ((host = gethostbyname(argv[4])) == NULL) {
			printf("Unknown troll host '%s'\n",argv[4]);
			exit(1);
		} 
		bzero ((char *)&destaddr, sizeof destaddr);
		destaddr.sin_family = htons(AF_INET);
    		bcopy(host->h_addr, (char*)&destaddr.sin_addr, host->h_length);
		destaddr.sin_port = htons(atoi(argv[5]));


		/* SOCKET TO TROLL */
		/* This creates a socket to communicate with the local troll process */

		if ((troll_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("totroll socket");
			exit(1);
		}
		bzero((char *)&localaddr, sizeof localaddr);
		localaddr.sin_family = AF_INET;
		localaddr.sin_addr.s_addr = INADDR_ANY; /* let the kernel fill this in */
		localaddr.sin_port = 0;					/* let the kernel choose a port */
		if (bind(troll_sock, (struct sockaddr *)&localaddr, sizeof localaddr) < 0) {
			perror("client bind");
			exit(1);
		}

		/* SOCKET TO CLIENT */
		/* This creates a socket to communicate with the local client process */

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

		/* PACKET BUFFER */

		packetbuffer = malloc(BUFFERSIZE * sizeof(Packet));
		sequencenumber = 0;
		bufferwindowstart = 0; 

		/* SEND DATA TO TROLL */

		/* Initialize checksum table */	
		//crcInit();
		
		/* Prepare for select */
		FD_ZERO(&selectmask);
		FD_SET(local_sock, &selectmask);
		
		/* Begin send loop */
		for(;;) {

			/* set up read and write */
			
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
					
					/* Set the sequence number for the packet */

					packet.sequence_number = sequencenumber;

					/* Store packet in buffer */

					packetbuffer[sequencenumber % BUFFERSIZE] = packet;

					printf("buffer:\t");

					int k = 0;
					for (k = 0; k < BUFFERSIZE; k++) {
						if(k == bufferwindowstart % BUFFERSIZE) {
							printf("[");
						}
						printf("%i, ", packetbuffer[k].sequence_number);
						if(k == (bufferwindowstart + WINDOWSIZE) % BUFFERSIZE) {
							printf("]");
						}
					}
					printf("\n");

					/* Prepare troll wrapper */

					message.msg_pack = packet;
					message.msg_header = destaddr;

					message.chksum = crc(packet.body,'\0' ,packet.bytes_to_read+1);
					/* Send packet to troll */
					amtToTroll = sendto(troll_sock, (char *)&message, sizeof message, 0, (struct sockaddr *)&trolladdr, sizeof trolladdr);
					//printf("Sent message to troll.\n\n");
					gettimeofday(&send_time,NULL);
					
					if (amtToTroll != sizeof message) {
						perror("totroll sendto");
						exit(1);
					}

					/* Wait for an Ack from server Troll *** MOVE THIS FOR CHECKPOINT 3, WHEN THE SELECT FUNCTIONALITY IS IMPLEMENTED */

					len = sizeof destaddr;
					recvfrom(troll_sock, (char *)&ack, sizeof(ack), MSG_WAITALL, NULL, NULL);

					printf("Ack %i received.\n", ack.sequence_number);
					gettimeofday(&now,NULL);
					RTT=1000*(now.tv_sec-send_time.tv_sec)+((float)now.tv_usec-(float)send_time.tv_usec)/1000;
					SRTT+=(RTT-SRTT)/8;
					if((SRTT-RTT)<0) DEV+=(RTT-SRTT-DEV)/8;
					else DEV+=(SRTT-RTT-DEV);
					RTO=SRTT+4*DEV;
					printf("RTT= %.3f ms\n,SRTT");
					printf("RTO= %.3f ms\n",RTO);
					sequencenumber++;
					bufferwindowstart++;


					/* For bookkeeping/debugging */
					total += amtToTroll;

				/* Packet from server (server = 0)*/
			     	} else if (tcpd_head.flag == 0) {
					/* Fill in later: Useful for checkpoint 2 */
				/* Unexpected packet type */
			   	} else {
					fprintf(stderr, "%s\n", "Message from unknown source");
				 	exit(1);
				}

			} 
		}
		
	/* Run on server side */
	} else if (atoi(argv[1]) == 0) {

		/* Validate args */
		if (argc < 5) {
			fprintf(stderr, "%s\n", "There are not enough arguments. Please be sure to include the local host, local port, remote host, and remote port.");
			exit(1);
		}

		printf("%s\n\n", "Running on server machine...");		

		int troll_sock;						/* a socket for sending messages and receiving responses */
		int local_sock; 					/* a socket to communicate with the client process */
		MyMessage message; 					/* recieved packet from remote troll process */
		struct sockaddr_in trolladdr, localaddr, serveraddr, destaddr;    /* Addresses */
		struct hostent *host; 					/* Hostname identifier */
		fd_set selectmask, readfds, writefds;			/* Socket descriptor for select */
		int amtFromTcpd, amtToServer, len, total = 0; 		/* Bookkeeping vars */
		unsigned char chksum = 0;						/* Checksum */
		struct AckMessage ack;					/* Ack Message */
		
		
		/* SOCKET FROM TROLL */
		/* This is the socket to recieve from the troll running on the client machine */
		if ((troll_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("fromtroll socket");
			exit(1);
		}
		bzero((char *)&localaddr, sizeof localaddr);
		localaddr.sin_family = AF_INET;
		localaddr.sin_addr.s_addr = INADDR_ANY; /* let the kernel fill this in */
		localaddr.sin_port = htons(atoi(argv[5]));
		if (bind(troll_sock, (struct sockaddr *)&localaddr, sizeof localaddr) < 0) {
			perror("client bind");
			exit(1);
		}
		

		/* SOCKET TO SERVER */
		/* This creates a socket to communicate with the local troll process */
		if ((local_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("client socket");
			exit(1);
		}
		
		/* LOCAL TROLL ADDRESSS */
		/* this is the addr that local troll is running on */

		if ((host = gethostbyname(argv[2])) == NULL) {
			printf("Unknown troll host '%s'\n",argv[2]);
			exit(1);
		}  
		bzero ((char *)&trolladdr, sizeof trolladdr);
		trolladdr.sin_family = AF_INET;
		bcopy(host->h_addr, (char*)&trolladdr.sin_addr, host->h_length);
		trolladdr.sin_port = htons(atoi(argv[3]));
		/* ADDRESS TO CONNECT WITH THE SERVER */
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(LOCALPORT);
		serveraddr.sin_addr.s_addr = inet_addr(LOCALADDRESS);

		/* RECEIVE DATA */

		/* Initialize checksum table */		
		//crcInit();

		/* Prepare descriptor*/
		FD_ZERO(&selectmask);
		FD_SET(troll_sock, &selectmask);

		/* Begin recieve loop */
		for(;;) {
			
			/* Set fd_sets for reading and writing */

				
			/* If data is ready to be recieved from troll on client machien */
			if (FD_ISSET(troll_sock, &selectmask)) {
				
				/* length of addr for recieve call */
				len = sizeof trolladdr;
				
				/* read in one packet from the troll */
				amtFromTcpd = recvfrom(troll_sock, (char *)&message, sizeof message, MSG_WAITALL,
					NULL, NULL);
				if (amtFromTcpd < 0) {
					perror("fromtroll recvfrom");
					exit(1);
				}

				printf("Recieved message from troll.\n\n");

				/* Send an ACK back to the client */

				ack.header = message.msg_header;
				ack.sequence_number = message.msg_pack.sequence_number;

				sendto(troll_sock, (char *)&ack, sizeof(ack), 0, (struct sockaddr *)&trolladdr, sizeof trolladdr);
				printf("Ack %i sent\n", ack.sequence_number);

				// see if the checksum is valid
				if (crc((char *)&message.msg_pack.body, message.chksum, message.msg_pack.bytes_to_read+1)) {
					 printf("Message garbled\n\n");
				} 
				/* Forward packet body to server */
				amtToServer = sendto(local_sock, (char *)&message.msg_pack.body, message.msg_pack.bytes_to_read, 0, (struct sockaddr *)&serveraddr, sizeof serveraddr);
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


