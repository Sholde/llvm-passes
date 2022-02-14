#ifndef _RUNTIME_H_
#define _RUNTIME_H_

//
#include <stdint.h>    // uint64_t
#include <sys/types.h> // pid_t

//
#define ALIGN 32
#define CAPTURE_LIMIT 10000

/**
 * Store function information (a call)
 */
typedef struct function_s
{
  uint64_t pid;
  uint64_t tid;
  uint64_t proc_id_start;
  uint64_t proc_id_end;
  uint64_t cycles;
  char func_name[256];
} function_t;

function_t *__func = NULL;

/**
 * Store global function information
 */
typedef struct global_function_s
{
  uint64_t ncalls;
  uint64_t nthreads;
  uint64_t cycles;
  uint64_t list_threads[256];
  char func_name[256];
} global_function_t;

global_function_t *__glob_func = NULL;

/**
 * Store module information
 */
typedef struct module_s
{
  uint64_t nprocs;
  uint64_t nprocs_avail;
  uint64_t nthreads;
  uint64_t nfuncs;
  double core_switch_ratio;
  uint64_t list_threads[256];
} module_t;

module_t __mod;

/**
 * Library constructor - called ahen the library is loaded
 */
void libinsertrdtsc_initialize() __attribute__((constructor));

/**
 * Library destructor - called when the library is unloaded
 */
void libinsertrdtsc_finalize() __attribute__((destructor));

/**
 * insert_function - insert information about function_t
 * @param index    : index to insert
 * @param pid      : process id
 * @param tid      : thread id
 * @param proc_id  : processor id
 * @param cycles   : cycles
 * @param func_name: function name
 * @return
 */
void insert_function(uint64_t index, uint64_t pid, uint64_t tid,
                     uint64_t proc_id_start, uint64_t proc_id_end,
                     uint64_t cycles, char *func_name);

/**
 * analyze_function - Analyze function_t structure
 * @param n: number of function calls
 * @return
 */
void analyze_function(uint64_t n);

/**
 * write_function_info - Write function info in stdout
 * @param n: number of function calls
 * @return
 */
void write_function_info(uint64_t n);

/**
 * write_function_info_in_csv_file - Write function info in CSV file
 * @param n: number of function calls
 * @return
 */
void write_function_info_in_csv_file(uint64_t n);

/**
 * write_function_summary - Write function summary in stdout
 * @return
 */
void write_function_summary();

/**
 * write_module_summary - Write module summary in stdout
 * @return
 */
void write_module_summary(void);

/**
 * rdtsc - ReaD Time-Stamp Counter
 * @return the time-stamp counter
 */
uint64_t rdtsc(void);

/**
 * rdtscp - ReaD Time-Stamp Counter and Processor id
 * @param cycles : get time-stamp counter
 * @param proc_id: get processor id
 * @return
 */
void rdtscp(uint64_t *cycles, uint64_t *proc_id);

/**
 * get_pid - Get Process ID
 * @return the process id
 */
uint64_t get_pid(void);

/**
 * get_tid - Get Thread ID
 * @return the thread id
 */
uint64_t get_tid(void);

/**
 * get_num_procs - Get Number of Processors
 * @return the number of processors
 */
uint64_t get_num_procs(void);

/**
 * get_num_procs_available - Get Number of Processors Available
 * @return the number of processors available
 */
uint64_t get_num_procs_available(void);

#endif // _RUNTIME_H_
