#include "comm.h"



int main()
{
  int pid;
  pid=fork();
  if(pid>0)
  {
    //father
    int sem_id=create_sem(1);
    while(1)
    {
      P_sem(sem_id,0);
      printf("A");
      fflush(stdout);
      sleep(1);
      printf("A");
      fflush(stdout);
      V_sem(sem_id,0);
    }
  }
  else
  {
    //child
    while(1)
    {
      int sem_id=get_sem();
      P_sem(sem_id,0);
      printf("B");
      fflush(stdout);
      sleep(1);
      printf("B");
      fflush(stdout);
      V_sem(sem_id,0);
    }
  }

  return 0;
}
