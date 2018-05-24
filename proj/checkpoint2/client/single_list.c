/*
/  This file contains implementation of a single-linked list for RTT/RTO
*/

#include "header.h"

struct node_t{
	int seqNo;
//	int start_location
//	int num_byte;
	struct timeval send_time;
	struct node_t* next;
}node;

node* creat_list(int seqNo){
	node* newnode=(node*)malloc(sizeof(node));
	if(newnode==NULL){
		printf("Create Failed!\n");
		return NULL;
	} 
	newnode->seqNo=seqNo,
	gettimeofday(&send_time,NULL);
	newnode->next=NULL;
	printf("Create No %d node with time!\n",newnode->seqNo);
}
	
void add_node(node* head,int seqNo){
	node* temp=head;
	while(temp->next!=NULL)
		temp=temp->next;
	temp->next=create_list(seqNo);
//	print_list(head);
	}

struct timeval delete_node(node* head, int seqNo){
	node* pre=head;
	node* temp=pre->next;
	struct timeval send_time=0;
	while(temp!=NULL){
		if(temp->seqNo==seqNo){
			pre->next=temp->next;
			send_time=temp->send_time;
			free(temp);
			break;
		}
		temp=temp->next;
		pre=pre->next;
				
	return send_time;	//if send_time==0, not found
}

int print_list(node* head){
	if(head->next==NULL){
		printf("Empty list!\n");
		return -1;
	}
	while(head->next!=NULL){
		head=head->next;
		printf("SeqNo: %d is left in the list!\n",head->seqNo);
	}
	return 0;
}



