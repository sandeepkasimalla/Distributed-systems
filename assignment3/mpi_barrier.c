#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void mpi_iittp_barrier(){
	int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if(world_rank == 0){
		if(2*world_rank+1 < world_size || 2*world_rank+2 < world_size){
			int rec_num;
			MPI_Recv(&rec_num, 1, MPI_INT, 2*world_rank+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(2*world_rank+2 < world_size){
				int rec_num1;
				MPI_Recv(&rec_num1, 1, MPI_INT, 2*world_rank+2, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			}
			int num = 1;
			MPI_Send(&num, 1, MPI_INT, 2*world_rank+1, 1, MPI_COMM_WORLD);
			if(2*world_rank+2 < world_size){
				int num1 = 1;
				MPI_Send(&num1, 1, MPI_INT, 2*world_rank+2, 1, MPI_COMM_WORLD);
			}
		}
	}
	else if(2*world_rank+1 < world_size || 2*world_rank+2 < world_size){
		int rec_num;
		MPI_Recv(&rec_num, 1, MPI_INT, 2*world_rank+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if(2*world_rank+2 < world_size){
			int rec_num1;
			MPI_Recv(&rec_num1, 1, MPI_INT, 2*world_rank+2, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		}
		int send_id;
		if(world_rank%2 == 0)
			 send_id = (world_rank-2)/2;
		else
			 send_id = (world_rank-1)/2;
		int num2 = 1;
		MPI_Send(&num2, 1, MPI_INT, send_id, 1, MPI_COMM_WORLD);
		int rec_num2;
		MPI_Recv(&rec_num2, 1, MPI_INT, send_id, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int num = 1;
		MPI_Send(&num, 1, MPI_INT, 2*world_rank+1, 1, MPI_COMM_WORLD);
		if(2*world_rank+2 < world_size){
			int num1 = 1;
			MPI_Send(&num1, 1, MPI_INT, 2*world_rank+2, 1, MPI_COMM_WORLD);
		}
	}
	else{
		int send_id;
		if(world_rank == 0)	
			 send_id = (world_rank-2)/2;
		else
			 send_id = (world_rank-1)/2;
		int num = 1;
		MPI_Send(&num, 1, MPI_INT, send_id, 1, MPI_COMM_WORLD);
		int rec_num;
		MPI_Recv(&rec_num, 1, MPI_INT, send_id, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	}
}

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	printf("before barrier of process id %d \n",world_rank);
	mpi_iittp_barrier();
	printf("after barrier of process id %d \n",world_rank);
	
    // Finalize the MPI environment.
    MPI_Finalize();
}

