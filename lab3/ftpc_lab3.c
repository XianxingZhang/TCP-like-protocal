/* 
/Xianxing Zhang 
/reimplement ftpc 
/reference:  Eric Olson and James Baker 
	      CSE 5462 Network Programming 
		Project 1 - Chekcpoint 1 - September 29, 2016
*/

#include "head.h"

// client ftpc called with file name and no args
int main(int argc, char *argv[]){
	if(argc<1){
		printf("Command Format is: \"ftpc <filename>\"\n");
		exit(1);
	}
	
	int sockfd;			//socket descriptor
	int count;			//
	int filesize;
	int readcount;
	int sentcount=0;
	int sent_total=0;
	struct sockaddr_in tcpd_addr;
	char *filename=argv[1];
	char buf[BUFFER_SIZE]={0};
	struct hostent *hp;

	printf("%s\n\n", "TCP client preparing to send file....");

	/* initialize socket connection */
	if((sockfd = SOCKET(AF_INET, SOCK_STREAM, 0))<0){
		perror("Error Opening socket);
		exit(1);
	}

	/* construct the name of socket to send to */
	tcpd_addr.sin_family=AF_INET;
	tcpd_addr.sin_port=0;   			//Kernel assign
	tcpd_addr.sin_addr.s_addr=INADDR_ANY;  //Kernel assign

	/* establish connection with tpcd */
	if(CONNECT(sockfd, (struct sockaddr)&tcpd_addr, 
		    sizeof(struct sockaddr_in))<0 )
	{
		close(sockfd);
		perror("Error connecting socket");
		exit(1);
	}

	/* Open file for transfer */
	if(strlen(filename>20)
	{
		printf("filename is too long!");
		exit(1);
	}

	FILE *fp=fopen(filename,"rb");
	if(fp==NULL){
		perror("Error opening file");
		exit(1);
	}

	/* Read number of bytes in files */
	fseek(fp,0L,2);
	filesize=ftell(fp);
	rewind(fp);

	/* Send file size in 4 bytes */
	printf("Send file size: %d bytes\n\n",filesize);
	filesize=htonl(filesize);
	if((SEND(sockfd, &filesize, 4, 0))<0){
		perror("Error send file size");
		exit(0);
	}

	/* Send file name in 20 bytes */
	if((SEND(sockfd, filename, 20, 0))<0){
		perror("Error send file name):
		exit(0);
	}
	printf("Sent file name: %s\n\n",filename);
	
	/* Send file content */
	sent_total=filesize;
	while(sent_total){
		sentcount=fread(buf,sizeof(char),BUFFER_SIZE,fp);
		if((SEND(sockfd,buf,sentcount,0))<0){
			perrer("Error sending file content");
			exit(1);
		}
		sent_total-=sentcount;
	}

	printf("%s\n", "File Sent.");

	/* Close file and connection */
	close(fp);
	close(sock);
	return 0;
} 
	
	
	
