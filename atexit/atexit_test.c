#include<stdio.h>
#include<stdlib.h>


void func1()
{
  printf("this is func1\n");
}

void func4()
{
  printf("this is func4\n");

}


void func3()
{
  printf("this is func3\n");
}


void func2()
{
  printf("this is func2\n");
}

void func0()
{
  printf("this is func0\n");
}
void func6()
{
  printf("this is func6\n");
}
void a()
{
  printf("this is a\n");
}
void z()
{
  printf("this is z\n");
}


int main()
{
  atexit(func3);
  atexit(func1);
  atexit(func2);
  
  atexit(func4);
  atexit(func0);
  atexit(func6);
  atexit(a);
  atexit(z);
  return 0;
}
