#include<mpi.h>
#include<stdio.h>

void countcycles(int v,int end,bool *visited,int graph[500][500],int n,int &count,int V){
	visited[v]=true;
	if(n==0){
		visited[v]=false;
		if(graph[v][end]==1){
			count++;
			return;
		}
		else{
			return;
		}
	}

	for(int i=0;i<V;i++){
		if(!visited[i] && graph[v][i]==1){
			countcycles(i,end,visited,graph,n-1,count,V);
		}
	}
	visited[v]=false;
}
int main(int argv,char *argc[]){
	MPI_Init(&argv,&argc);
	int rank;
	int total_number_processes;
	int nodes;
	int len;
	int findcycleswith;
	int graph[500][500];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);   
    MPI_Comm_size(MPI_COMM_WORLD, &total_number_processes);
    if(rank==0){
    	printf("Enter the number of nodes in graph:");
    	fflush(stdout);
    	scanf("%d",&nodes);
    	printf("\nEnter the number of nodes as if row i and col j are connected make graph[i][j]=1 graph[j][1]=1 else input 0:\n");
    	fflush(stdout);
    	for(int i=0;i<nodes;i++){
    		for(int j=0;j<nodes;j++){
    			int temp;
    			scanf("%d",&temp);
    			graph[i][j]=temp;
    		}
    	}
    	printf("\nEnter the length of cycle you need to find:");
    	fflush(stdout);
    	scanf("%d",&len);
    }
    MPI_Bcast(&nodes,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&graph[0][0],500*500,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&len,1,MPI_INT,0,MPI_COMM_WORLD);
    //assumption number of ranks is equal to number of nodes + 1
    if(rank==0){
	    for(int i=1;i<total_number_processes;i++){
	    	int temp=i-1;
	    	MPI_Send(&temp,1,MPI_INT,i,0,MPI_COMM_WORLD);
	    }
	}
	if(rank!=0){
		MPI_Recv(&findcycleswith,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		bool visited[nodes];
		for(int i=0;i<nodes;i++){
			visited[i]=false;
		}
		int count=0;
		//arguments start end visited graph size counter
		countcycles(findcycleswith,findcycleswith,visited,graph,len-1,count,nodes);
		count=count/2;
		printf("\n findcycleswith %d --->rank %d --> cycles %d \n",findcycleswith,rank,count);
		fflush(stdout);
		MPI_Send(&count,1,MPI_INT,0,0,MPI_COMM_WORLD);
	}
	if(rank==0){
		int total_count=0;
		int count=0;
		for(int i=1;i<total_number_processes;i++){
			MPI_Recv(&count,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			total_count=total_count+count;
			count=0;
		}

		int r=total_count/len;
		printf("\nThe total number of cycles of length %d in the graph are:%d\n",len,r);
		fflush(stdout);
	}
	MPI_Finalize();
	return 0;
}