#include <stdio.h>
#include <string.h>
int main(){
	FILE *fp;
	int flen;
	char p[]="example1.txt";
	if((fp=fopen(p,"r"))!=NULL){
		fseek(fp,0L,2);
		flen=ftell(fp);
		printf("The length of the file is %d byte\n",flen);
	}
	
	rewind(fp);													//?
	
	char str[]="the";
	char buffer[100];
	int i,counter=0,slen=strlen(str);
	for(i=0;i<flen;++i){
		if(i%100==0) 
			fread(buffer,1,100,fp);
		if(buffer[i%100]==str[0]){
			int j,sign;
			long history_offset=ftell(fp);
			for(j=1,sign=1;j<slen&&j+i<flen&&sign;++j){
				if((j+i)%100==0)
					fread(buffer,1,100,fp);
				sign=(str[j]==buffer[(i+j)%100]);
			}
			if(sign&&j==slen)
				++counter;
			if(ftell(fp)!=history_offset){
				//fseek(fp,history_offset-100,0);
				fseek(fp,history_offset-100,SEEK_SET);
				fread(buffer,1,100,fp);
			}
		}
	}
	printf("counter: %d\n",counter);
	return 0;
}
