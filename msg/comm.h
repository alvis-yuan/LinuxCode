#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>


#define _PATH_NAME_ "/tmp"
#define _PROJ_ID_ 0x666
#define _SIZE_ 1024

static int comm_create_msg_set(int flags);
int create_msg_set();
int get_msg_set();
void destory_msg_set(int msg_id);
void send_msg(int msg_id,long msgtype,char * buf);
void receive_msg(int msg_id,long msgtype,char *buf);



struct msgbuf
{
  long mtype;
  char mtext[_SIZE_];
};


