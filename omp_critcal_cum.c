#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

//create Array
void generate_random(double *input, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    input[i] = rand() / (double)(RAND_MAX);
  }
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
    int n_threads = 10;							                 //total number of elements
    int threads[] = {1, 2, 4, 8, 12, 16, 20, 24, 28, 32};		//creats list with all sizes
    int size = 1E+7;
    double time[n_tests];
    array = (double *) malloc(size*sizeof(double));
    
    generate_random(array, size);
    for (int a=0; a<n_threads; a++) {
        omp_set_num_threads(threads[a]);
        result = omp_critical_sum(array, size);
        start_time = omp_get_wtime();
        for (int i=0; i<n_tests; i++) {
            result = omp_critical_sum(array, size);
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
