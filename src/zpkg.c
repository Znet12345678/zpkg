/*
*ZPKG for pocket linux
*package manager
*(c) 2016 Zachary James Schlotman
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <inttypes.h>
#include "zpkg.h"
char dsig[7] = {0x0f,'D',0x0f,'I',0x0f,'R',0x0f};
char fsig[9] = {0x0f,'F',0x0f,'I',0x0f,'L',0x0f,'E',0x0f};

int pkg_dir(const char *dname,const char *out){
	DIR *d;
	FILE *f;
	FILE *rf;
	struct dirent *fent;
	struct dirent *dent;

	struct stat stats;
	int c;
	struct dirent *ent;
	printf("Open Dir:%s\n",dname);
	d = opendir(dname);
	if(!(d)){
		perror("Failed to open dir");
		//printf("Failed to open dir:%s\n",dname);
		return -1;
	}
	f = fopen(out,"ab");
	if(!(f)){
		perror("Couldn't open file");
		return -1;
	}
	char *_header = malloc(1024);
	struct stat _stat;
	stat(dname,&_stat);
	mode_t _mode = _stat.st_mode;
	sprintf(_header,"%c%s%c%c%c%c%c%s%c",1,dsig,0x0f,__TYPE_D,0x0f,_mode >> 8,_mode ,dname,0);
	for(int i = 0; i < strlen(_header) + 1;i++)
		fputc(_header[i],f);
	while((dent = readdir(d))){
		if(dent->d_type == DT_DIR){
			if(strcmp(dent->d_name,".") == 0 || strcmp(dent->d_name,"..") == 0)
				continue;
			char *_dname = malloc(1024);
			sprintf(_dname,"%s/%s",dname,dent->d_name);
			printf("Found dir:%s\n",_dname);
			struct stat stats;
			stat(_dname,&stats);
			mode_t mode = stats.st_mode;
			char *header = malloc(1024);
			sprintf(header,"%s%c%c%c%c%c%s%c",dsig,0x0f,__TYPE_D,0x0f,mode >> 8,mode,_dname,0);
			for(int i = 0; i < strlen(header) + 1;i++)
				fputc(header[i],f);
			//printf("%s",dent->d_name);
		}
		//printf("%s\n",dent->d_name);
	}
	closedir(d);
	d = opendir(dname);
	if(!(d)){
		printf("Failed to open dir:%s\n",dname);
		return -1;
	}
	while((fent = readdir(d))){
		if(fent->d_type == DT_DIR)
			continue;
		else if(strcmp(fent->d_name,out) == 0)
			continue;
		else{
			char *fname = malloc(1024);
			sprintf(fname,"%s/%s",dname,fent->d_name);
			printf ("Opened file %s\n",fname);
			rf = fopen(fname,"rb");
			if(!(f)){
				perror("Couldn't open file");
				return -1;
			}
			char *header = malloc(1024);
			stat(fname,&stats);
			mode_t mode = stats.st_mode;
			sprintf(header,"%s%c%c%c%c%c%s%c",fsig,mode >> 8,mode ,0x0f,__TYPE_F,0x0f,fname,0);
			for(int i = 0; i < strlen(header) + 1;i++){
				putc(header[i],f);
			}
			while((c = getc(rf)) != EOF){
				putc(c,f);
			}
			putc(0x0f,f);
			putc(0x0f,f);
			putc(0x0a,f);
			fclose(rf);
		}
	}
	fclose(f);
	//fclose(rf);
	closedir(d);
	d = opendir(dname);
	if(!(d)){
		perror("Couldn't open dir");
		return -1;
	}
	while((ent = readdir(d))){
		if(ent->d_type == DT_DIR){
			if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
				continue;
			char *name = malloc(1024);
			sprintf(name,"%s/%s",dname,ent->d_name);
			pkg_dir(name,out);
		}
	}
	closedir(d);
	return 1;
}
int extract_pkg(const char *name,int v){
	FILE *f;
	f = fopen(name,"rb");
	if(!(f)){
		perror("Failed to open file");
		return -1;
	}
	int c;
	while((c = getc(f)) != EOF){
		if(c == dsig[0]){
			int pos = ftell(f);
			if(getc(f) == dsig[1] && getc(f) == dsig[2] && getc(f) == dsig[3] && getc(f) == dsig[4] && getc(f) == dsig[5] && getc(f) == dsig[6]){
				if(getc(f) == 0x0f && getc(f) == __TYPE_D && getc(f) == 0x0f){
					mode_t mode = getc(f) << 8 | getc(f);
					char *dname = malloc(1024);
					int i = 0;
					while((c = getc(f)) != 0){
						dname[i] = c;
						i++;
					}
					if(v == 1)
						printf("Creating dir: %s\n",dname);
					if(mkdir(dname,mode) < 0){
						if(v == 1)
							perror("Failed to create dir");
					}
				}
				else
					fseek(f,pos,SEEK_SET);
			}
			else
				fseek(f,pos,SEEK_SET);
		}
		if(c == fsig[0]){
			//printf(".\n");
			int pos = ftell(f);
			if(getc(f) == fsig[1] && getc(f) == fsig[2] && getc(f) == fsig[3] && getc(f) == fsig[4] && getc(f) == fsig[5] && getc(f) == fsig[6] && getc(f) == fsig[7] && getc(f) == fsig[8]){
				//printf(".\n");
				mode_t mode = getc(f) << 8 | getc(f);
				if(getc(f) == 0x0f && getc(f) == __TYPE_F && getc(f) == 0x0f){
					char *fname = malloc(1024);
					int i = 0;
					while((c = getc(f)) != 0){
						fname[i] = c;
						i++;
					}
					if(v == 1)
						printf("Open file:%s\n",fname);
					FILE *wf = fopen(fname,"wb");
					if(!(wf)){
						perror("Failed to open file!");
						return -1;
					}
					while((c = getc(f)) != EOF){
						if(c == 0x0f){
							int pos = ftell(f);
							if(getc(f) == 0x0f && getc(f) == 0x0a)
								break;
							else
								fseek(f,pos,SEEK_SET);

						}
						putc(c,wf);
					}
					fclose(wf);
					chmod(fname,mode);
				}
			}
			else
				fseek(f,pos,SEEK_SET);
		}
	}
	return 1;
}
