#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <libgen.h>


void eatblank(char **buf)
{
  while(**buf==' ')
  {
    (*buf)++;
  }
}


void GetHostName(char *hostname,int length)
{
  gethostname(hostname,length);
  char *p=hostname;
  while(*p!='\0')
  {
    if(*p=='.')
    {
      *p='\0';
    }
    p++;
  }
}

void Pipe(char **my_argv,char *buf);
void BuildCommand(char **my_argv,char *buf)
{
  eatblank(&buf);
  my_argv[0]=buf;
  int index=1;
  char *p=buf;
      while(*p!='\0')
      {
        if(*p==' ')
        {
          *p='\0';
          p++;
          eatblank(&p) ;
          if(*p!='|')
          {
            my_argv[index++]=p;
          }
          continue;
        }
        else if(*p=='|')
        {
          p++;
          //p++;
          my_argv[index]=NULL;
          Pipe(my_argv,p);
        }
        else
        {
          p++;
        }
      }
     my_argv[index]=NULL;
}



void Pipe(char ** my_argv,char *buf)
{
  int fd[2];
  pipe(fd);
  pid_t id2=fork();
  if(id2==0)
  {
    close(1);
    dup(fd[1]);
    close(fd[1]);
    close(fd[0]);
    execvp(my_argv[0],my_argv);
  }
  else
  {
    waitpid(id2,NULL,0);
    close(0);
    dup(fd[0]);
    close(fd[0]);
    close(fd[1]);
    BuildCommand(my_argv,buf);
    execvp(my_argv[0],my_argv);
  }
  //在此处添加exec族函数
}


int main()
{
  while(1)
  {
    char *my_argv[64];
      struct passwd *pwd=getpwuid(getuid());
      char hostname[256]={'\0'};
      char cwd[256]={'\0'};
      getcwd(cwd,256);
      GetHostName(hostname,256);
      printf("[%s@%s %s]#",pwd->pw_name,hostname,basename(cwd));
      fflush(stdout);
      char buf[1024];
      buf[0]='\0';

      int count=read(0,buf,sizeof(buf));
      buf[count-1]='\0';
      my_argv[0]=buf;    
    pid_t id=fork();
    if(id==0)
    {
      //child
     
      if(strncmp(buf,"cd",2)==0) 
      {
        exit(1);
      }
      BuildCommand(my_argv,buf);
     execvp(my_argv[0],my_argv);
     printf("if the process has some problem ,I should run here\n");
     exit(0);
    }
    else
    {
      //father
      int status=0;
      wait(&status);
      if(status==256)
      {
        my_argv[0]+=3;
        chdir(my_argv[0]);
      }
    }
  }
  return 0;
}
