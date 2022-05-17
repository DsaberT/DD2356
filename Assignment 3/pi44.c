
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define SEED     921
#define Runs 1000000000

int main(int argc, char* argv[])
{
    int count = 0;
    double x, y, z, pi;

    int rank, size, i, provided, iterations;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Makes sure the total number of iterations between is always the same no mather the size.
    iterations = Runs/size;

    srand(SEED*rank); // Important: Multiply SEED by "rank" when you introduce MPI!

    //Timing Variables
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
    //Send results from all other ranks
    if (rank != 0){
        MPI_Reduce(&count, NULL, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    //Recive the results from all other ranks in to rank 0
    else {
         int sumcount;
         MPI_Reduce(&count,&sumcount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
          // Estimate Pi and display the result
         pi = ((double)sumcount / (double)(iterations*size)) * 4.0;
         Stop_T= MPI_Wtime();
         printf("pi = %f Execution Time = %f\n", pi , Stop_T-Start_T);
   }

    MPI_Finalize();
    return 0;
}
