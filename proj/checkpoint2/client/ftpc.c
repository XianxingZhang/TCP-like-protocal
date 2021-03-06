/*
/ Eric Olson and James Baker
/ CSE 5462 Network Programming
/ Project 1 - Checkpoint 1 - September 29, 2016
/ 
/ This file contains our implementation of a TCP client.
*/

#include "header.h"

/* client program called with file name and no args*/
int main(int argc, char *argv[]) {
	/* validate input args */
	if(argc < 1) {
		fprintf(stderr, "Error: Include file name is arguments.\n");
		exit(1);
	}

	int sock;                     /* initial socket descriptor */
	int rval;                     /* returned value from a read */
	int num_bytes;		      /* number of bytes of file to be sent */
	int num_read;		      /* bytes read from file stream */
	int num_sent_total = 0;
	int num_sent = 0;
	struct sockaddr_in sin_addr;  /* structure for socket name setup */
	char * file_name = argv[1];   /* file name */
	char buf[MSS] = {0};          /* bytes to send to server */
	struct hostent *hp;	      /* host */
	
	printf("%s\n\n", "TCP client preparing to send file...");

	/* initialize socket connection in unix domain */
	if((sock = SOCKET(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Error opening socket");
		exit(1);
	}

  	/* construct name of socket to send to */
  	sin_addr.sin_family = AF_INET;
  	sin_addr.sin_port = 0; /* Kernel assign*/
	sin_addr.sin_addr.s_addr = INADDR_ANY; /* Kernel assign */
  
  	/* establish connection with server */
  	if(CONNECT(sock, (struct sockaddr *)&sin_addr, sizeof(struct sockaddr_in)) < 0) 
	{
    		close(sock);
    		perror("Error connecting stream socket");
    		exit(1);
  	}
  
  	/* Open file for transfer */
  	FILE *fp = fopen(file_name,"rb");
  	if(fp==NULL) 
	{
    		perror("Error opening file");
    		exit(1);  
  	}

  	/* Read number of bytes in file, rewind file to start index */  
	fseek(fp, 0L, SEEK_END);
	num_bytes = ftell(fp);
	rewind(fp);
        
	/* Send file size in 4 bytes */
	SEND(sock, &num_bytes, MSS, 0);
	printf("Sent size: %i bytes\n\n", num_bytes);

	/* Send file name in 20 bytes */
	SEND(sock, file_name, MSS, 0);
	printf("Sent name: %s\n\n", file_name);

	bzero(buf, sizeof(buf));

	while((num_read = fread(buf,1,sizeof(buf), fp)) > 0)
	{
		/* send buffer client side tcpd */
		num_sent = SEND(sock, buf, num_read, 0);

		/* for bookkeeping */
		num_sent_total += num_sent;

		/* added to avoid overloading client side tcpd when sending large files */
		usleep(20000);
	}

	printf("%s\n", "File Sent.");

	/* Close file and connection */
	close(fp);
	close(sock);
	return(0); 
}
