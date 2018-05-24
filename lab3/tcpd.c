//tcpd in client


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
#define PORTIN 1995
#define PORTOUT 1996

int main()
{
	int sockin, sockout;
	char* buffer[BUFFER_SIZE];
	
	if((sockin=socket(AF_INET,SOCK_DGRAM,0))<0){
     	perror("Create Socket Failed\n");
     	exit(1);
	}
 	if((sockout=socket(AF_INET,SOCK_DGRAM,0))<0){
     	perror("Create Socket Failed\n");
    	 exit(1);
  	}
  	
  	if(recvfrom(sockin,buffer,BUFFER_SIZE,0,client_addr,sizeof(client_addr))<0)
	{
		perror("recving datagram message");
		exit(2);
	}
	
	if(sendto(sockout,buffer,BUFFER_SIZE,0,troll_addr,sizeof(troll_addr))<0){
		perror("sending datagram message");
		exit(2);
	}
}


