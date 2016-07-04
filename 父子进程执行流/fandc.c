#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>


//一共输出多少个'-'

int main()
{
  int i;
  for(i=0;i<2;i++)
  {
    fork();
    printf("-\n");
  }
  return 0;
}
