#include <stdio.h>
#include "zpkg.h"
#include <string.h>
void usage(const char *argv){
	printf("%s [-x file to extract] [-a directory to archive file output {-v}]\n",argv);
}
int getarg(int argc,const char *argv[],char *arg){
	for(int i = 0; i < argc;i++){
		if(strcmp(argv[i],arg) == 0)
			return i;
	}
	return -1;
}
int main(int argc,const char *argv[]){
	if(argc < 3)
		usage(argv[0]);
	else{
		if(getarg(argc,argv,"-x") > 0){
			if(getarg(argc,argv,"-v") == 0)
				extract_pkg(argv[getarg(argc,argv,"-x") + 1],1);
			else
				extract_pkg(argv[getarg(argc,argv,"-x") + 1],0);
		}
		else if(getarg(argc,argv,"-a") > 0){
			FILE *f = fopen(argv[getarg(argc,argv,"-a") + 2],"wb");
			if(!(f)){
				perror("Failed to open file");
				return -1;
			}
			fputc(0,f);
			pkg_dir(argv[getarg(argc,argv,"-a") + 1],argv[getarg(argc,argv,"-a") + 2]);
		}
		else
			usage(argv[0]);
	}
}
