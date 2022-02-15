#include <stdio.h>       // fprintf
#include <stdlib.h>      // aligned_alloc
#include <stdint.h>      // uint64_t
#include <unistd.h>      // syscall, getpid
#include <sys/syscall.h> // syscall
#include <sys/types.h>   // pid_t
#include <sys/sysinfo.h> // get_nprocs
#include <sys/sysinfo.h> // get_nprocs_conf
#include <string.h>      // strcpy

#include "runtime.h"

void libinsertrdtsc_initialize()
{
  __func = aligned_alloc(ALIGN, sizeof(function_t) * CAPTURE_LIMIT);
  __glob_func = aligned_alloc(ALIGN, sizeof(global_function_t) * CAPTURE_LIMIT);

  for (uint64_t i = 0; i < CAPTURE_LIMIT; i++)
    {
        __func[i].pid = 0;
        __func[i].tid = 0;
        __func[i].proc_id_start = 0;
        __func[i].proc_id_end = 0;
        __func[i].cycles = 0;
        strcpy(__func[i].func_name, "");

        __glob_func[i].ncalls = 0;
        __glob_func[i].nthreads = 0;
        __glob_func[i].cycles = 0;
        __mod.core_switch_ratio = 0.0;
        strcpy(__glob_func[i].func_name, "");
    }

  __mod.nprocs = get_num_procs();
  __mod.nprocs_avail = get_num_procs_available();
  __mod.nthreads = 0;
  __mod.nfuncs = 0;
}

void libinsertrdtsc_finalize()
{
  free(__func);
  free(__glob_func);
}

void insert_function(uint64_t index, uint64_t pid, uint64_t tid,
                     uint64_t proc_id_start, uint64_t proc_id_end,
                     uint64_t cycles, char *func_name)
{
  __func[index].pid = pid;
  __func[index].tid = tid;
  __func[index].proc_id_start = proc_id_start;
  __func[index].proc_id_end = proc_id_end;
  __func[index].cycles = cycles;
  strcpy(__func[index].func_name, func_name);
}

void analyze_function(uint64_t n)
{
  // Core switch ratio
  uint64_t count_csr = 0;

  for (uint64_t i = 0; i < n; i++)
    {
      // Count threads on module
      uint64_t found = 0;

      for (uint64_t j = 0; j < __mod.nthreads; j++)
        {
          if (__func[i].tid == __mod.list_threads[j])
            {
              found = 1;
              break;
            }
        }

      if (!found)
        {
          __mod.list_threads[__mod.nthreads] = __func[i].tid;
          __mod.nthreads++;
        }

      // Count functions
      found = 0;

      for (uint64_t j = 0; j < __mod.nfuncs; j++)
        {
          if (strcmp(__func[i].func_name, __glob_func[j].func_name) == 0)
            {
              found = 1;
              break;
            }
        }

      if (!found)
        {
          strcpy(__glob_func[__mod.nfuncs].func_name, __func[i].func_name);
          __mod.nfuncs++;
        }

      // Count calls
      for (uint64_t j = 0; j < __mod.nfuncs; j++)
        {
          if (strcmp(__func[i].func_name, __glob_func[j].func_name) == 0)
            {
              __glob_func[j].ncalls++;
            }
        }

      // Count thread
      for (uint64_t j = 0; j < __mod.nfuncs; j++)
        {
          if (strcmp(__func[i].func_name, __glob_func[j].func_name) == 0)
            {
              found = 0;

              for (uint64_t k = 0; k < __glob_func[j].nthreads; k++)
                {
                  if (__func[i].tid == __mod.list_threads[k])
                    {
                      found = 1;
                      break;
                    }
                }

              if (!found)
                {
                  __glob_func[j].list_threads[__glob_func[j].nthreads] = __func[i].tid;
                  __glob_func[j].nthreads++;
                }
            }
        }

      // Cycle
      for (uint64_t j = 0; j < __mod.nfuncs; j++)
        {
          if (strcmp(__func[i].func_name, __glob_func[j].func_name) == 0)
            {
              __glob_func[j].cycles = __glob_func[j].cycles < __func[i].cycles ? __func[i].cycles : __glob_func[j].cycles;
            }
        }

      // Core switch ratio
      if (__func[i].proc_id_start != __func[i].proc_id_end)
        count_csr++;
    }

  // Update core switch ratio
  __mod.core_switch_ratio = (double)count_csr / (double)n;
}

