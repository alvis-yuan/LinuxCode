#include "comm.h"
long c_type=1;
long s_type=22;
int main()
{
  int msg_id=create_msg_set();
  char buf[_SIZE_];

  while(1)
  {
    memset(buf,'\0',sizeof(buf));
    receive_msg(msg_id,c_type,buf);
    if(strcasecmp(buf,"quit")==0)
    {
      break;
    }
    printf("client # %s\n",buf);
    printf("clent say done ! Please Input:");
    fflush(stdout);
    memset(buf,'\0',sizeof(buf));
    send_msg(msg_id,c_type,buf);
  }
  destory_msg_set(msg_id);
  return 0;
}
