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

double omp_sum(double *x, size_t size)
{
    double sum_val = 0.0;
    #pragma omp parallel for
        for (size_t i = 0; i < size; i++) {
            sum_val += x[i];
        }
    return sum_val;
}
//saves old function to compare if they get the same result
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
    int size = 1E+7;
    double time[n_tests];
    array = (double *) malloc(size*sizeof(double));

    omp_set_num_threads(32);					//set threads to 32
    generate_random(array, size);
    result = omp_sum(array, size);
    start_time = omp_get_wtime();
    for (int i=0; i<n_tests; i++) {
        result = omp_sum(array, size);
        time[i] = omp_get_wtime() - start_time;
    }
    avg_time = (omp_get_wtime() - start_time) / n_tests;
    double sd_sum = 0;
    for (int i=0; i<n_tests-1; i++) {
        sd_sum += pow((time[i+1] - time[i]) - avg_time, 2);
    }
    double std_dev = sqrt((sd_sum/(n_tests - 1)));
    printf(" Execution time of parallel sum was %fs with a std deviation of %.15f \n", avg_time, std_dev);
    printf("Serial sum = %f, parallel sum = %f \n", serial_sum(array, size), result);
    free(array);
}
