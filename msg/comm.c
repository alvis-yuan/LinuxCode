#include "comm.h"
static int comm_create_msg_set(int flags)
{
  key_t _key=ftok(_PATH_NAME_,_PROJ_ID_);
  if(_key<0)
  {
    printf("%d:%s",errno,strerror(errno));
  }
  int msg_id=msgget(_key,flags);
  if(msg_id<0)
  {
    printf("%d:%s",errno,strerror(errno));
  }
  return msg_id;
}


int get_msg_set()
{
  key_t _key=ftok(_PATH_NAME_,_PROJ_ID_);
  int flags=IPC_CREAT;
  return comm_create_msg_set(flags);
}


int create_msg_set()
{
  key_t _key=ftok(_PATH_NAME_,_PROJ_ID_);
  int flags=IPC_CREAT | IPC_EXCL;
  return comm_create_msg_set(flags);
}


void send_msg(int msg_id,long msgtype,char *buf)
{
  memset(buf,'\0',strlen(buf)+1);
  ssize_t _size=read(0,buf,_SIZE_);
  if(_size>0)
  {
    buf[_size-1]='\0';
  }

  struct msgbuf _mbuf;
  memset(&_mbuf,'\0',sizeof(struct msgbuf));
  _mbuf.mtype=msgtype;
  strcpy(_mbuf.mtext,buf);
  if(msgsnd(msg_id,&_mbuf,_size,0)<0)
  {
    printf("send error,%d:%s",errno,strerror(errno));
  }
}

void receive_msg(int msg_id , long msgtype ,char *buf)
{
  struct msgbuf _mbuf;
  memset(&_mbuf,'\0',sizeof(struct msgbuf));
  _mbuf.mtype=0;
  if(msgrcv(msg_id,&_mbuf,_SIZE_,msgtype,0)<0)
  {
    printf("recv error %d:%s",errno,strerror(errno));
  }
  strcpy(buf,_mbuf.mtext);
}

void destory_msg_set(int msg_id)
{
  if(msgctl(msg_id,IPC_RMID,0)<0)
  {
    printf("%d:%s",errno,strerror(errno));
  }
}
