#include "comm.h"


long c_type=1;
long s_type=2;

int main()
{
  int msg_id = get_msg_set();

  char buf[_SIZE_];

  while(1)
  {
    memset(buf,'\0',sizeof(buf));
    send_msg(msg_id,c_type,buf);
    if(strcasecmp(buf,"quit")==0)
    {
      break;
    }
    memset(buf,'\0',sizeof(buf));
    receive_msg(msg_id,c_type,buf);
    printf("server # %s\n",buf);
    printf("server say done ! Please Input:");
    fflush(stdout);
  }

  return 0;
}
