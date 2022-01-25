#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <omp.h>
#include <stdatomic.h>

#define ALIGN 64
#define N 1000000
#define N_SAMPLES 100

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

double omp_reduc(double *restrict a, uint64_t n)
{
  double res = 0.0;

#pragma omp parallel shared(res)
  {
#pragma omp for reduction(+:res)
    for (uint64_t i = 0; i < n; i++)
      res += a[i];
  }

  return res;
}

int main(int argc, char **argv)
{
  double *restrict a = aligned_alloc(ALIGN, sizeof(double) * N);

  for (uint64_t i = 0; i < N; i++)
    {
      a[i] = i;
    }

  double res = omp_reduc(a, N);

  free(a);

  return 0;
}
