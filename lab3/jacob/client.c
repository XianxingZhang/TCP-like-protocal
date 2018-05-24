
  //client.c
  //xianxing

#include<stdio.h>	
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>			//socket
#include<sys/types.h>			//socket
#include<netinet/in.h>			//sockaddr_in
#include<netdb.h>
#include<unistd.h>

#define BUFFER_SIZE 1000
#define FILENAME_MAX_LENGTH 20
#define PORT 1994

int main(int argc,char *argv[]){
  if(argc!=4){
     puts("command format is\"ftpc<remote-IP><remote-port><local-file>\"\n");
     exit(1);
  }

  int sockfd;				//initial socket descriptor
  struct sockaddr_in server_addr;	//set up socket struct
  char buf[BUFFER_SIZE];		//create buffer
  struct hostent *hostIP;
  char *filename;
  FILE *fp;
  int flength;

//create stream socket
  if((sockfd=SOCKET(AF_INET,SOCK_STREAM,0))<0){
     perror("Create Stream Socket Failed\n");
     exit(1);
  }
  
  hostIP=gethostbyname(argv[1]);
  if(hostIP==0){
     printf("%s: unknow host\n",argv[1]);
     exit(2);
  }
  
//set the server socket
  server_addr.sin_family=AF_INET;
  server_addr.sin_port=htons(atoi(argv[2])); 
  bcopy((void *)hostIP->h_addr,(void *)&server_addr.sin_addr,hostIP->h_length);

//connect with server
  if(CONNECT(sockfd,(struct sockaddr *)&server_addr, sizeof(struct sockaddr_in))<0)
  {
     close(sockfd);
     perror("Connecting Stream Socket Failed\n");
     exit(1);
  }
  
//send file
  filename=argv[3];
  if(strlen(filename)>20){
     puts("filename is too long!");
     exit(1);
  }
  if((fp=fopen(filename,"rb"))==NULL){
     puts("file cannnot open\n");
     exit(1);
  }
  fseek(fp,0L,2);
  flength=ftell(fp);
  printf("fileSize:\t%u\n", flength);
  int countf=flength;
  flength=htonl(flength);
  //printf("fileSize in network order: %u\n",flength);

  bzero(buf,BUFFER_SIZE);
  memcpy(buf,&flength,4);
  rewind(fp);
  if(SEND(sockfd,buf,4,0)<0){
     perror("Send File Size Failed");
     exit(1);
  }
  bzero(buf,4);

  strcpy(buf,argv[3]);
  printf("buffer:\t%s\n", buf);
  if(write(sockfd, buf, 20) < 0) {
    perror("Send File Name Failed");
    exit(1);
  }
  int count=0;
  while(countf){
	count=fread(buf,sizeof(char),BUFFER_SIZE,fp);
	if(SEND(sockfd,buf,count,0)<0){
	   printf("Send File Failed\n");
	   exit(1);
  	}
	countf-=count;
	bzero(buf,BUFFER_SIZE);
  }

//close file and disconnet
  fclose(fp);
  printf("File: %s Transfer Succeeded!\n",filename);
  close(sockfd);
  return 0;
}

