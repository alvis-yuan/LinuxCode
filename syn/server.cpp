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
#define JSONSIZE 2048
#define DEFAULTPORT 8001
#define FILELISTSIZE 1024



struct FileList
{
  FileList()
    :size(0)
  {
    FilePath.resize(10);
    FileSize.resize(10);
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
  int filefd=open(filepath,O_RDONLY);
  if(filefd<0)
  {
    printf("file open error!\n");
    return -1;
  }
  char sendBuf[BUFFSIZE]={'\0'};
  while(1)
  {
    char *sendjson;
    cJSON *transgram=cJSON_CreateObject();

    int len = read(filefd,sendBuf,BUFFSIZE);


    cJSON_AddItemToObject(transgram,"length",cJSON_CreateNumber(len));
    cJSON_AddItemToObject(transgram,"datapack",cJSON_CreateString(sendBuf));
    sendjson=cJSON_Print(transgram);


    send(sockConn,sendjson,strlen(sendjson)+1,0);//>256

    recv(sockConn,sendBuf,BUFFSIZE,0);// aknowledge !


    memset(sendBuf,'\0',BUFFSIZE);//256
    if(len < 1)
    {
      printf("this may be 0%s\n",sendBuf);
    //  send(sockConn,sendjson,BUFFSIZE,0);
      break;
    }
  }
  return 0;
}
int ReceiveFile(char *filepath,int clientfd)
{
  printf("I will downloadfile %s!\n",filepath);

  

  char recvBuf[JSONSIZE]={'\0'};//>256
  int filefd;
  long long totallength = 0;
  filefd = open(filepath,O_RDWR|O_CREAT,0777);
  while(1)
  {

    cJSON *transgram;

    int test=recv(clientfd,recvBuf,JSONSIZE,0);//512>test>256

    printf("the receive is %s\n",recvBuf);

    send(clientfd,"acknowledge",BUFFSIZE,0);



    transgram=cJSON_Parse(recvBuf);
    int len=cJSON_GetObjectItem(transgram,"length")->valueint;

    if(len==0)
    {
      printf("I try to break\n");
      break;
    }
    char *temp=cJSON_GetObjectItem(transgram,"datapack")->valuestring;


    write(filefd ,temp,BUFFSIZE);
    memset(recvBuf,'\0',BUFFSIZE);
    totallength+=len;
  }
  printf("\nthe %s file size is %d\n",filepath,totallength);
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
  send(sockConn,data,FILELISTSIZE,0);

  recv(sockConn,data,FILELISTSIZE,0);//ackowledge receive


  char *size;  
  for(int i=0;i<fl.size;++i)
  {
    cJSON_AddItemToArray(filesize,cJSON_CreateNumber(fl.FileSize[i]));
  }
  size=cJSON_Print(filesize);


  send(sockConn,size,FILELISTSIZE,0);
}




void mainstream()
{
  FileList *Serverfl=new FileList;
  //this is a test
  int filefd=open("./SyncFloderServer/test2.txt",O_RDONLY);
  Serverfl->Add("./SyncFloderServer/test2.txt",0);
  filefd=open("./SyncFloderServer/test3.txt",O_RDONLY);
  Serverfl->Add("./SyncFloderServer/test3.txt",0);
  filefd=open("./SyncFloderServer/test1.txt",O_RDONLY);
  Serverfl->Add("./SyncFloderServer/test1.txt",0);
  filefd=open("./SyncFloderServer/test5.txt",O_RDONLY);
  Serverfl->Add("./SyncFloderServer/test5.txt",0);
  char signals[1024]={'\0'};  
  //test is over
  
  //try to connect client
  struct sockaddr_in server_addr;
  int sockfd;
  sockfd=BindSocket("192.168.84.128",DEFAULTPORT,server_addr);
  int sockConn=ConnectToClient(sockfd,server_addr);


int loopcount=0;


  while(1)
  {
    printf("wating for signals\n\n");
    recv(sockConn,signals,1023,0);

    printf("recv signal success:%s\n",signals);


    cJSON *root=cJSON_Parse(signals);
    memset(signals,'\0',1024);
    int sig=cJSON_GetObjectItem(root,"signal")->valueint;


  
    if(sig==1)
    {
      //request for filelist
      SendFileList(sockConn,*Serverfl);
      printf("send filelist complete!\n");
    }
    else if(sig==2)
    {
      //requesr for downloadfile
      for(int i=0;i<Serverfl->size;++i)
      {
        FileTransport(Serverfl->FilePath[i].c_str(),sockConn);


        printf("file transport success next one!\n");

        sleep(10) ;
      }

    }
    else if(sig==3)
    {
      printf("can i run here\n");

      char fileinforecv[JSONSIZE]={'\0'};


      send(sockConn,"ready",JSONSIZE,0);



      recv(sockConn,fileinforecv,JSONSIZE,0);

      printf("fileinfo recv is %s\n",fileinforecv);

      cJSON *fileinfo=fileinfo=cJSON_Parse(fileinforecv);


      char *filepath=cJSON_GetObjectItem(fileinfo,"filepath")->valuestring;



      int filesize=cJSON_GetObjectItem(fileinfo,"filesize")->valueint;


      Serverfl->Add(filepath,filesize);

      printf("___filelist update complete___\n");


      send(sockConn,"I am ready to send file",JSONSIZE,0);

      ReceiveFile(filepath,sockConn);
    }


    loopcount++;
    printf("\nthis is the %d loop\n",loopcount);
  }
  ShutDownConnect(sockConn);
  CloseSocket(sockfd);
}


int main()
{
  mainstream();
  return 0;
}
