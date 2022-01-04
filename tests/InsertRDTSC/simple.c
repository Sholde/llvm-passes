#include <stdio.h>
#include <stdint.h>

uint64_t rdtsc(){
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

void print_hello(void)
{
  printf("--my code-- Hello World !\n");
}

void yo(uint64_t n)
{
  if (n < 6)
    return;
  else
    if (n < 9)
      return;

  return;
}

int main(int argc, char **argv)
{
  print_hello();

  yo(5);
  yo(7);

  return 0;
}
