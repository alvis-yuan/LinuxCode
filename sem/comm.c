#include "comm.h"

union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };


static int comm_create_sem(int flags,int num)
{
  key_t _key=ftok(_PATH_NAME_,_PROJ_ID_);
  if(_key<0)
  {
    printf("%d:%s",errno,strerror(errno));
  }
  int sem_id;
  if((sem_id=semget(_key,num,flags))<0)
  {
    printf("semget errno,%d:%s",errno,strerror(errno));
  }
  return sem_id;
}

int create_sem(int num)
{
  int flags=IPC_CREAT | IPC_EXCL;
  int sem_id=comm_create_sem(flags,num);
  union semun v;
  v.val=1;
  if(semctl(sem_id,0,SETVAL,v)<0)//init
  {
    printf("init error,%d:%s",errno,strerror(errno));
  }
  return sem_id;
}

int get_sem()
{
  int flags=0;
  return comm_create_sem(flags,0);
}


void P_sem(int sem_id,int index)
{
  struct sembuf s;
  s.sem_num=index;
  s.sem_op=-1;
  s.sem_flg=0;
  if(semop(sem_id,&s,1)<0)
  {
    printf("op errro,%d:%s",errno,strerror(errno));
  }
}

void V_sem(int sem_id,int index)
{
  struct sembuf s;
  s.sem_num=index;
  s.sem_op=1;
  s.sem_flg=0;
  if(semop(sem_id,&s,1)<0)
  {
    printf("op error,%d:%s",errno,strerror(errno));
  }

}

void destory_sem(int sem_id)
{
  semctl(sem_id,0,IPC_RMID);
}


