
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
  int fd;
  fd=open(_PATH_NAME_,O_RDONLY);
  char buf[1024]={'\0'};
  read(fd,buf,1024);
  printf("%s\n",buf);
  return 0;
}
