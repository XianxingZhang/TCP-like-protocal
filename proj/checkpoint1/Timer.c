//Timer Process
#include<stdio.h>
#include<stdlib.h> 
//#include<sys/select.h>
//#include<errno.h>
#include <sys/param.h>
//#include <sys/types.h>
//#include <sys/signal.h>
//#include <sys/socket.h>
//#include <sys/time.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <ctype.h>
//#include <netdb.h>
//#include <string.h>


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
			else{
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
 

//delete a node of list
int delete_list(delta_node** phead, int seqNo){
	int position=0;
	delta_node *temp=*phead;
	if(temp==NULL){
		printf("Empty delta list! Cannot cancel!\n");
		return -1;
	}
	if(temp->seqNo==seqNo){
		*phead=temp->next;
		(*phead)->time+=temp->time;
		if(temp->next){
			delta_node* tempnext=temp->next;
			tempnext->pre=NULL;
		}
		print_list(*phead);
		free(temp);
		return position;
	}
	while((temp=temp->next)!=NULL){
		if(temp->seqNo==seqNo){
			position=1;
			delta_node* tempnext=temp->next;
			delta_node* temppre=temp->pre;
			if(!tempnext){
				position=2;
				temppre->next=NULL;
				free(temp);
				print_list(*phead);
				return position;
			}
			tempnext->time+=temp->time;
			temppre->next=tempnext;
			tempnext->pre=temppre;
			print_list(*phead);
			return position;
		}
	}
	printf("Cannot find the timer with Sequence Number!\n");
	return -1;
	}


/* int sleepsec(int seconds){
 	struct timeval tv;
 	int err;
 	tv.tv_sec=seconds;
 	tv.tv_usec=0;
 	err=select(0,NULL,NULL,NULL,&tv);
 	return err;
 }
 
*/
 int main(){
 


	delta_node** phead=(delta_node**)malloc(sizeof(delta_node *));
	//*phead=(delta_node*)malloc(sizeof(delta_node));
	*phead=NULL; 
	add_list(phead,20.0,1);
	add_list(phead,10.0,2);
	add_list(phead,30.0,3);
	sleep(5);
	update_list(phead,5.0);
	delete_list(phead,2);
	add_list(phead,40.0,4);
	sleep(5);
	update_list(phead,5.0);
	add_list(phead,18.0,5);
	delete_list(phead,4);
	delete_list(phead,5);
 
 /*	while(1){
 	//	sleepsec(1);
 		sleep(1);
 		if((update_list(phead,1.0))<0)
 		break;
 		print_list(*phead);
	 }
	 print_list(*phead);
*/
	 return 0;
 }
 
 
 
 
