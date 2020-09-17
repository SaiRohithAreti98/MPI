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
	int len1=3;
	int len2=4;
	int edges;
	int findcycleswith;
	int graph[500][500];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);   
    MPI_Comm_size(MPI_COMM_WORLD, &total_number_processes);
    if(rank==0){
    	printf("Enter the number of nodes in graph:");
    	fflush(stdout);
    	scanf("%d",&nodes);
    	printf("\nEnter the number of edges\n");
    	fflush(stdout);
    	scanf("%d",&edges);
    	for(int i=0;i<edges;i++){
    		int u;
    		int v;
    		scanf("%d",&u);
    		scanf("%d",&v);
    		graph[u][v]=1;
    		graph[v][u]=1;
    	}
    }
    MPI_Bcast(&nodes,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&graph[0][0],500*500,MPI_INT,0,MPI_COMM_WORLD);

    //assumption number of ranks is equal to number of nodes + 1
    if(rank==0){
    	//for len3
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
		//arguments start end visited graph size counter
		int count_3=0;
		countcycles(findcycleswith,findcycleswith,visited,graph,len1-1,count_3,nodes);
		count_3=count_3/2;
		MPI_Send(&count_3,1,MPI_INT,0,0,MPI_COMM_WORLD);
		// printf("\n findcycleswith %d --->rank %d --> cycles %d \n",findcycleswith,rank,count_3);
		// fflush(stdout);
		for(int i=0;i<nodes;i++){
			visited[i]=false;
		}
		int count_4=0;
		countcycles(findcycleswith,findcycleswith,visited,graph,len2-1,count_4,nodes);
		count_4=count_4/2;
		MPI_Send(&count_4,1,MPI_INT,0,0,MPI_COMM_WORLD);
	}
	if(rank==0){
		//getting cycles of length 3
		int total_count=0;
		int count_3=0;
		for(int i=1;i<total_number_processes;i++){
			MPI_Recv(&count_3,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			total_count=total_count+count_3;
			count_3=0;
		}
		int finalcount_3=total_count/len1;

		//getting total cycles of length 4
		total_count=0;
		int count_4=0;
		for(int i=1;i<total_number_processes;i++){
			MPI_Recv(&count_4,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			total_count=total_count+count_4;
			count_4=0;
		}
		int finalcount_4=total_count/len2;

		printf("\nThe total number of cycles of length %d in the graph are:%d\n",len1,finalcount_3);
		fflush(stdout);

		printf("\nThe total number of cycles of length %d in the graph are:%d\n",len2,finalcount_4);
		fflush(stdout);
	}
	MPI_Finalize();
	return 0;
}