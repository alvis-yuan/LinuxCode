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
#include <vector>
#include <signal.h>
#include "cJSON.h"

using namespace std;



#define BUFFSIZE 512
#define DEFAULTPORT 8001
#define FILELISTSIZE 1024



struct FileList
{
  FileList()
    :size(0)
  {

  }
  void Add(char *filepath,long long filesize)
  {
    FilePath[size]=filepath;
    FileSize[size]=filesize;
    size++;
  }
  void Delete(char *filepath)
  {
    vector<string>::iterator it;
    vector<long long>::iterator itsize;
    it = FilePath.begin();
    itsize=FileSize.begin();
    for(;it!=FilePath.end();)
    {
      if(*it==filepath)
      {
        it=FilePath.erase(it);
        itsize=FileSize.erase(itsize);
        break;
      }
      else
      {
        it++;
        itsize++;
      }
    }
    size--;
  }
  void change(char *filepath,long long filesize)
  {
  }
  bool operator==(FileList &fl)
  {
    if(size!=fl.size)
    {
      return false;
    }
    for(int i=0;i<size;++i)
    {
      if(FilePath[i]!= fl.FilePath[i] || FileSize[i]!=fl.FileSize[i] )
      {
        return false;
      }
    }
    return true;
  }
    vector<string> FilePath;
    vector<long long> FileSize;
    size_t size;
};


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
    printf("accept error%d:%s\n",errno,strerror(errno));
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

int FileTransport(const char * filepath,int sockConn)
{
  printf("I will open %s\n",filepath);
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


void SendFileList(int sockConn,struct FileList& fl)
{
  char *data=NULL;
  cJSON *filepath=cJSON_CreateArray();
  cJSON *filesize=cJSON_CreateArray();
  for(int i=0;i<fl.size;++i)
  {
    cJSON_AddItemToArray(filepath,cJSON_CreateString(fl.FilePath[i].c_str()));
  }
  data=cJSON_Print(filepath);
  printf("the filepath is %s\n",data);
  send(sockConn,data,FILELISTSIZE,0);

  recv(sockConn,data,FILELISTSIZE,0);//ackowledge receive


  char *size;  
  for(int i=0;i<fl.size;++i)
  {
    cJSON_AddItemToArray(filesize,cJSON_CreateNumber(fl.FileSize[i]));
  }
  size=cJSON_Print(filesize);

  printf("the file size is %s\n",size);

  send(sockConn,size,FILELISTSIZE,0);
}




void mainstream()
{
  FileList *Serverfl=new FileList;

  //this is a test
  int filefd=open("./SyncFloderServer/test.pdf",O_RDONLY);
  Serverfl->Add("./SyncFloderServer/test.pdf",0);
  filefd=open("./SyncFloderServer/test1.txt",O_RDONLY);
  Serverfl->Add("./SyncFloderServer/test1.txt",0);
  char signals[1024]={'\0'};  
  //test is over
  
  //try to connect client
  struct sockaddr_in server_addr;
  int sockfd;
  sockfd=BindSocket("192.168.84.128",DEFAULTPORT,server_addr);
  int sockConn=ConnectToClient(sockfd,server_addr);

  while(1)
  {
    recv(sockConn,signals,1023,0);

    printf("recv signal success:%s\n",signals);


    cJSON *root=cJSON_Parse(signals);
    int sig=cJSON_GetObjectItem(root,"signal")->valueint;
  
    printf("the sig is %d\n",sig);
  


  
    if(sig==1)
    {
      //request for filelist
      SendFileList(sockConn,*Serverfl);
      printf("send filelist complete!\n");
    }
    if(sig==2)
    {
      //requesr for downloadfile
      for(int i=0;i<Serverfl->size;++i)
      {
        FileTransport(Serverfl->FilePath[i].c_str(),sockConn);
      }
    }
  }
  ShutDownConnect(sockConn);
  CloseSocket(sockfd);
}


int main()
{
  mainstream();
  return 0;
}
