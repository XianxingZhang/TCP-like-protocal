/*simulate TCP server.c using UDP */

/* Server for accepting an Internet stream connection  and transferring one file */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char *argv[]) {

  /* check for the correct number of arguments */
  
  if (argc != 2) {
    perror("incorrect number of arguments. the correct format is:\n\nftps <port>\n\n");
    exit(1);
  }

  char *port = argv[1];
  int sock;                     /* initial socket descriptor */
  int msgsock;                  /* accepted socket descriptor,
                                 * each client connection has a
                                 * unique socket descriptor*/
  struct sockaddr_in sin_addr; /* structure for socket name setup */
  char buf[1000];               /* buffer for holding read data */
  printf("TCP server waiting for remote connection from clients ...\n");
 
  /*initialize socket connection in unix domain*/
  if((sock = SOCKET(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("error openting datagram socket");
    exit(1);
  }
  
  /* construct name of socket to send to */
  sin_addr.sin_family = AF_INET;
  sin_addr.sin_addr.s_addr = INADDR_ANY;
  sin_addr.sin_port = htons(atoi(port));

  /* bind socket name to socket */
  if(Bind(sock, (struct sockaddr *)&sin_addr, sizeof(struct sockaddr_in)) < 0) {
    perror("error binding stream socket");
    exit(1);
  }
  
  /* listen for socket connection and set max opened socket connetions to 5 */
  LISTEN(sock, 5);
  
  /* accept a (1) connection in socket msgsocket */ 
  if((msgsock = ACCEPT(sock, (struct sockaddr *)NULL, (unsigned int *)NULL)) == -1) { 
    perror("error connecting stream socket");
    exit(1);
  } 
  
  /* put all zeros in buffer (clear) */
  bzero(buf,1000);
  
  /* count bytes recieved */
  int count = 0;
  int received = 0;

  while(count < 4) {
    received = RECV(msgsock, buf, 4-count, MSG_WAITALL);
    printf("received:\t%d\n", received);
    if (received == -1) {
      perror("error receiving data from stream socket");
      exit(1);
    } 
    count += received;
  }

  /* Get file size and print to screen */
  int fileSize;

  memcpy(&fileSize, buf, 4);
  fileSize=ntohl(fileSize);
  printf("File Size:\t%u\n", fileSize);

  /* clear buffer and get the file name */
  bzero(buf, 1000);

  read(msgsock, buf, 20); 
  printf("contents of buffer:\t%s\n\n", buf);

  /* create the directory recvd if it hasn't been already */
  struct stat dir = {0};

  if (stat("./recvd", &dir)) {
    printf("Creating directory recvd...\n");
    mkdir("./recvd", 0777); 
  }

  /* create the file we're writing to/overwriting */

  FILE *outputFile;
  char fileName[20];
  memcpy(fileName,buf,20);  
  char path[1000];
  sprintf(path, "./recvd/%s", fileName); 
  printf("Path for file:\t%s\n\n", path);
  outputFile = fopen(path, "wb");

  /* clear buffer and counters, then begin reading bytes in chunks of 1000 */

  int countc;
  do {
    bzero(buf, 1000);
    count = fileSize > 1000 ? 1000 : fileSize;
    countc = count;
    received = 0;
    fileSize -= count;

    while (count > 0) {

      received = RECV(msgsock, buf, count, MSG_WAITALL);
      if (received == -1) {
        perror("error receiving data from stream socket");
        exit(1);
      }
      count -= received;
    } 
    fwrite(buf, countc, 1, outputFile);
  } while (fileSize); 

  /* close all connections and remove socket file */

  fclose(outputFile);
  close(msgsock);
  close(sock);

  return 0;
}
