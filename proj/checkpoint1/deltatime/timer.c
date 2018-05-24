//Timer Process
#include<stdio.h>
#include<stdlib.h> 
#include<sys/time.h>
#include<sys/select.h>
#include<errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <string.h>
#define portNo 1100


 //struct of doubly list
 typedef struct delta_node_s{
 	float time;
 	int seqNo;
 // int portNo;
 //	char info[1]='0';
    struct delta_node_s *pre;
    struct delta_node_s *next;
 }delta_node;
 

 //create a list node
 delta_node* create_node(float time, int seqNo){
 	delta_node* newnode=NULL;
	newnode=(delta_node*)malloc(sizeof(delta_node));
	if(newnode==NULL){
		printf("Create Failed!\n");
		return NULL;
	}
	newnode->seqNo=seqNo;
	newnode->time=time;
	newnode->next=newnode->pre=NULL;
	printf("create No %d Timer, time is %.1f !\n",newnode->seqNo, newnode->time);
	return newnode;
 }
 
  //delete all list
 void delete_all_list(delta_node** phead){
 	if(*phead==NULL)
 		return;
 	delta_node* temp=*phead;
 	*phead=temp->next;
 	free(temp);
 	
 	delete_all_list(phead);
 }

//print the list
 int print_list(delta_node* head){
 	printf("Delta List as follows:\n");
 	delta_node *temp=head;
 	if(head==NULL)
 		printf("Empty List!\n");
 	while(temp!=NULL){
 		printf("Timer No %d : time left %.1f\n",temp->seqNo,temp->time);
		temp=temp->next;
	 }
	 printf("\n");
	 return 0;
 }
 
 //add a node to delta_list
  int add_list(delta_node**phead, float time, int seqNo){
  	if(phead==NULL){
  		printf("No delta_list!\n");
  		return -1;
	  }
 	int position=0;	//show where newnode inserted
 	float count=0;
 	if(time<=0){
 		printf("time has expired! Cannot create the timer!\n");
 		print_list(*phead);
 		return -1;
	 }
	delta_node* newnode=create_node(time,seqNo);
 	delta_node* temp=*phead;
 	
 	if(temp==NULL)
 		{
 			*phead=newnode;
 			print_list(*phead);
			return position;
		}
	
	if(time<temp->time){
		*phead=newnode;
		newnode->next=temp;
		temp->pre=newnode;
		temp->time-=newnode->time;
//  	printf("add to the head!\n");
//		print_list(*phead);
	}
	else{
		count=temp->time;
		while(temp->next!=NULL)
		{
			if(count<=time){
				temp=temp->next;
				count+=temp->time;
			}
			if(count>time){
				position=1;
				newnode->next=temp;
				newnode->pre=temp->pre;
				delta_node *tempre=temp->pre;
				tempre->next=newnode;
				temp->pre=newnode;
				
				newnode->time=time+temp->time-count;
				temp->time-=newnode->time;
				break;
			}
		}
		if(position!=1){
			temp=*phead;
			while(temp->next!=NULL) temp=temp->next;
			temp->next=newnode;
			newnode->pre=temp;
			newnode->time-=count;
			print_list(*phead);
			return 2;
		}
	}
	print_list(*phead);
	return position;
}
 
 //update the time of timer
int update_list(delta_node** phead, float sec){
	printf(" %.1f seconds passed!\n",sec);
	if(!phead) {
		printf("No delta list!\n");
		return -1;
	}
	delta_node *temp=*phead;
	if(temp==NULL)
	{
		printf("Empty Delta List!\n");
		return -1;
	}
	temp->time-=sec;
	if(temp->time<=0){
		printf("Time No %d expired!\n",temp->seqNo);
		*phead=temp->next;
		free(temp);
		if(*phead==NULL)
		{
			printf("All Timer has expired!\n");
			return -1; 
		}
	} 
	print_list(*phead);
	return 0;
} 


 

