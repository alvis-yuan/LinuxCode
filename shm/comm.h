#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdlib.h>


#define _PATH_NAME_ "/tmp"
#define _PROJ_ID_ 0x6666



static int comm_create_ssm(int flags,size_t size);
int create_shm(size_t size);
int get_shm();
char *shm_at(int shm_id);
void destory_shm(int shm_id);
int shm_dt(char *addr);
