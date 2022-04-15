#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

void generate_random(double *input, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    input[i] = rand() / (double)(RAND_MAX);
  }
}

typedef struct { double l_sum; char pad[128]; } p_double; 

double opt_local_sum(double *x, size_t size, int n_threads) {
    p_double local_sum[n_threads];
    #pragma omp parallel shared(local_sum)
    {
        int id = omp_get_thread_num();
        local_sum[id].l_sum = 0.0;
        #pragma omp for
            for (size_t i = 0; i<size; i++) {
                local_sum[id].l_sum += x[i];
            }
    }
    double sum_val = 0.0;
    for (int i = 0; i < n_threads; i++) {
       sum_val += local_sum[i].l_sum;
    }
    return sum_val;
}

int main(int argc, char *argv[]){
    double *array;
    double start_time, avg_time, result;
    int n_tests = 10;
    int n_threads = 10;
    int threads[] = {1, 2, 4, 8, 12, 16, 20, 24, 28, 32};
    int size = 1E+7;
    double time[n_tests];
    array = (double *) malloc(size*sizeof(double));

    generate_random(array, size);
        for (int thread=0; thread<n_threads; thread++) {
            omp_set_num_threads(threads[thread]);

            start_time = omp_get_wtime();
            for (int i=0; i<n_tests; i++) {
                result = opt_local_sum(array, size, threads[thread]);
                time[i] = omp_get_wtime() - start_time;
            }
            avg_time = (omp_get_wtime() - start_time) / n_tests;
            double sd_sum = 0;
            for (int i=0; i<n_tests-1; i++) {
                sd_sum += pow((time[i+1] - time[i]) - avg_time, 2);
            }
            double std_dev = sqrt((sd_sum/(n_tests - 1)));
            printf("Number of threads this run it %d \n",threads[thread]);
            printf(" Execution time is %fs with a std deviation of %.15f \n", avg_time, std_dev);
            printf("Serial = %f, OMP = %f \n", serial_sum(array, size), result);
        }
    free(array);
}

