#include<stdio.h>                                                                

#include<stdlib.h>

void bug()
{
  printf("haha I ma a bug!!");
  exit(100);
}
int func(int x, int y)
{
  int *p = &x;
  p++;
  
  *p=bug;
  printf("x=%d,y=%d\n",x,y);
  int c = 0xcccc;
  return c;
}



int main()
{

  printf("I am main\n");
  int a = 0xaaaa;
  int b = 0xbbbb;
  func(a, b);
  printf("I should run here\n");
  return 0;
}
