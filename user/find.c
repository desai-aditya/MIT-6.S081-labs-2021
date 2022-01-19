#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char namebuf[DIRSIZ * FSSIZE + 1];

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), 0, DIRSIZ-strlen(p));
  return buf;
}

void
find(char *path, char * pat)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(namebuf, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  //printf("%s\n",namebuf);

  switch(st.type){
  case T_FILE:
    //printf("%s fmtname\n",fmtname(namebuf));
  	if(strcmp(fmtname(namebuf),pat)==0)
		printf("%s\n",namebuf);
    //printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, namebuf);
    p = namebuf+strlen(namebuf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
	  if(strcmp(de.name,".")==0)continue;
	  else if(strcmp(de.name,"..")==0)continue;
	  find(buf,pat);
      //printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
	printf("usage: find [<dir>] <name>\n");
	exit(1);
  }
  char * dir = ".";
  char * pat= argv[1];
  if(argc >= 3){
    dir=argv[1];
    pat=argv[2];
  }
  memmove(namebuf,dir,strlen(dir));
  find(dir,pat);
  exit(0);
}
