#include <stdio.h>

int main(int argc, char **argv)
{
  int a = 0;
  int b = 0;

  if (a > b)
    {
      a = 7;
      b = 8;
    }
  else if (a == b)
    {
      a = 9;
      b = 10;
    }
  else
    {
      a = 11;
      b = 12;
    }

  return 0;
}
