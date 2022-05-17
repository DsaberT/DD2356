
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define SEED     921
#define Runs 16777216

int main(int argc, char* argv[])
{
    int count = 0;
    double x, y, z, pi;
    int rank, size, i, provided, iterations;


    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(SEED*rank); // Important: Multiply SEED by "rank" when you introduce MPI!

   // int Looplength = log2(size);
    iterations = Runs/size;

    double Start_T, Stop_T;

	Start_T = MPI_Wtime();

    // Calculate PI following a Monte Carlo method
    for (int iter = 0; iter < iterations; iter++)
    {
        // Generate random (X,Y) points
        x = (double)random() / (double)RAND_MAX;
        y = (double)random() / (double)RAND_MAX;
        z = sqrt((x*x) + (y*y));

        // Check if point is in unit circle
        if (z <= 1.0)
        {
            count++;
        }
    }

    //Binary Tree Implemention
    int CountLocal = 0;
    int end = 1;
    int Looplength = log2(size);

    //For the first loop to makes sure all odd ranks send their result to the rank before
    if (rank % 2 == 1){
        MPI_Send(&count, 1, MPI_INT,(rank - 1), 0, MPI_COMM_WORLD);
        end = 0;
    }
    for (int i = 0; i < Looplength; i++){
        if (((rank % (1 << (i+1))) == 0) && end) {
            MPI_Recv(&CountLocal, 1, MPI_INT, (rank + (int)(pow(2,i))), 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
              count += CountLocal;
        }
        else if (end){
            MPI_Send(&count, 1, MPI_INT,(rank - (int)(pow(2,i))), 0, MPI_COMM_WORLD);
            end = 0;
        }
    }

    if (rank == 0) {
        // Estimate Pi and display the result
         pi = ((double)count / (double)(iterations*size)) * 4.0;
         //Stop_T= MPI_Wtime();
         //printf("pi = %f Execution Time = %f\n", pi , Stop_T-Start_T);
   }

    Stop_T= MPI_Wtime();
    if (rank == 0){
        printf("pi = %f Execution Time = %f\n", pi , Stop_T-Start_T);
    }
    MPI_Finalize();
    return 0;
}
