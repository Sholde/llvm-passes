#include <stdio.h>     // printf
#include <stdint.h>    // uint64_t
#include <sys/types.h> // pid_t
#include <stdatomic.h> // atomic_fetch_add

#include "runtime.h"

//
uint64_t INDEX = 0;

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
  //
  uint64_t n = 10;

#pragma omp parallel for
  for (uint64_t i = 0; i < n; i++)
    {
      //
      uint64_t index = atomic_fetch_add(&INDEX, 1);

      // rdtsc
      uint64_t cycles = rdtsc();
      //printf("  cycles (rdtsc): %ld\n", cycles);

      // rdtscp
      uint64_t cycles_start = 0;
      uint64_t proc_id_start = 0;
      rdtscp(&cycles_start, &proc_id_start);
      //printf(" cycles (rdtscp): %ld\n", cycles_start);
      //printf("    processor id: %ld\n", proc_id_start);

      // get_pid
      uint64_t pid = get_pid();
      //printf("      process id: %ld\n", pid);

      // get_tid
      uint64_t tid = get_tid();
      //printf("       thread id: %ld\n", tid);

      // get_num_procs
      uint64_t nprocs = get_num_procs();
      //printf("          nprocs: %ld\n", nprocs);

      // get_num_procs_available
      uint64_t nprocs_available = get_num_procs_available();
      //printf("nprocs available: %ld\n", nprocs_available);

      // rdtscp
      uint64_t cycles_end = 0;
      uint64_t proc_id_end = 0;
      rdtscp(&cycles_end, &proc_id_end);
      //printf(" cycles (rdtscp): %ld\n", cycles_end);
      //printf("    processor id: %ld\n", proc_id_end);

      //
      insert_function(index, pid, tid, proc_id_start, proc_id_end, cycles_end - cycles_start, "main");
    }

  write_module_summary();
  write_function_summary();
  write_function_info(n);
  write_function_info_in_csv_file(n);

  //printf("          nprocs: %ld\n", __mod.nprocs);
  //printf("nprocs available: %ld\n", __mod.nprocs_avail);

  return 0;
}
