#include <mpi.h>
#include <stdio.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void countCycles(int startVertex, int endVertex, bool* visited, bool* graph, int pathLength, int &count4, int &count3, int noOfVertices) {
	visited[startVertex] = true;

	// Termination Condition
	if(pathLength == 0) {
		visited[startVertex] = false;
		if(*(graph + startVertex*noOfVertices + endVertex)) count4++;
		return;
	} else if(pathLength == 1) {
		if(*(graph + startVertex*noOfVertices + endVertex)) count3++;
	}

	// Perform DFS
	for(int i = 0; i < noOfVertices; i++){
		if(!visited[i] && *(graph + startVertex*noOfVertices + i)){
			countCycles(i, endVertex, visited, graph, pathLength-1, count4, count3, noOfVertices);
		}
	}
	visited[startVertex] = false;
}
int main(int argc,char* argv[]){
	// All the global variables used are kept here
	int myRank, noOfProcess, noOfVertices, noOfEdges, len1 = 3, len2 = 4, ROOT_RANK = 0, count3 = 0, count4 = 0;

	// Initialization of MPI
	MPI_Init(&argc, &argv);
    if(MPI_Comm_size(MPI_COMM_WORLD, &noOfProcess) != MPI_SUCCESS) {
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

	// Input the graph and broadcast this graph to other nodes
    if(myRank == ROOT_RANK) {
    	scanf("%d %d", &noOfVertices, &noOfEdges);
	}
	if(MPI_Bcast(&noOfVertices, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
		printf("Some error occurred: MPI_Bcast(noOfVertices)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}
	bool graph[noOfVertices][noOfVertices];
	for(int i = 0; i < noOfVertices; i++) {
		for(int j = 0; j < noOfVertices; j++) {
			graph[i][j] = false;
		}
	}
	if(myRank == ROOT_RANK) {
		int u, v;
    	for(int i = 0; i < noOfEdges; i++){
    		scanf("%d %d", &u, &v);
    		graph[u-1][v-1] = true; graph[v-1][u-1] = true;
    	}
    }
    if(MPI_Bcast(graph, noOfVertices*noOfVertices, MPI_INT, ROOT_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
		printf("Some error occurred: MPI_Bcast(graph)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}

    if(myRank == ROOT_RANK) {
		// Distribute the task
		int taskAssignedto = 0;
		for(int i = 0; i < noOfVertices; i++) {
			taskAssignedto++; taskAssignedto %= noOfProcess; if(taskAssignedto == ROOT_RANK) taskAssignedto++;
			if(MPI_Send(&i, 1, MPI_INT, taskAssignedto, 0, MPI_COMM_WORLD) != MPI_SUCCESS) {
				printf("Some error occurred: MPI_Send(taskAssignedto)!!"); fflush(stdout);
				MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			}
		}

		// End the task
		int endTaskId = -1;
		for(int i = 0; i < noOfProcess; i++) {
			if(MPI_Send(&endTaskId, 1, MPI_INT, i, 0, MPI_COMM_WORLD) != MPI_SUCCESS) {
				printf("Some error occurred: MPI_Send(End the task)!!"); fflush(stdout);
				MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			}
		}
	}
	else {
		// local variables of RECEIVER
		int nodeNoAssigned; bool visited[noOfVertices];
		for(int i = 0; i < noOfVertices; i++){
			visited[i] = false;
		}

		// accept the task indeterminately
		while(1) {
			if(MPI_Recv(&nodeNoAssigned, 1, MPI_INT, ROOT_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS) {
				printf("Some error occurred: MPI_Recv(nodeNoAssigned)!!"); fflush(stdout);
				MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
			}
			if(nodeNoAssigned == -1) break;

			countCycles(nodeNoAssigned, nodeNoAssigned, visited, (bool*)graph, len2-1, count4, count3, noOfVertices);
		}
	}

	count3 /= 2; count4 /= 2; // As clockwise and anti-clockwise is counted twice

	// Calculate the final result
	int finalResult3, finalResult4;
	if(MPI_Reduce(&count3, &finalResult3, 1, MPI_INT, MPI_SUM, ROOT_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
		printf("Some error occurred: MPI_Reduce(count3)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}
	if(MPI_Reduce(&count4, &finalResult4, 1, MPI_INT, MPI_SUM, ROOT_RANK, MPI_COMM_WORLD) != MPI_SUCCESS) {
		printf("Some error occurred: MPI_Reduce(count4)!!"); fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}
	if(myRank == ROOT_RANK) {
		printf("%d %d\n", finalResult3/len1, finalResult4/len2); // As at every node, we are performing countCycles
	}

	MPI_Finalize();
	return EXIT_SUCCESS;
}