#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#define _PATH_NAME_ "./pp"

int main()
{
  char *str="hello world";
  int fd;
  if(mkfifo(_PATH_NAME_,0644)<0) 
  {
    printf("hahaha\n");
    printf("mkfifo error %d:%s\n",errno,strerror(errno));
  }
  fd=open(_PATH_NAME_,O_WRONLY);
  write(fd,str,strlen(str));
  return 0;
}
