#include<stdio.h>
#include<unistd.h>


int main()
{
  int i;
  for(i=0;i<2;++i)
  {
    pid_t id=fork();
    if(id==0)
    {
      //child
      printf("child:%d\n",getpid());
    }
    else
    {
      //father
      printf("father:%d\n",getpid());
    }
  }
  return 0;
}

