#include "omp.h"
#include "stdio.h"
#include <stdlib.h>
#define MAXTHREADS 32

/* The code for task 3 of the lab */

void generate_random(double *input, size_t size)
{
        for (size_t i = 0; i < size; i++) {
                input[i] = rand() / (double)(RAND_MAX);
        }
}

double serial_sum(double *x, size_t size)
{
        double sum_val = 0.0;

        for (size_t i = 0; i < size; i++) {
                sum_val += x[i];
        }

        return sum_val;
}

double omp_sum(double *x, size_t size)
{
        double sum_val = 0.0;
        #pragma omp parallel for
        for (size_t i = 0; i < size; i++) {
                sum_val += x[i];
        }

        return sum_val;
}

double omp_critical_sum(double *x, size_t size)
{
        double sum_val = 0.0;
        #pragma omp parallel for
        for (size_t i = 0; i < size; i++) {
                #pragma omp critical
                {
                        sum_val += x[i];
                }
        }

        return sum_val;
}

double omp_local_sum(double *x, size_t size)
{
        double sum_val = 0.0;
        double thread_sum[MAXTHREADS];
        #pragma omp parallel shared(thread_sum)
        {
                int id = omp_get_thread_num();
                thread_sum[id] = 0.0;
                #pragma omp for
                for (size_t i = 0; i < size; i++) {
                        thread_sum[id] += x[i];
                }
        }

        for(size_t i = 0; i < MAXTHREADS; i++){
                sum_val += thread_sum[i];
        }
        return sum_val;
}

double opt_local_sum(double *x, size_t size)
{
        typedef struct { double sum; char pad[56]; } padded_sum;
        double sum_val = 0.0;
        padded_sum thread_sum[MAXTHREADS];
        #pragma omp parallel shared(thread_sum)
        {
                int id = omp_get_thread_num();
                thread_sum[id].sum = 0.0;
                #pragma omp for
                for (size_t i = 0; i < size; i++) {
                        thread_sum[id].sum += x[i];
                }
        }

        for(size_t i = 0; i < MAXTHREADS; i++){
                sum_val += thread_sum[i].sum;
        }
        return sum_val;
}

int main(int argc, char *argv[]){
        omp_set_num_threads(MAXTHREADS);
        size_t array_size = 10000000;
        double start, end, result;
        double *rand_array = (double*)malloc(sizeof(double)*array_size);

        printf("Five runs with %d threads\n", MAXTHREADS);
        for(int i = 0; i < 5; i++){
                generate_random(rand_array, array_size);
                start = omp_get_wtime();
                result = omp_sum(rand_array, array_size); //Replace with function to test
                end = omp_get_wtime();
                printf("%f ", end - start);
        }
        printf("\n");

        free(rand_array);
}
