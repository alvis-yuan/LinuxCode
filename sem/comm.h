#include <stdio.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>
#include <sys//sem.h>
#include <stdlib.h>
#include <errno.h>


#define _PATH_NAME_ "/tmp"
#define _PROJ_ID_ 0x6666

static int comm_create_sem(int flags,int num);
int create_sem(int num);
int get_sem();
void P_sem(int sem_id,int index);
void V_sem(int sem_id,int index);

