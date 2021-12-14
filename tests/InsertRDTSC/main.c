#include <stdint.h>
#include <stdlib.h>

#define ALIGN 64
#define N 1000000
#define N_SAMPLES 100

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

void dotprod(double *restrict a, double *restrict b, double *restrict c, uint64_t n)
{
  for (uint64_t i = 0; i < n; i++)
    c[i] = a[i] * b[i];
}

int main(int argc, char **argv)
{
  double *restrict a = aligned_alloc(ALIGN, sizeof(double) * N);
  double *restrict b = aligned_alloc(ALIGN, sizeof(double) * N);
  double *restrict c = aligned_alloc(ALIGN, sizeof(double) * N);

  for (uint64_t i = 0; i < N; i++)
    {
      a[i] = i;
      b[i] = i / N;
      c[i] = 0;
    }

  for (uint64_t i = 0; i < N_SAMPLES; i++)
    dotprod(a, b, c, N);

  free(c);
  free(b);
  free(a);

  return 0;
}
