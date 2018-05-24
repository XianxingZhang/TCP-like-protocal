#include<stdio.h>
#include<stdlib.h>

int main()
{
	
	char op;
	char buf[9];
	float time=20.0;
	int s=2;
	
	float t;
	int se;
	
	buf[0]='A';
	strncpy(&op,buf,1);
	memcpy(&buf[1],&time,4);
	memcpy(&buf[5],&s,4);
	
	printf("op==%c\n",op);

	memcpy(&t,&buf[1],4);
	printf("time==%.1f\n",t); 
	
	return 0;
 } 
