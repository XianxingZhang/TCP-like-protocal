#include <stdio.h>
#include <string.h>
int main(int argc,char *argv[]){
	if(argc!=4){
		puts("The command format is \"count <a><b><c>\"\n");
		return -1;
	}
	FILE *fp,*fo;
	int flen;
	char filename[20],str[20],outputfile[20];
	strcpy(filename,argv[1]);
	strcat(filename,".txt");
	strcpy(str,argv[2]);
	strcpy(outputfile,argv[3]);
	strcat(outputfile,".txt");
	if((fp=fopen(filename,"r"))==NULL){
		puts("Cannot open inputfile!");
		return -1;
	}
	if((fo=fopen(outputfile,"wr"))==NULL){
		puts("Cannon open outputfile!");
		return -1;
	}
	fseek(fp,0L,SEEK_END);
	flen=ftell(fp);
	fprintf(fo,"Size of file is %d\n",flen);
	rewind(fp);
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
				fseek(fp,history_offset-100,SEEK_SET);
				fread(buffer,1,100,fp);
			}
		}
	}
	fprintf(fo,"Number of matches =  %d\n",counter);
	fclose(fo);
	fclose(fp);
	return 0;
}
