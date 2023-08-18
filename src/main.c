#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>

#include <time.h>

#include "mhultistone.h"
#include "xutThreadPool.h"
#include "xut.h"

//#include proc_1benchmark.h"

long int n_threads = 0;
long int n_stones = 0;

#define N_ARG_MAP   256
#define N_ARG_FLAGS 256

char* arg_map[N_ARG_MAP];
int arg_flags[N_ARG_FLAGS];

void process_args(int argc, char* argv[]) {
    int len_a1, len_a2;
    int n_extract, t_extract;
    char* endptr;

    if (argc <= 1) {
        // run default settings
        // TODO: calculate reasonable value for default n_stones
        printf("Run default settings\n");
        n_threads = 1;
        n_stones = 100000;
        return;
    }

    // extract n_stones from first argument
    len_a1 = strlen(argv[1]);
    n_extract = strtol(argv[1], &endptr, 10);
    if ((endptr - len_a1) != argv[1]) {
        printf("ERROR: oddly formatted input argument 1 - expected int (got '%s')\n", argv[1]);
        // TODO: print usage string
        return;
    }
    if (n_extract < 1) {
        printf("ERROR: invalid n_stone parameter - expected value > 1 (got '%i')\n", n_extract);
        // TODO: print usage string
        return;
    }

    if (argc >= 2) {
        len_a2 = strlen(argv[2]);
        t_extract = strtol(argv[2], &endptr, 10);
        if ((endptr - len_a2) != argv[2]) {
            printf("ERROR: oddly formatted input argument 2 - expected int (got '%s')\n", argv[2]);
            return;
        }
        if (t_extract <= 0) {
            printf("ERROR: invalid n_thread parameter - expected value >= 1 (got '%i')\n", t_extract);
            return;
        }
    }

    n_stones = n_extract;
    n_threads = t_extract;
}

void clear_arg_db() {
    for (int i = 0; i < N_ARG_MAP; i++) {
        arg_map[i] = "";
    }

    for (int i = 0; i < N_ARG_FLAGS; i++) {
        arg_flags[i] = 0;
    }
}

void process_args_to_db(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        char* str = argv[i];
        int len = strlen(str);
        if (len <= 0) {
            // I don't know if the OS will pass empty strings as arguments, so handle here
            continue;
        }

        if (str[0] == '-') {
            if (len >= 2 && str[1] == '-') {
                // TODO: long flag parsing
                // ignore for now
                if (strcmp(str, "--help")) {
                    // special case which will be replaced by generic handling in future
                    // TODO: remove
                    arg_flags['h'] = 1;
                }
            }
            else if (len >= 3 && str[2] == '=') {
                // mapped variable
                char key = str[1];
                arg_map[key] = str + 3;
            }
            else {
                // simple flags
                for (int j = 1; j < len; j++) {
                    // -abc
                    // sets flags a, b, c
                    arg_flags[str[j]] = 1;
                }
            }
        }
        else {
            // ignore unrecognized argument
        }
    }
}


int count_arg_flags() {
    int n = 0;
    for (int i = 0; i < N_ARG_FLAGS; i++) {
        n += (arg_flags[i] != 0);
    }
    return n;
}
int count_arg_map() {
    int n = 0;
    for (int i = 0; i < N_ARG_MAP; i++) {
        n += (strlen(arg_map[i]) != 0);
    }
    return n;
}

void print_usage() {
    printf("Usage: mp-dhrystone [options]\n");
    printf("Options:\n");
    printf("  --help              Display this information\n");
    printf("  -h\n");
    printf("\n");
    printf("  -n=<n_dhrystones>   Run specified number of dhrystones\n");
    printf("  -t=<n_threads>      Run benchmark with specified number of threads\n");
    printf("  -v                  verbose mode\n");
    printf("  -V                  Print benchmark version number (also --version)\n");
}


void print_db() {
    int n_flags = count_arg_flags();
    int n_map = count_arg_map();

    printf("==== Begin print_db() ====\n");
    if (n_map) {
        printf("  argument_map count=%i\n", n_map);
        for (int i = 0; i < N_ARG_MAP; i++) {
            if (strlen(arg_map[i]) > 0 && isprint(i)) {
                printf("    '%c' : \"%s\"\n", i, arg_map[i]);
            }
        }
    }


    if (n_flags) {
        printf("  flags -> ");
        for (int i = 0; i < N_ARG_FLAGS; i++) {
            if (arg_flags[i] && isprint(i)) {
                printf("%c", i);
            }
        }
        printf("\n");
    }

    printf("==== End print_db() ====\n");
}


void build_config_from_db() {

}

