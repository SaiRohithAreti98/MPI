#include <mpi.h>
#include<stdio.h>
#define EXIT_FAILURE 1

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int row_to_rank[1000];
    double input_equations[1000][1000];
    double constants[1000];
    int i,j;
    int row;
    double coeff[1000];
    double sum=0;
    double Xvalues[1000];
    int n;
    int rank,total_number_processes;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   
    MPI_Comm_size(MPI_COMM_WORLD, &total_number_processes); 

    if(total_number_processes < 2) {
        printf("To run in parallel please enter greater than or equal to 2 processes\n");
        fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if (rank==0)
    {
        scanf("%d",&n);

        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                double p;
                scanf("%lf",&p);
                input_equations[i][j]=p;
            }
        }

        for(int i=0;i<n;i++){
            double k;
            scanf("%lf",&k);
            constants[i]=k;
        }
    
    }
    //Bcasting the values to all other processes
    if(MPI_Bcast (&n,1,MPI_INT,0,MPI_COMM_WORLD)!=MPI_SUCCESS){
        printf("Some error occurred: MPI_Bcast(n)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if( MPI_Bcast (constants,n,MPI_DOUBLE,0,MPI_COMM_WORLD)!= MPI_SUCCESS){
        printf("Some error occurred: MPI_Bcast(constants)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }    
    if(MPI_Bcast (&input_equations[0][0],1000*1000,MPI_DOUBLE,0,MPI_COMM_WORLD)!=MPI_SUCCESS){
        printf("Some error occurred: MPI_Bcast(input_equations)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    //mapping the row to rank respectively
    i=0;
    while(i<=n-1){
        row_to_rank[i]=i%total_number_processes;
        i++;
    }

    for(row=0;row<=n-1;row++)
    {
        if(MPI_Bcast (&constants[row],1,MPI_DOUBLE,row_to_rank[row],MPI_COMM_WORLD)!= MPI_SUCCESS){
            printf("Some error occurred: MPI_Bcast(constants[row])!!"); fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        if(MPI_Bcast (&input_equations[row][row],n-row,MPI_DOUBLE,row_to_rank[row],MPI_COMM_WORLD)!=MPI_SUCCESS){
            printf("Some error occurred: MPI_Bcast(input_equations[row])!!"); fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        //starting from row+1 and fiinding coefficents to make that col below that to zero
        i=row+1;
        while(i<=n-1){
          if(row_to_rank[i]==rank)
                coeff[i]=input_equations[i][row]/input_equations[row][row];
            i++;
        }
        //just subtract the coeff*values of row so you get resultant values
        i=row+1;
        while(i<=n-1)
        { 
            if(row_to_rank[i]==rank)
            {
                for(j=0;j<=n-1;j++)
                    input_equations[i][j]-=coeff[i]*input_equations[row][j];
                constants[i]-=coeff[i]*constants[row];
            }
            i++;
        }
    }

    if(rank==0)
    { 
        //backward substitution
        Xvalues[n-1]=constants[n-1]/input_equations[n-1][n-1];
        i=n-2;
        while(i>=0)
        {
            sum=0;
            for(j=i+1;j<n;j++)
                sum+=input_equations[i][j]*Xvalues[j];
            Xvalues[i]=(constants[i]-sum)/input_equations[i][i];
            i--;
        }
        //printing after back substitution
        i=0;
        while(i<=n-1){
            printf("%f ",Xvalues[i]);
            i++;
        }
        printf("\n");
    }
    MPI_Finalize();
    return 0;
}


// if(MPI_Bcast(graph, noOfVertices*noOfVertices, MPI_INT, ROOT_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
//         printf("Some error occurred: MPI_Bcast(graph)!!"); fflush(stdout);
//         MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
//     }

