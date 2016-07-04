#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>


int main()
{
  pid_t pid;
  pid=vfork();
  if(pid==0)
  {
    //child
    printf("I am child pid:%d\n",getpid());
    
   return 0; 
  }
  else
  {
    //father
    printf("I am father pid:%d\n",getpid());
  }
  return 0;
}
