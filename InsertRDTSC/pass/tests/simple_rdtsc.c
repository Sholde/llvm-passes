#include <stdio.h>
#include <stdint.h>

uint64_t rdtsc(){
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

void print_hello(void)
{
  uint64_t beg = rdtsc();

  printf("--my code-- Hello World !\n");

  uint64_t end = rdtsc();
  printf("print_hello: %lu cycles\n", end - beg);
}

void yo(uint64_t n)
{
  uint64_t beg = rdtsc();

  if (n < 6)
    {
      uint64_t end = rdtsc();
      printf("yo: %lu cycles\n", end - beg);
      return;
    }
  else
    {
      if (n < 9)
        {
          uint64_t end = rdtsc();
          printf("yo: %lu cycles\n", end - beg);
          return;
        }
    }

  uint64_t end = rdtsc();
  printf("yo: %lu cycles\n", end - beg);
  return;
}

int main(int argc, char **argv)
{
  uint64_t beg = rdtsc();

  print_hello();

  yo(5);
  yo(7);

  uint64_t end = rdtsc();
  printf("main: %lu cycles\n", end - beg);

  return 0;
}
