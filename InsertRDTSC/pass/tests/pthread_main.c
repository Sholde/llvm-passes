#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>

#define _GNU_SOURCE
#include <unistd.h>

#define ALIGN 64
#define N_THREAD 10

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

void *thread_func(void *arg)
{
  printf("tid: %d\n", gettid());
  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t tid[N_THREAD];

  for (uint64_t i = 0; i < N_THREAD; i++)
    pthread_create(tid + i, NULL, thread_func, NULL);

  for (uint64_t i = 0; i < N_THREAD; i++)
    pthread_join(tid[i], NULL);

  return 0;
}
