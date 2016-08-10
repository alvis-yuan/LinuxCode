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
#include <netinet/tcp.h> 
//参数解释
//fd:网络连接描述符
//start:首次心跳侦测包发送之间的空闲时间  
 //interval:两次心跳侦测包之间的间隔时间 
//count:探测次数，即将几次探测失败判定为TCP断开
int set_tcp_keepAlive(int fd, int start, int interval, int count)   
{   
    int keepAlive = 1;   
    if (fd < 0 || start < 0 || interval < 0 || count < 0) return -1;   //入口参数检查 ，编程的好习惯。
    //启用心跳机制，如果您想关闭，将keepAlive置零即可   
    if(setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,(void*)&keepAlive,sizeof(keepAlive)) == -1)   
    {   
        perror("setsockopt");   
        return -1;   
    }   
    //启用心跳机制开始到首次心跳侦测包发送之间的空闲时间   
    if(setsockopt(fd,SOL_TCP,TCP_KEEPIDLE,(void *)&start,sizeof(start)) == -1)   
    {   
        perror("setsockopt");   
        return -1;   
    }   
    //两次心跳侦测包之间的间隔时间   
    if(setsockopt(fd,SOL_TCP,TCP_KEEPINTVL,(void *)&interval,sizeof(interval)) == -1)   
    {   
        perror("setsockopt");   
        return -1;   
    }   
    //探测次数，即将几次探测失败判定为TCP断开   
    if(setsockopt(fd,SOL_TCP,TCP_KEEPCNT,(void *)&count,sizeof(count)) == -1)   
    {   
        perror("setsockopt");   
        return -1;   
    }   
    return 0;   
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
set_tcp_keepAlive(newfd,0,5,2);
  return 0;
}
