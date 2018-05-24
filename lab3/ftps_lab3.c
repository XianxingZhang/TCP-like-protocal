/* Xianxing
/reimplement
/reference Jacob Sage, Eric Olson and James Baker

/This file contains implementation of a TCP server
*/

#include "header_lab3.h"

int main(int argc, char *argv[]){

	/* check for the correct number of arguments */
	if(argc !=2){
		perror("incorrect number of arguments. the correct format is :\n				\nftps <port>\n\n");
		exit(1);
	}
	char *port = argv[1];
  	int sockfd;                     /* initial socket descriptor */
  	struct sockaddr_in tcpd_addr; /* structure for socket name setup */
  	char buf[BUFFER_SIZE];               /* buffer for holding read data */
	int filesize=0;
	char filename[20]={'\0'};
	printf("TCP server waiting for remote connection from clients ...\n");		
  	/*initialize socket connection in unix domain*/
  	if((sock = SOCKET(AF_INET, SOCK_STREAM, 0)) < 0){
    		perror("error openting datagram socket");
    		exit(1);
  	}
  	
	/* construct name of socket to send to */
  	tcpd_addr.sin_family = AF_INET;
 	tcpd_addr.sin_addr.s_addr = INADDR_ANY;	//Kernel assign
  	tcpd_addr.sin_port = htons(LOCALPORT);	//Kernel assign

  	/* bind socket name to socket */
  	if((BIND(sock, (struct sockaddr *)&sin_addr, sizeof(struct sockaddr_in)) 			< 0)
	{
    		perror("error binding stream socket");
    		exit(1);
  	 }
  
  /* listen for socket connection and set max opened socket connetions to 5 */
  	LISTEN(sock, 5);
  
  /* accept a (1) connection in socket msgsocket */ 
  	if((ACCEPT(sock, (struct sockaddr *)NULL, (unsigned int *)NULL)) == -1) { 
 		perror("error connecting stream socket");
    		exit(1);
	} 
  
  /* Receive file size */
	if((RECV(sockfd,&filesize,4,0))<4){
		perror("Error: The size read returned less than 4 bytes");
		exit(1);
	}
	filesize=ntohl(filesize);
	printf("Received size: %d bytes\n\n",filesize);

  /* Get file name */
	if((RECV(sockfd,filename,sizeof(filename),0))<20){
		perror("The name read returned less than 20 bytes");
		exit(1);
	}
	printf("Received name: %s\n\n",filename);

  /* create the directory recvd if it hasn't been already */
	  struct stat dir = {0};
 	 if (stat("./recvd", &dir)) {
   		 printf("Creating directory recvd...\n");
   		 mkdir("./recvd", 0777); 
 	 }

  /* create the file we're writing to/overwriting */
 	 FILE *outputFile;
 	 char path[1000];
 	 sprintf(path, "./recvd/%s", fileName); 
 	 printf("Path for file:\t%s\n\n", path);
 	 outputFile = fopen(path, "wb");

  /* clear buffer and counters, then begin reading bytes in chunks of 1000 */
	int received=0;
	int total=0;
	while(total < filesize){
		if((received=RECV(sockfd,buf,sizeof(buf),MSG_WAITALL))<0){
			printf("Error reading!\n);
			exit(1);
		}
		total+=received;
		fwrite(buf,1,received,outputFile);
	}
	prinf("Recieved file.\n);

		
  /* close all connections and remove socket file */

  fclose(outputFile);
  close(sockfd);

  return 0;
}