#include <stdio.h>
#include <iostream>
#include <sys/un.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "cJSON.h"


using namespace std;
#define DEFAULTPORT 8001
#define FILELISTSIZE 1024
#define BUFFSIZE 512
#define JSONSIZE 2048
#define SIGSIZE 17

#define UPDATERATE 5


char fileend[BUFFSIZE]={'\0'};



void DownloadFile(const char *filepath,int clientfd)
{

  printf("I will downloadfile %s!\n",filepath);

  

  char recvBuf[JSONSIZE]={'\0'};
  int filefd;
  long long totallength = 0;
  filefd = open(filepath,O_RDWR|O_CREAT,0777);
  while(1)
  {

    cJSON *transgram;

    int test=recv(clientfd,recvBuf,JSONSIZE,0);
    printf("made made meici de test shi duo shao a %d\n",test);
    printf("I get json is %s\n",recvBuf);


    send(clientfd,"acknowledge",BUFFSIZE,0);



    transgram=cJSON_Parse(recvBuf);
    int len=cJSON_GetObjectItem(transgram,"length")->valueint;

    if(len==0)
    {
      printf("I try to break\n");
      break;
    }
    write(filefd ,cJSON_GetObjectItem(transgram,"datapack")->valuestring ,len);
    memset(recvBuf,'\0',JSONSIZE);
    totallength+=len;
  }
  printf("\nthe %s file size is %d\n",filepath,totallength);
  int num=lseek(filefd,0,SEEK_END);
 ftruncate(filefd,totallength);
}


int UpdateFile(const char *filepath,int sockConn)
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

    printf("the length of sendjson is %d\n",strlen(sendjson)+1);
    printf("WTF the json is %s \n",sendjson);


    send(sockConn,sendjson,strlen(sendjson)+1,0);

    recv(sockConn,sendBuf,BUFFSIZE,0);


    memset(sendBuf,'\0',BUFFSIZE);
    if(len < 1)
    {
      printf("this may be 0%s\n",sendBuf);
    //  send(sockConn,sendjson,BUFFSIZE,0);
      break;
    }
  }
  return 0;
}











struct FileList
{
  FileList()
    :size(0)
  {

  }
  void Add(const char *filepath,long long filesize)
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


class Sync
{
public:
  Sync(int serverconnection)
    :sockConn(serverconnection)
  {
    InitLocalfl();
    InitServerfl();

  }
  void InitServerfl()
  {
    Serverfl.FilePath.resize(10);
    Serverfl.FileSize.resize(10);
  }
  void InitLocalfl()
  {
    mkdir("SyncFloderServer",0777);
    Localfl.FilePath.resize(10);
    Localfl.FileSize.resize(10);

  }
  void RequestServerfl(int sockConn)
  {
    char *signals;
    cJSON *root=cJSON_CreateObject();
    cJSON_AddItemToObject(root,"signal",cJSON_CreateNumber(1));
    signals=cJSON_Print(root);
    send(sockConn,signals,SIGSIZE,0);


    char data[FILELISTSIZE]={'\0'};
    int len=recv(sockConn,data,FILELISTSIZE,0);

    printf("the fileparh is %s\n",data);

    send(sockConn,data,FILELISTSIZE,0);//acknowledge receive
    cJSON *filepath=cJSON_Parse(data);

    int sizeofarray=cJSON_GetArraySize(filepath);


    Serverfl.size=sizeofarray;

    for(int i=0;i<sizeofarray;++i)
    {
      char *temp=cJSON_GetArrayItem(filepath,i)->valuestring;


      Serverfl.FilePath[i]=*(new string(temp));
      


    }


    memset(data,'\0',FILELISTSIZE);
    len=recv(sockConn,data,FILELISTSIZE,0);


    cJSON *filesize=cJSON_Parse(data);
    for(int i=0;i<sizeofarray;++i)
    {
      int temp=cJSON_GetArrayItem(filesize,i)->valueint;
      Serverfl.FileSize[i]=temp;

    }
  }
  void FileSync()//专门认为是从服务器端下载到客户端
  {
    RequestServerfl(sockConn);
    
    printf("serverfl size is %d\n",Serverfl.size);
    printf("localfl size is %d\n",Localfl.size);


    if(!(Localfl == Serverfl))
    {
      //need to sync
      
      printf("I need to sync!!!\n");

      char *signals;
      cJSON *root=cJSON_CreateObject();
      cJSON_AddItemToObject(root,"signal",cJSON_CreateNumber(2));
      signals=cJSON_Print(root);
      send(sockConn,signals,SIGSIZE,0);

      for(int i=0;i<Serverfl.size;++i)
      {



          DownloadFile(Serverfl.FilePath[i].c_str(),sockConn);


          printf("download success!next one\n");

          Localfl.FilePath[i]=Serverfl.FilePath[i];
          Localfl.FileSize[i]=Serverfl.FileSize[i]; 
      }
      Localfl.size=Serverfl.size;
    }
    //no need to sync
    else
    {
      //do nothing
      printf("I don't need to sync ,I choose to do nothing\n");
    }
  }
  void FileUpdate(const char *filepath,int filesize)//专门认为是从客户端上传到服务器端
  {
    //add or delete or change will do this
    char *signals;
    cJSON *root=cJSON_CreateObject();
    cJSON_AddItemToObject(root,"signal",cJSON_CreateNumber(3));
    signals=cJSON_Print(root);
    send(sockConn,signals,SIGSIZE,0);

    Localfl.Add(filepath,filesize);


    cJSON *fileinfo=cJSON_CreateObject();
    cJSON_AddItemToObject(fileinfo,"filepath",cJSON_CreateString(filepath));
    cJSON_AddItemToObject(fileinfo,"filesize",cJSON_CreateNumber(filesize));
    char *fileinfosend=cJSON_Print(fileinfo);
    send(sockConn,fileinfosend,BUFFSIZE,0);


    UpdateFile(filepath,sockConn);
  }

  void SyncAdd(char *filepath,long long filesize)
  {
    Localfl.Add(filepath,filesize);
    FileUpdate(filepath,filesize);
  }
  void SyncDelete();
  void SyncChange();
  void SyncLoop()
  {
    while(1)
    {
      sleep(UPDATERATE);
      FileSync();
    }
  }

private:
  int sockConn;
  FileList Localfl;
  FileList Serverfl;
};



int ConnectToServer(char *address,int port,struct sockaddr_in& server_addr)
{
  server_addr.sin_addr.s_addr=inet_addr(address);
  server_addr.sin_port=htons(port);
  server_addr.sin_family=AF_INET;
  int sockfd=socket(PF_INET,SOCK_STREAM,0);
  if(sockfd<0)
  {
    printf("socket error!\n");
    return -1;
 
  }
  if(connect(sockfd,(struct sockaddr*)(&server_addr),sizeof(struct sockaddr)) == -1)
  {
    printf("connect error!\n");
    return -1;
  }
  printf("coonnect success!\n");
  return sockfd;
}


void mainstream()
{

  struct sockaddr_in server_addr;
  int sockConn=ConnectToServer("192.168.84.128",DEFAULTPORT,server_addr);
  Sync *cli=new Sync(sockConn);
  //cli->SyncAdd("./SyncFloderServer/test3.txt",0);
  cli->SyncLoop();
}




int main()
{
  mainstream();
  return 0;
}

