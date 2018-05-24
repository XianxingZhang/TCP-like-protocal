// driver.c


#include<stdio.h>	
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>			//socket
#include<sys/types.h>			//socket
#include<netinet/in.h>			//sockaddr_in
#include<netdb.h>
#include<unistd.h>

int main(){
  int sockfd;				//initial socket descriptor
  struct sockaddr_in server_addr;	//set up socket struct
  char buf[BUFFER_SIZE];		//create buffer
  struct hostent *hostIP;
  char *filename;
  FILE *fp;
  int flength;

	
}




