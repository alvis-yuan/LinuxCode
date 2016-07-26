#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> //inet_ntoa()函数的头文件
#include <unistd.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


using namespace std;



#define BUFFSIZE 512
#define DEFAULTPORT 8001


int BindSocket(char *address,int port,struct sockaddr_in& server_addr)
{
  int sockfd;
  bzero(&server_addr,sizeof(sockaddr_in));
  server_addr.sin_addr.s_addr=inet_addr(address);
  server_addr.sin_port=htons(port);
  server_addr.sin_family=AF_INET;
  if((sockfd=socket(PF_INET,SOCK_STREAM,0)) == -1)
  {
    printf("socket error");
    return -1;
  }
  if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr)) == -1)
  {
    printf("bind error ");
    return -1;
  }
  if(listen(sockfd,5) == -1)
  {
    printf("listen error");
    return -1;
  }
  return sockfd;
}


int ConnectToClient(int sockfd,struct sockaddr_in& server_addr)
{
  socklen_t len;
  int clientfd;
  clientfd=accept(sockfd,(struct sockaddr *)(&server_addr),&len);
  if(clientfd == -1)
  {
    printf("accept error\n");
    return -1;
  }
  printf("connect success\n");
  return clientfd;
}


void ShutDownConnect(int clientfd)
{
  close(clientfd);
}

void CloseSocket(int sockfd)
{
  close(sockfd);
}

int FileTransport(char * filepath,int sockConn)
{
  int filefd=open(filepath,O_RDONLY);
  if(filefd<0)
  {
    printf("file open error!\n");
    return -1;
  }
  char sendBuf[BUFFSIZE]={'\0'};
  while(1)
  {
    memset(sendBuf,'\0',BUFFSIZE);
    int len = read(filefd,sendBuf,BUFFSIZE);
    send(sockConn,sendBuf,len,0);
    if(len < 1)
    {
      break;
    }
  }
  return 0;
}
int RecviveFile(char *filepath,int clientfd)
{
  char recvBuf[BUFFSIZE]={'\0'};
  int filefd;
  long long totallength = 0;
  filefd = open(filepath,O_RDWR|O_CREAT,0777);
  while(1)
  {
    int len = recv(clientfd,recvBuf,BUFFSIZE,0);
    if(len<1)
    {
      break;
    }
    write(filefd ,recvBuf ,BUFFSIZE);
    memset(recvBuf,'\0',BUFFSIZE);
    totallength+=len;
  }
  int num=lseek(filefd,0,SEEK_END);
  ftruncate(filefd,totallength);
}
int main()
{
  struct sockaddr_in server_addr;
  int sockfd;
  sockfd=BindSocket("192.168.84.128",DEFAULTPORT,server_addr);
  int sockConn=ConnectToClient(sockfd,server_addr);
  FileTransport("test5.txt",sockConn);
  ShutDownConnect(sockConn);
  return 0;
}
