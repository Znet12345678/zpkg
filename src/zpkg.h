#ifndef __ZPKG_H
#define __ZPKG_H

#define __TYPE_F 1
#define __TYPE_D 2
struct header{
	int s1;
	int type;
	int s2;
	char name[80];
	int perms;
};
int pkg_dir(const char *dir,const char *out);
int extract_pkg(const char *name,int v);
#endif
