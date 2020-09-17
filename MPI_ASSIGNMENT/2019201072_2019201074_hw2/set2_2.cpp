#include <mpi.h>
#include<stdio.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv); // Initialization of MPI
    int row_to_rank[1000];
    double input_equations[100][100];
    double constants[100];
    int i,j;
    int row;
    double coeff[100];
    double sum=0;
    double Xvalues[100];
    int n;
    int rank,total_number_processes, ROOT_RANK = 0;

    if(MPI_Comm_rank(MPI_COMM_WORLD, &rank) != MPI_SUCCESS) {
        printf("Some error occurred: MPI_Comm_rank!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if(MPI_Comm_size(MPI_COMM_WORLD, &total_number_processes) != MPI_SUCCESS) { // Calculating the number of processes to be used
        printf("Some error occurred: MPI_Comm_size!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if(total_number_processes < 2) {
        printf("Please use atleast two processes!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // scan the input 
    if (rank == ROOT_RANK)
    {
        double p;
        scanf("%d",&n);
        for(int i=0; i < n; i++) {
            for(int j=0; j < n; j++) {
                scanf("%lf", &p);
                input_equations[i][j] = p;
            }
        }
        for(int i=0; i < n; i++) {
            scanf("%lf", &p);
            constants[i] = p;
        }
    }
    //Bcasting the values to all other processes
    if(MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD) != MPI_SUCCESS){
        printf("Some error occurred: MPI_Bcast(n)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if(MPI_Bcast(constants, 100, MPI_DOUBLE, 0, MPI_COMM_WORLD) != MPI_SUCCESS){
        printf("Some error occurred: MPI_Bcast(constants)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if(MPI_Bcast (input_equations, 100*100, MPI_DOUBLE, 0, MPI_COMM_WORLD) != MPI_SUCCESS){
        printf("Some error occurred: MPI_Bcast(input_equations)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    //mapping the row to rank respectively
    for(i = 0; i < n; i++)
        row_to_rank[i] = i%total_number_processes;

    for(row = 0; row < n; row++)
    {
        if(MPI_Bcast(&constants[row], 1, MPI_DOUBLE, row_to_rank[row], MPI_COMM_WORLD) != MPI_SUCCESS) {
            printf("Some error occurred: MPI_Bcast(constants[row])!!"); fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        if(MPI_Bcast(&input_equations[row][row], n-row, MPI_DOUBLE, row_to_rank[row], MPI_COMM_WORLD) != MPI_SUCCESS) {
            printf("Some error occurred: MPI_Bcast(input_equations[row])!!"); fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        //starting from row+1 and finding coefficents to make that col below that to zero
        for(i = row+1; i < n; i++) {
            if(row_to_rank[i] == rank) {
                coeff[i] = input_equations[i][row]/input_equations[row][row];
                for(j = 0; j < n; j++)
                    input_equations[i][j] -= coeff[i] * input_equations[row][j];
                constants[i] -= coeff[i]*constants[row];
            }
        }
        // just subtract the coeff*values of row so you get resultant values
        // for(i = row+1; i < n; i++)
        // {
        //     if(row_to_rank[i] == rank)
        //     {
        //         for(j=0;j<=n-1;j++)
        //             input_equations[i][j]-=coeff[i]*input_equations[row][j];
        //         constants[i]-=coeff[i]*constants[row];
        //     }
        // }
    }

    if(rank == ROOT_RANK)
    {
        //backward substitution
        Xvalues[n-1] = constants[n-1]/input_equations[n-1][n-1];
        for(i = n-2; i >= 0; i--)
        {
            sum=0;
            for(j=i+1; j<n; j++)
                sum += input_equations[i][j]*Xvalues[j];
            Xvalues[i] = (constants[i]-sum)/input_equations[i][i];
        }
        //printing after back substitution
        for(i=0; i<n; i++) {
            printf("%.2lf ",Xvalues[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}