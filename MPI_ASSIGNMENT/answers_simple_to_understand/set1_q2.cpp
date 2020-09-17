#include <mpi.h>
#include <stdio.h>

#define M_PI 3.14159265358979323846
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int main(int argc, char* argv[]) {
    int noOfProcess, myRank;
    int SENDER = 0, RECEIVER = 1;
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &noOfProcess); 
    if(noOfProcess != 2) {
        printf("Please use two processes only!!");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if(myRank == SENDER) {
        double actualResult = M_PI * M_PI / 6.0, calculatedResult = 0, summationVariable = 0, receivedResult;
        double accuracyScore;
        printf("enter accuracy:");
        scanf("%lf", &accuracyScore);

        if(accuracyScore < 0.0 || accuracyScore > 1.0) {
            printf("Invalid accuracy score!!");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }

        while((1 - (actualResult-calculatedResult)/actualResult) < accuracyScore) {
            summationVariable++;
            MPI_Send(&summationVariable, 1, MPI_DOUBLE, RECEIVER, 0, MPI_COMM_WORLD);
            MPI_Recv(&receivedResult, 1, MPI_DOUBLE, RECEIVER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            calculatedResult += receivedResult;
        }
                          
      printf("The calculated sum = %lf and summation variable n = %lf\n", calculatedResult, summationVariable);

          summationVariable = -1.0;
          MPI_Send(&summationVariable, 1, MPI_DOUBLE, RECEIVER, 0, MPI_COMM_WORLD);
      }
      else if(myRank == RECEIVER) {
          double result, summationVariable;

          while(1){
              MPI_Recv(&summationVariable, 1, MPI_DOUBLE, SENDER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
              if(summationVariable == -1.0) break;
              result = 1.0 / (summationVariable * summationVariable);
              MPI_Send(&result, 1, MPI_DOUBLE, SENDER, 0, MPI_COMM_WORLD);
          }
      }
      else {
          printf("This code can never reach here!!");
          MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }

      MPI_Finalize();
      return EXIT_SUCCESS;
  }
