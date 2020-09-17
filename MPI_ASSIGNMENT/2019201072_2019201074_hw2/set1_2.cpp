#include <mpi.h>
#include <stdio.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int main(int argc, char* argv[]) {
    // All the global variables used are kept here
    int noOfProcess, myRank, ROOT_RANK = 0, tasksPerProcess; double result = 0.0;

    MPI_Init(&argc, &argv); // Initialization of MPI
    if(MPI_Comm_size(MPI_COMM_WORLD, &noOfProcess) != MPI_SUCCESS) { // Calculating the number of processes to be used
        printf("Some error occurred: MPI_Comm_size!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if(noOfProcess < 2) {
        printf("Please use atleast two processes!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if(MPI_Comm_rank(MPI_COMM_WORLD, &myRank) != MPI_SUCCESS) {
        printf("Some error occurred: MPI_Comm_rank!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    int n; double finalResult; // Local variables of ROOT
    if(myRank == ROOT_RANK) {
        // printf("Enter the value of N: "); fflush(stdout);
        scanf("%d", &n);
        if(n < 1) {
            printf("Value of N should be atleast 1"); fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        tasksPerProcess = n/(noOfProcess-1);
    }

    if(MPI_Bcast(&tasksPerProcess, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
        printf("Some error occurred: MPI_Bcast!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if(myRank == ROOT_RANK) {
        for(int i = tasksPerProcess*(noOfProcess-1)+1; i<=n; i++) {
            result += 1.0/(i*i);
        }
    } else {
        for(int i = tasksPerProcess*(myRank-1)+1, j=0; j<tasksPerProcess; j++, i++) {
            result += 1.0/(i*i);
        }
    }

    if(MPI_Reduce(&result, &finalResult, 1, MPI_DOUBLE, MPI_SUM, ROOT_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
        printf("Some error occurred: MPI_Reduce!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if(myRank == ROOT_RANK) {
        printf("%.6lf\n", finalResult);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}