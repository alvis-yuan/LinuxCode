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
#include <malloc.h>

#define BUFFER_SIZE 64
#define USER_LIMIT 5
#define FD_LIMIT 65535

struct client_data
{
  struct sockaddr_in address;
  char *write_buf;
  char buf[BUFFER_SIZE];
};


int setnoblock(int fd)
{
  int old_option=fcntl(fd,F_GETFL);
  int new_option=old_option | O_NONBLOCK;
  fcntl(fd,F_SETFL,new_option);
  return old_option;
}


int main()
{
  const char *ip="192.168.84.128";
  int port=8001;
  struct sockaddr_in address;
  bzero(&address,sizeof(struct sockaddr_in));
  address.sin_port=htons(port);
  address.sin_family=AF_INET;
  address.sin_addr.s_addr=inet_addr(ip);
  

  int listenfd=socket(PF_INET,SOCK_STREAM,0);
  if(listenfd<0)
  {
    printf("socket error,%d:%s\n",errno ,strerror(errno));
  }
  if(bind(listenfd,(struct sockaddr*)(&address),sizeof(struct sockaddr))<0)
  {
    printf("bind error,%d:%s\n",errno,strerror(errno));
  }
  if(listen(listenfd,5)<0)
  {
    printf("listen error,%d:%s\n",errno,strerror(errno));
  }
  struct client_data *users=(struct client_data *)calloc(0,sizeof(struct client_data)*FD_LIMIT);
  struct pollfd fds[USER_LIMIT+1];
  int user_count=0;
  int i;
  for(i=1;i<USER_LIMIT+1;++i)
  {
    fds[i].fd=-1;
    fds[i].events=0;
  }
  fds[0].fd=listenfd;
  fds[0].events=POLLIN | POLLERR;
  fds[0].revents=0;


  while(1)
  {
    int ret;
    ret=poll(fds,user_count+1,-1);
    if(ret<0)
    {
      printf("poll error,%d:%s\n",errno,strerror(errno));
      break;
    }
   
    for(i=0;i<user_count+1;++i)
    {
      if((fds[i].fd==listenfd) && (fds[i].revents & POLLIN))
      {
        struct sockaddr_in clientaddress;
        socklen_t clientaddrlength=sizeof(struct sockaddr_in);
        int connfd=accept(listenfd,(struct sockaddr*)(&clientaddress),&clientaddrlength);
        if(connfd<0)
        {
          printf("accept error,%d:%s\n",errno,strerror(errno));
          continue;
        }
        if(user_count>=USER_LIMIT)
        {
          //还有一些没写的，测试的时候不需要之后补上
          continue;
        }
        user_count++;
        users[connfd].address=clientaddress;
        setnoblock(connfd);
        fds[user_count].fd=connfd;
        fds[user_count].revents=0;
        fds[user_count].events=POLLIN | POLLHUP | POLLERR;
        printf("Here comes a new user,now we have %d users\n",user_count);
      }
      else if(fds[i].revents & POLLERR)
      {
        printf("error!");
        continue;
      }
      else if(fds[i].revents & POLLRDHUP)
      {
        continue;
      }
      else if(fds[i].revents & POLLIN)
      {
        int fd=fds[i].fd;
        memset(users[fd].buf,'\0',BUFFER_SIZE);
        ret = recv(fd,users[fd].buf,BUFFER_SIZE-1,0);
        printf("get %d bytes of client data %s form %d\n",ret,users[fd].buf,fd);
        if(ret<0)
        {
          printf("recv error,%d:%s\n",errno,strerror(errno));
          continue;
        }
        else if(ret==0)
        {

        }
        else
        {
          int j;
          for( j = 1;j<=user_count;++j)
          {
            if(fds[j].fd==fd)
            {
              continue;
            }
            fds[j].events |=~POLLIN;
            fds[j].events |=POLLOUT;
            users[fds[j].fd].write_buf=users[fd].buf;
          }
        }
      }
      else if(fds[i].revents & POLLOUT)
      {
        int fd=fds[i].fd;
        if(!users[fd].write_buf)
        {
          continue;
        }
        ret=send(fd,users[fd].write_buf,strlen(users[fd].write_buf),0);
        users[fd].write_buf=NULL;
        fds[i].events |=~POLLOUT;
        fds[i].events |=POLLIN;
      }
    }
  }
  free(users);
  close(listenfd);
  return 0;
}
