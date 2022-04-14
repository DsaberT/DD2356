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

double omp_local_sum(double *x, size_t size, int n_threads) {
    double local_sum[n_threads];
    #pragma omp parallel shared(local_sum)
    {
        int id = omp_get_thread_num();
        #pragma omp for
            for (size_t i = 0; i<size; i++) {
                local_sum[id] += x[i];
            }
    }
    double sum_val = 0.0;
    for (int i = 0; i < n_threads; i++) {
       sum_val += local_sum[i];
    }
    return sum_val;
}

double serial_sum(double *x, size_t size)
{
    double sum_val = 0.0;
    for (size_t i = 0; i < size; i++) {
        sum_val += x[i];
    }
    return sum_val;
}

int main(int argc, char *argv[]){
    double *array;
    double start_time, avg_time, result;
    int n_tests = 10;
    int n_threads = 10;							                //n+1 num of elements
    int threads[] = {1, 2, 4, 8, 12, 16, 20, 24, 28, 32};		//list of number of threads
    int size = 1E+7;
    double time[n_tests];
    array = (double *) malloc(size*sizeof(double));
    generate_random(array, size);
    for (int a=0; a<n_threads; a++) {
        omp_set_num_threads(threads[a]);
        result = omp_local_sum(array, size, threads[a]); //Avoiding cold start
        start_time = omp_get_wtime();
        for (int i=0; i<n_tests; i++) {
            result = omp_local_sum(array, size, threads[a]);
            time[i] = omp_get_wtime() - start_time;
        }
        avg_time = (omp_get_wtime() - start_time) / n_tests;
        double sd_sum = 0;
        for (int i=0; i<n_tests-1; i++) {
            sd_sum += pow((time[i+1] - time[i]) - avg_time, 2);
        }
        double std_dev = sqrt((sd_sum/(n_tests - 1)));
        printf("Number of threads this run it %d \n",threads[a]);
        printf(" Execution time of parallel sum was %fs with a std deviation of %.15f \n", avg_time, std_dev);
        printf("Control: Serial sum = %f, OMP sum = %f \n", serial_sum(array, size), result);
    }
    free(array);
}
