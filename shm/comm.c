#include "comm.h"


static int comm_create_shm(int flags,size_t size)
{
  key_t _key=ftok(_PATH_NAME_,_PROJ_ID_);
  if(_key<0)
  {
    printf("%d:%s",errno,strerror(errno));
  }
  int shm_id;
  if((shm_id=shmget(_key,size,flags))<0)
  {
    printf("shmget error,%d:%s",errno,strerror(errno));
  }
  return shm_id;
}


int create_shm(size_t size)
{
  int flags=IPC_CREAT |IPC_EXCL;
  return comm_create_shm(flags,size);
}

int get_shm()
{
  int flags=IPC_CREAT;
  return comm_create_shm(flags,0);
}

char *shm_at(int shm_id)
{
  return (char *)shmat(shm_id,NULL,0);
}
int shm_dt(char *addr)
{
  return shmdt(addr);
}

void destory_shm(int shm_id)
{
  shmctl(shm_id,IPC_RMID,0);
}
