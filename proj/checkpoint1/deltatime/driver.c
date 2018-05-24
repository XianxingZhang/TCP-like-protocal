/*
/Xianxing Zhang
/ driver.c
/ This file contains implementation of sending requests to timer process
*/

#include<stdio.h>	
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>			//socket
#include<sys/types.h>			//socket
#include<netinet/in.h>			//sockaddr_in
#include<netdb.h>
#include<unistd.h>

#define BUFFER_SIZE 9
//#define HostIP 164.107.113.22
#define portNo 1100



int main(){
  int sockfd;				//initial socket descriptor
  struct sockaddr_in timer_addr;	//set up socket struct
  char buf[BUFFER_SIZE];		//create buffer
  //struct hostent *hostIP;
  int port=portNo;
  
/* Create a socket to communicate with timer process */
  if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
	perror("Create Socket Failed!");
	exit(1);
  }
  
 /* hostIP=gethostbyname("164.107.113.22");
  if(hostIP==0){
	printf("Unknown host\n");
	exit(2);
  }
*/

  timer_addr.sin_family=AF_INET;
  timer_addr.sin_port=htons(port); //set the port number of 1100
 // bcopy((void*)hostIP->h_addr,(void*)&timer_addr.sin_addr,hostIP->h_length);
  timer_addr.sinaddr.s_addr=INADDR_ANY; //let the kernel fill this in

   if(connect(sockfd,(struct sockaddr*)&timer_addr,sizeof(struct sockaddr_in))<0)
   {
	perror("Cannot send!\n");
	exit(2);
    }

//define starttimer
int starttimer(float timer,int seqNo){
	bzero(buf,BUFFER_SIZE);
	buf[0]='A';
        memcpy(&buf[1],&timer,4);
	memcpy(&buf[5],&seqNo,4);
	if(send(sockfd,buf,9,0)<0){
		printf("Send Add Request Failed!\n");
		exit(1);
	}	
	return 0;
}

//define canceltimer()
int canceltimer(int seqNo){
	bzero(buf,BUFFER_SIZE);
	buf[0]='D';
	memcpy(&buf[5],&seqNo,4);
	if(send(sockfd,buf,9,0)<0){
		printf("Send Delete Request Failed!\n");
		exit(1);
	}
    	return 0;
}
	starttimer(20.0,1);
	starttimer(10.0,2);
	starttimer(30.0,3);
	sleep(5);
	canceltimer(2);
	starttimer(20.0,4);
	sleep(5);
	starttimer(18.0,5);
	sleep(2);
	canceltimer(4);
	canceltimer(8);

	return 0;
}



