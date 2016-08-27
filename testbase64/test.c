#include <stdio.h>
#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>
#define BUFFSIZE 512
void CompressFile(const char * filepath)
{
  gzFile temp;
  int filefd;
  char readbuf[BUFFSIZE]={'\0'};
  temp=gzopen("compressedfile.gz","wb9");
  filefd=open(filepath,O_RDONLY);
  while(1)
  {
    int len=read(filefd,readbuf,BUFFSIZE);
    if(len<1)
    {
      break;
    }
    gzwrite(temp,readbuf,len);
    memset(readbuf,'\0',BUFFSIZE);
  }
  gzclose(temp);
}
void UnCompressFile(const char *filepath)
{
  printf("hello\n");
  gzFile temp;
  int filefd;
  char readbuf[BUFFSIZE]={'\0'};
  temp=gzopen("compressedfile.gz","rb9");
  filefd=open(filepath,O_WRONLY | O_CREAT);
  while(1)
  {
    int len=gzread(temp,readbuf,BUFFSIZE);
    if(len<1)
    {
      break;
    }
    write(filefd,readbuf,len);
    memset(readbuf,'\0',BUFFSIZE);
  }
  gzclose(temp);
}





int main()
{
  CompressFile("./test7.txt");
  UnCompressFile("./try.txt");
  return 0;
}