//delete a node of list
int delete_list(delta_node** phead, int seqNo){
	int position=0;
	delta_node *temp=*phead;
	if(temp==NULL){
		printf("Empty delta list! Cannot cancel!\n");
		return 0;
	}
	if(temp->seqNo==seqNo){
		*phead=temp->next;
		if((temp->next)!=NULL){
			(*phead)->time+=temp->time;
			(*phead)->pre=NULL;
		}
		printf("Delete No %d ,time is %.1f!\n",temp->seqNo,temp->time);
		print_list(*phead);
		free(temp);
		return position;
	}
	while((temp=temp->next)!=NULL){
		if(temp->seqNo==seqNo){
			position=1;
			delta_node* tempnext=temp->next;
			delta_node* temppre=temp->pre;
			if(tempnext==NULL){
				position=2;
				temppre->next=NULL;
				printf("Delete No %d ,time is %.1f!\n",temp->seqNo,temp->time);
				free(temp);
				print_list(*phead);
				return position;
			}
			tempnext->time+=temp->time;
			temppre->next=tempnext;
			tempnext->pre=temppre;
			printf("Delete No %d ,time is %.1f!\n",temp->seqNo,temp->time);
			free(temp);
			print_list(*phead);
			return position;
		}
	}
	printf("Cannot find the timer with Sequence No. %d!\n",seqNo);
	print_list(*phead);
	return 0;
	}




 int main(){
 
	delta_node** phead=(delta_node**)malloc(sizeof(delta_node *));
	*phead=NULL; 
	char buf[9];
//	struct hostent *host;
	int port=1100;
	struct sockaddr_in sin_addr;
	int sock;
	int msgsock;
	char op;
	float time;
	int seqNo;
	fd_set read_fds;
	
	printf("start timer process!\n");
	if((sock=socket(AF_INET, SOCK_STREAM,0))<0)
	{
		perror("error opening a datagram socket");
		exit(1);
	}

	sin_addr.sin_family=AF_INET;
	sin_addr.sin_addr.s_addr=INADDR_ANY;
	sin_addr.sin_port=htons(port);	

	if(bind(sock,(struct sockaddr*)&sin_addr,sizeof(struct sockaddr_in))<0){
		perror("error binding socket");
		exit(1);
	}
	listen(sock,5);

	if((msgsock = accept(sock,(struct sockaddr *)NULL, (unsigned int *)NULL)) == -1) 
	{ 
    	perror("error connecting stream socket");
    	exit(1);
    } 
    struct timeval tv;
	tv.tv_sec=1;
	tv.tv_usec=0;
	int err;
    
while(1){
	FD_ZERO(&read_fds);
	FD_SET(msgsock,&read_fds);
	
	if((err=select(msgsock+1,&read_fds,NULL,NULL,&tv))>0)
	{
  		int received=0;
    		if((received=recv(msgsock,buf,9,0))>0){
    			strncpy(&op,buf,1);
			memcpy(&time,&buf[1],4);
			memcpy(&seqNo,&buf[5],4);
			if(op=='A')
		    	{
				if(*phead!=NULL){
			     	   (*phead)->time=(float)(tv.tv_sec);				
				}
				if(add_list(phead,time,seqNo)==0)
				{
				    tv.tv_sec=(int)((*phead)->time);
				};
		   	 }
		   	 else if(op=='D')
		         {	
				if(*phead!=NULL){
			     	   (*phead)->time=(float)(tv.tv_sec);				
				}
				delete_list(phead,seqNo);
		         }
		  }
		if(received==0){
			while(1){
				if(*phead!=NULL){
			     	   (*phead)->time=(float)(tv.tv_sec);				
				}
				select(0,NULL,NULL,NULL,&tv);
				update_list(phead,(*phead)->time);
				if(*phead!=NULL)  tv.tv_sec=(int)((*phead)->time);
				if(*phead==NULL) exit(1);
			}
		}
	}
	else
	{	
		update_list(phead,(*phead)->time);
		if(*phead!=NULL) tv.tv_sec=(*phead)->time;
	}
    }
	 return 0;
 }
 
 
 
 