void write_function_info(uint64_t n)
{
  //
  FILE *file = fopen("output-insert-rdtsc.raw", "w");

  if (!file)
    exit(13);

  //
  fflush(file);

  //
  fprintf(file,
          "============================= FULL FUNCTIONS INFO =============================\n"
          "\n"
          "%16s  %16s  %16s  %16s  %16s  %s"
          "\n",
          "PID",
          "TID",
          "CORE ID START",
          "CORE ID END",
          "CYCLES",
          "FUNCTION NAME");

  //
  for (uint64_t i = 0; i < n; i++)
    {
      fprintf(file, "%16ld  %16ld  %16ld  %16ld  %16ld  %s\n",
              __func[i].pid,
              __func[i].tid,
              __func[i].proc_id_start,
              __func[i].proc_id_end,
              __func[i].cycles,
              __func[i].func_name);
    }

  //
  fprintf(file, "\n");
  fflush(file);
  fclose(file);
}

void write_function_info_in_csv_file(uint64_t n)
{
  //
  FILE *file = fopen("output-insert-rdtsc.csv", "w");

  if (!file)
    exit(13);

  //
  fflush(file);

  //
  fprintf(file,
          "PID,TID,CORE ID START,CORE ID END,CYCLES,FUNCTION NAME\n");

  //
  for (uint64_t i = 0; i < n; i++)
    {
      fprintf(file, "%ld,%ld,%ld,%ld,%ld,%s\n",
              __func[i].pid,
              __func[i].tid,
              __func[i].proc_id_start,
              __func[i].proc_id_end,
              __func[i].cycles,
              __func[i].func_name);
    }

  //
  fflush(file);
  fclose(file);
}

void write_function_summary()
{
  //
  fflush(stdout);

  //
  fprintf(stdout,
          "============================== FUNCTIONS SUMMARY ==============================\n"
          "\n"
          "%18s  %18s  %18s  %s"
          "\n",
          "NUMBER OF CALLS",
          "NUMBER OF THREADS",
          "CYCLES MAX",
          "FUNCTION NAME");

  //
  for (uint64_t i = 0; i < __mod.nfuncs; i++)
    {
      fprintf(stdout, "%18ld  %18ld  %18ld  %s\n",
              __glob_func[i].ncalls,
              __glob_func[i].nthreads,
              __glob_func[i].cycles,
              __glob_func[i].func_name);
    }

  //
  fprintf(stdout, "\n");
  fflush(stdout);
}

void write_module_summary(void)
{
  //
  fflush(stdout);

  //
  fprintf(stdout,
          "=============================== MODULE SUMMARY ===============================\n"
          "\n"
          "%28s: %ld\n"
          "%28s: %ld\n"
          "%28s: %ld\n"
          "%28s: %ld\n"
          "%28s: %3.2lf%c\n",
          "number of cores", __mod.nprocs,
          "number of cores available", __mod.nprocs_avail,
          "number of threads appears", __mod.nthreads,
          "number of functions", __mod.nfuncs,
          "core switch ratio",  __mod.core_switch_ratio, '%');

  //
  fprintf(stdout, "\n");
  fflush(stdout);
}

uint64_t rdtsc(void)
{
    uint32_t lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)(((uint64_t)hi << (uint64_t)32) | (uint64_t)lo);
}

void rdtscp(uint64_t *cycles, uint64_t *proc_id)
{
  // rdtscp
  // high cycles : edx
  // low cycles  : eax
  // processor id: ecx

  asm __volatile__
    (
     // Assembler
     "mfence;\n"
     "rdtscp;\n"
     "shl $32, %%rdx;\n"
     "or %%rdx, %%rax;\n"
     "mov %%rax, (%[_cycles]);\n"
     "mov %%ecx, (%[_proc_id]);\n"

     // outputs
     :

     // inputs
     :
     [_cycles] "r" (cycles),
     [_proc_id] "r" (proc_id)

     // clobbers
     :
     "cc", "memory", "%eax", "%edx", "%ecx"
     );

  return;
}

uint64_t get_pid(void)
{
  return (uint64_t)getpid();
}

uint64_t get_tid(void)
{
  return (uint64_t)syscall(SYS_gettid);
}

uint64_t get_num_procs(void)
{
  return (uint64_t)get_nprocs();
}

uint64_t get_num_procs_available(void)
{
  return (uint64_t)get_nprocs_conf();
}
