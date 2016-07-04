#include <stdio.h>
#include <sys/un.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
  struct sockaddr_in  server_addr;
  server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
  server_addr.sin_port=htons(8001);
  server_addr.sin_family=AF_INET;
  int sockfd=socket(PF_INET,SOCK_STREAM,0);
  if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
  {
    printf("connect error");
    exit(0);

  }
  printf("cnnect success");
  return 0;
}
