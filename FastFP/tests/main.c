#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ALIGN 64
#define N 1000000
#define N_SAMPLES 100

void dotprod(double *restrict a, double *restrict b, double *restrict c, const uint64_t n)
{
  for (uint64_t i = 0; i < n; i++)
    c[i] = a[i] * b[i];
}

double reduc(double *restrict c, const uint64_t n)
{
  double res = 0.0;

  for (uint64_t i = 0; i < n; i++)
    res += c[i];

  return res;
}

int main(int argc, char **argv)
{
  double *restrict a = aligned_alloc(ALIGN, sizeof(double) * N);
  double *restrict b = aligned_alloc(ALIGN, sizeof(double) * N);
  double *restrict c = aligned_alloc(ALIGN, sizeof(double) * N);

  for (uint64_t i = 0; i < N; i++)
    {
      a[i] = 4.97e-21 + i * 1.0e-21;
      b[i] = 3.8e-34 + i * 1.0e-34;
      c[i] = 0;
    }

  for (uint64_t i = 0; i < N_SAMPLES; i++)
    dotprod(a, b, c, N);

  double res = reduc(c, N);
  printf("Reduc: %e\n", res);

  free(c);
  free(b);
  free(a);

  return 0;
}
