#include <stdio.h>
#include "zpkg.h"
int main(int argc,char *argv[]){
	if(argc != 2)
		return -1;
	printf("Begin extract\n");
	extract_pkg(argv[1]);
}
