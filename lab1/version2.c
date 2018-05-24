#include<stdio.h>
#include<string.h>

//get size of file
int GetFileSize(char *filename)			
{
	int size=0;
	FILE *f=fopen(filename,"rb");		//open read-only file 
	if(f==NULL)				//if file not exist,information
	{
		printf("input file not exists\n");
		return 0;
	}
	fseek(f,0L,2);
	size=ftell(f);
	fclose(f);
	return size;
}

//search the number of specific string
int Search(char *str,char *filename) 	 
{
	int size=GetFileSize(filename);
	FILE *f=fopen(filename,"rb");
	char buf[100];				//read file in chunks of 100 bytes
	int i,count=0,strl=strlen(str);		//"i" defines current searching position
	for(i=0;i<size;i++)
	{
		if(i%100==0)
			fread(buf,1,100,f);
		if(buf[i%100]==str[0])		//when matches,"i" stops, "j" moves on
		{
			int j,sign=1;
			long label=ftell(f);	//mark the current buffur status
			for(j=1;sign&&j<strl&&i+j<size;j++)
			{
				if((i+j)%100==0)//if searchig goes beyond the chunk,refill it
					fread(buf,1,100,f);
				sign=(buf[(i+j)%100]==str[j]);
			}
			if(sign&&j==strl)
				count++;				
			if(i+j==size)
				return count;
			if(ftell(f)!=label)	//recover buffur status if changed
			{
				fseek(f,label-100,0);
				fread(buf,1,100,f);
			}
		}
	}
	return count;
}
		
//main function with four arguments 
int main(int argc, char *argv[])		
{	if(argc!=4)				//set format of oommand
		puts("the command format is \"count<a><b><c>\"\n");
	else{
	char inputf[20],searchs[20],outputf[20];
	FILE *out;
	if(strlen(argv[1])>20){
		puts("the name of inputfile is too long!");
		return -1;
	}
	if(strlen(argv[2])>20){
		puts("searching string is too long!");
		return -1;
	}
	if(strlen(argv[3])>20){
		puts("the name of outputfile is too long!");
		return -1;
	}
	strcpy(inputf,argv[1]);
	strcpy(searchs,argv[2]);
	strcpy(outputf,argv[3]);

	if((out=fopen(outputf,"wr"))==NULL){
		puts("cannot open outputfile!");
		return -1;
	}

	int size=GetFileSize(inputf);
	int times=Search(searchs,inputf);
	printf("Size of file is %d bytes\n",size);
	printf("Number of matches = %d\n",times);
//	FILE *out=fopen(outputf,"w+");		//output searching result
	fprintf(out,"Size of file is %d bytes\n",size);
	fprintf(out,"Number of matches = %d bytes\n",times);
}
	return 0;
}

