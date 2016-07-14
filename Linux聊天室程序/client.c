#define _GNU_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>


#define BUFFER_SIZE 64


int main()
{
  const char *ip="192.168.84.128";
  int port=8001;
  struct sockaddr_in serveraddress;
  bzero(&serveraddress,sizeof(struct sockaddr_in));
  serveraddress.sin_family=AF_INET;
  serveraddress.sin_port=htons(port);
  serveraddress.sin_addr.s_addr=inet_addr(ip);

  int sockfd=socket(PF_INET,SOCK_STREAM,0);
  if(sockfd<0)
  {
    printf("socket error ,%d:%s\n",errno,strerror(errno));
    exit(-1);
  }
  if(connect(sockfd,(struct sockaddr *)(&serveraddress),sizeof(struct sockaddr))<0)
  {
    printf("connect error,%d:%s\n",errno,strerror(errno));

  }
  struct pollfd fds[2];
  fds[0].fd=0;
  fds[0].events=POLLIN;
  fds[0].revents=0;
  fds[1].fd=sockfd;
  fds[1].events=POLLIN | POLLRDHUP;
  fds[1].revents=0;
  char read_buff[BUFFER_SIZE];
  int pipefd[2];
  int ret=pipe(pipefd);
  if(ret<0)
  {
    printf("pipe error,%d:%s\n",errno,strerror(errno));
  }
  while(1)
  {
    ret=poll(fds,2,-1);
    if(ret<0)
    {
      printf("poll error,%d:%s\n",errno,strerror(errno));
    }
    if(fds[1].revents & POLLRDHUP)
    {
      printf("server shutdown\n");
      break;
    }
    else if(fds[1].revents & POLLIN)
    {
      memset(read_buff,'\0',BUFFER_SIZE);
      recv(sockfd,read_buff,BUFFER_SIZE-1,0);
      printf("%s\n",read_buff);
    }
    if(fds[0].revents & POLLIN)
    {
      memset(read_buff,'\0',BUFFER_SIZE);
      read(fds[0].fd,read_buff,BUFFER_SIZE-1);
      send(sockfd,read_buff,BUFFER_SIZE,0);
    }
  }
  close(sockfd);
  return 0;
}
