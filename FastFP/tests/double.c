#include <stdio.h>

int main(int argc, char **argv)
{
  double a = 4.97e-21;
  double b = 3.8e-34;
  double c = a * b;

  printf("%e\n", c);

  return 0;
}
