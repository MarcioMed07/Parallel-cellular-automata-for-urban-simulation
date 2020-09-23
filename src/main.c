#include <stdio.h>
#include <omp.h>
#include <string.h>

int main(void){
    int th_id, nthreads, flag = -1;
    #pragma omp parallel private(th_id)
    {
        th_id = omp_get_thread_num();
        printf("Hello from %d\n", th_id);
        #pragma omp barrier
        #pragma omp single
        nthreads = omp_get_num_threads();
        flag = th_id;
        #pragma omp barrier
        #pragma omp single
        printf("Hi, I'm thread %d,\nThread %d discoverd that there are %d threads\n",th_id, flag, nthreads);
    }
    return 0;
}