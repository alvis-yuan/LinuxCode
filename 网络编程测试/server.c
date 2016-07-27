#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

void func(int num)
{
  printf("this is a test!\n");
}



//服务器端
int main()
{
  int sockfd;
  int newfd;
  struct sockaddr_in server_addr; 
  bzero(&server_addr,sizeof(struct sockaddr_in));
  socklen_t len;
  server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
  server_addr.sin_port=htons(8001);
  server_addr.sin_family=AF_INET;
  if((sockfd=socket(PF_INET,SOCK_STREAM,0))==-1)
  {
    printf("socket error");
    exit(0);
  }

  if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
  {
    fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
    exit(0);
  }


  if(listen(sockfd,5)==-1)
  {
    printf("listen error");
  }

  newfd=accept(sockfd,(struct sockaddr *)(&server_addr),&len);
  if(newfd==-1)
  {
    printf("accept error");
  }
  else{
    printf("connect success");
  }
  return 0;
}
