#include<stdio.h>
#include<string.h>
#include<unistd.h>

void proc()
{
  int rate=0;
  const char *running="|/-\\";
  char p[102];
  memset(p,'\0',sizeof(p));

  while(rate<=100)
  {
    p[rate]='#';
    printf("[%-102s][%d%%][%c]\r",p,rate,running[rate%4]);
    rate++;
    fflush(stdout);
    sleep(1);
  }
}





int main()
{
  proc();
  return 0;
}
