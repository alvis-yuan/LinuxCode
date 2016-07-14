#include "comm.h"



int main()
{
  int pid=fork();
  if(pid>0)
  {
    //father
    int shm_id=create_shm(4096);
    char *buf=shm_at(shm_id);
    int i=0;
    while(i<4096)
    {
      sleep(3);
      buf[i]='A';
      i++;
      buf[i]='\0';
    }
  }
  else
  {
    //child
    int shm_id=get_shm();
    char *buf=shm_at(shm_id);
    while(1)
    {
      sleep(3);
      printf("%s\n",buf);
    }
  }
  return 0;
}
