#include <stdio.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <signal.h>



void func(int num)
{
  printf("catch signal number is %d",num);

}


void printfpendingsignal(sigset_t *set)
{
  int i;
  for(i=1;i<32;++i)
  {
    if(sigismember(set,i))
    {
      printf("1");

    }
    else
    {
      printf("0");
    }
  }
  printf("\n");
}


int main()
{ 
  sigset_t s,p,o;
  signal(SIGINT,func);
  sigemptyset(&s);
  sigemptyset(&p);
  sigemptyset(&o);
  sigaddset(&s,SIGINT);
  sigprocmask(SIG_SETMASK,&s,&o);
  int count=0;
  while(1)
  {
    sigpending(&p);
    printfpendingsignal(&p);
    sleep(1);
    if(count++==10)
    {
      printf("recover!\n");
      sigprocmask(SIG_SETMASK,&o,NULL);
    }
  }
  return 0;
}