void report_config() {
    printf("    n_threads = %li\n", n_threads);
    printf("    n_stones  = %li\n", n_stones);
}

double timespec_delta(const struct timespec* start, const struct timespec* end) {
    //double t0 = tstart.tv_sec + tstart.tv_nsec / 1000000000.0;
    //double t1 = tend.tv_sec + tend.tv_nsec / 1000000000.0;

    //double dt = t1 - t0;
    //return dt;
    return (end->tv_sec - start->tv_sec)
        + (end->tv_nsec - start->tv_nsec) / 1000000000.0;
}


typedef struct mhultiwrap_data{
    int id;
    int keepRunning;
    int64_t n_runs;
    int64_t count;
} mhultiwrap_data;

/*
void mhultistone_wrapper(void *data){
    if(data == 0){
        return;
    }

    mhultiwrap_data *d = (mhultiwrap_data*)(data);

    //run_mhultistone_instance(d->id, d->n_runs);
    dispatch_idle_mhultistone_instance(d->n_runs);

    return;
}
*/

void wrapper_0(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX0(d->n_runs, &d->keepRunning, &d->count);
    //printf("count=%" PRId64 "\n", d->count);
}
void wrapper_1(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX1(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_2(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX2(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_3(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX3(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_4(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX4(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_5(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX5(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_6(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX6(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_7(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX7(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_8(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX8(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_9(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX9(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_10(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX10(d->n_runs, &d->keepRunning, &d->count);
}
void wrapper_11(void* arg){
    mhultiwrap_data *d = (mhultiwrap_data*)arg;
    d->keepRunning = 1;
    d->count = 0;

    mhultistone_instance_CTX11(d->n_runs, &d->keepRunning, &d->count);
}

void shouldntrun(void* arg){
    printf("I SHOULDNT RUN\n");
}


void (*get_wrapper_instance(int id))(void*){
    switch(id){
        case 0:
            return wrapper_0;
        case 1:
            return wrapper_1;
        case 2:
            return wrapper_2;
        case 3:
            return wrapper_3;
        case 4:
            return wrapper_4;
        case 5:
            return wrapper_5;
        case 6:
            return wrapper_6;
        case 7:
            return wrapper_7;
        case 8:
            return wrapper_8;
        case 9:
            return wrapper_9;
        case 10:
            return wrapper_10;
        case 11:
            return wrapper_11;

        default:
            return NULL;
    }
}


/*
void poolbenchmark(int n_jobs, int64_t stones_per_job){

    int64_t mult = 1;
    n_jobs *= mult;
    stones_per_job /= mult;

    int n_threads = MHULTI_N_CORES;
    struct timespec ts_start;
    struct timespec ts_end;

    printf("xutThreadPooling example\n");

    int64_t nstones_tot = n_jobs * stones_per_job;

    // initialize job inputs
    mhultiwrap_data *job_data = malloc(sizeof(mhultiwrap_data) * n_jobs);
    for(int i = 0; i < n_jobs; i++){
        job_data[i].id = i;
        job_data[i].n_runs = stones_per_job;
    }

    //threadpool pool = thpool_init(n_threads);
    xutThreadPool *pool = xutThreadPool_init(n_threads);

    
    
    xutThreadPool_configWorker(pool, 0,  wrapper_0);
    xutThreadPool_configWorker(pool, 1,  wrapper_1);
    xutThreadPool_configWorker(pool, 2,  wrapper_2);
    xutThreadPool_configWorker(pool, 3,  wrapper_3);
    xutThreadPool_configWorker(pool, 4,  wrapper_4);
    xutThreadPool_configWorker(pool, 5,  wrapper_5);
    xutThreadPool_configWorker(pool, 6,  wrapper_6);
    xutThreadPool_configWorker(pool, 7,  wrapper_7);
    xutThreadPool_configWorker(pool, 8,  wrapper_8);
    xutThreadPool_configWorker(pool, 9,  wrapper_9);
    xutThreadPool_configWorker(pool, 10, wrapper_10);
    xutThreadPool_configWorker(pool, 11, wrapper_11);
    
    

    
    for(int i = 0; i < n_jobs; i++){
        //thpool_add_work(pool, mhultistone_wrapper, job_data + i);
        xutThreadPool_queueJob(pool, shouldntrun, job_data + i);
    }
    //thpool_wait(pool);
    timespec_get(&ts_start, TIME_UTC);
    xutThreadPool_launch(pool);
    xutThreadPool_waitAll(pool);
    timespec_get(&ts_end, TIME_UTC);

    // compute performance & report
    double dt = timespec_delta(&ts_start, &ts_end);
    double dhry_per_second = ((double)nstones_tot) / dt;
    double dmips = dhry_per_second / 1757.0;
    double dmips_per_thread = dmips / (double)n_threads;

    printf("    stones/job   = %" PRId64 "s\n", stones_per_job);
    printf("    njobs        = %i\n",           n_jobs);
    printf("    nthreads     = %i\n",           n_threads);
    printf("    nstones_tot  = %" PRId64 "\n",  nstones_tot);

    printf("    dt           = %fs\n", dt);
    printf("    Dhrystones/s = %f\n", dhry_per_second);
    printf("    DMIPS        = %f\n", dmips);
    printf("    DMIPS/thread = %f\n", dmips_per_thread);
    printf("\n");


    //thpool_destroy(pool);
    //xutThreadPool_destroy(pool);
    free(job_data);


}
*/

void poolbenchmark(int n_jobs, int64_t stones_per_job, int dt_target_ms){

    int64_t mult = 20;
    //n_jobs *= mult;
    //stones_per_job /= mult;

    int n_threads = MHULTI_N_CORES;
    struct timespec ts_start;
    struct timespec ts_end;

    printf("xutThreadPooling example\n");

    int64_t nstones_tot = n_jobs * stones_per_job;

    xutThreadPool *pool = xutThreadPool_init(n_threads);

    // initialize job inputs
    mhultiwrap_data *job_data = malloc(sizeof(mhultiwrap_data) * n_jobs);
    for(int i = 0; i < n_jobs; i++){
        job_data[i].id = i;
        job_data[i].n_runs = stones_per_job;
        job_data[i].count = 0;

        xutThreadPool_queueJob(pool, get_wrapper_instance(i),  job_data + i);
    }

    timespec_get(&ts_start, TIME_UTC);
    xutThreadPool_launch(pool);
    xutSleep_ms(dt_target_ms);
    for(int i = 0; i < n_jobs; i++){
        job_data[i].keepRunning = 0;
    }
    xutThreadPool_waitAll(pool);
    timespec_get(&ts_end, TIME_UTC);

    int64_t n_stones_run = 0;
    for(int i = 0; i < n_jobs; i++){
        n_stones_run += job_data[i].count;
    }

    // compute performance & report
    double dt = timespec_delta(&ts_start, &ts_end);
    double dhry_per_second = ((double)n_stones_run) / dt;
    double dmips = dhry_per_second / 1757.0;
    double dmips_per_job = dmips / (double)n_jobs;

    printf("    njobs        = %i\n",           n_jobs);
    printf("    nthreads     = %i\n",           n_threads);
    printf("    nstones_tot  = %" PRId64 "\n",  n_stones_run);

    printf("    dt           = %fs\n", dt);
    printf("    Dhrystones/s = %f\n", dhry_per_second);
    printf("    DMIPS        = %f\n", dmips);
    printf("    DMIPS/job    = %f\n",    dmips_per_job);
    printf("\n");


    //thpool_destroy(pool);
    //xutThreadPool_destroy(pool);
    free(job_data);


}

int arg_map_extract_long_int(char key, long int *ret);
long int arg_map_extract_long_int_default(char key, long int default_val);

int arg_map_extract_double(char key, double *ret);
double arg_map_extract_double_default(char key, double default_val);

int arg_map_extract_long_int(char key, long int *ret){
    char* endptr;
    long int retval = strtol(arg_map[key], &endptr, 10);

    if(endptr == arg_map[key]){
        return 0;
    }

    *ret = retval;
    return 1;
}

long int arg_map_extract_long_int_default(char key, long int default_val){
    long int r;
    if(!arg_map_extract_long_int(key, &r)){
        return default_val;
    }
    return r;
}

double arg_map_extract_double_default(char key, double default_val){
    double r;
    if(!arg_map_extract_double(key, &r)){
        return default_val;
    }
    return r;
}

int arg_map_extract_double(char key, double *ret){
    char* endptr;
    double retval = strtod(arg_map[key], &endptr);

    if(endptr == arg_map[key]){
        return 0;
    }

    *ret = retval;
    return 1;
}

int main(int argc, char* argv[]) {
    clear_arg_db();
    process_args_to_db(argc, argv);

    //print_db();
    if (arg_flags['h']) {
        print_usage();
        return 0;
    }

    char* endptr;
    long int nstones = arg_map_extract_long_int_default('n', 10000);
    long int n_jobs = arg_map_extract_long_int_default('j', MHULTI_N_CORES);
    double dt_target = arg_map_extract_double_default('t', 1.0);
    long int dt_target_ms = (long int)(dt_target * 1000.0);

    printf("dt_taget=%f\n", dt_target);

    poolbenchmark(n_jobs, nstones, dt_target_ms);

    return 0;
}

