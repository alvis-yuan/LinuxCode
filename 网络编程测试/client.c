
#include <stdio.h>
#include <sys/un.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
int main()
{
  struct sockaddr_in  server_addr;
  server_addr.sin_addr.s_addr=inet_addr("192.168.84.128");
  server_addr.sin_port=htons(8001);
  server_addr.sin_family=AF_INET;
  int sockfd=socket(PF_INET,SOCK_STREAM,0);
  if(sockfd<0)
  {
    printf("socket error, %d:%s",errno,strerror(errno));
  }
  if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
  {
    printf("connect error,%d:%s",errno,strerror(errno));
    exit(0);

  }
  printf("cnnect success");
  return 0;
}
