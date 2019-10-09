#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int get_num_elements(int np, int len, int Rank){
	int rem = len%np;
	if(rem == 0)
		return len/np;
	else{
		int quo = len/np;
		if(Rank < rem)	
			return quo+1;
		else 
			return quo; 
	}
	
}

void even_phase(int np, int len, int* array, int array_len, int Rank){
	int start_index = 0;
	for(int i=0; i<Rank; i++)
		start_index += get_num_elements(np, len, i);
	int rec_num, rec_num1;
	if(start_index % 2 == 0){
		for(int i=0; i<array_len-1; i++){
			if(array[i]>array[i+1]){
				int x = array[i];
				array[i] = array[i+1];
				array[i+1] = x;
			}
		}
		if(!(array_len%2 == 0) && Rank != np-1){
			MPI_Send(&array[array_len-1], 1, MPI_INT, Rank+1, 2, MPI_COMM_WORLD);
			MPI_Recv(&rec_num1, 1, MPI_INT, Rank+1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(rec_num1 < array[array_len-1])
				array[array_len-1] = rec_num1;
		}
	}
	else{
		if(Rank != 0){
			MPI_Recv(&rec_num, 1, MPI_INT, Rank-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(&array[0], 1, MPI_INT, Rank-1, 2, MPI_COMM_WORLD);
			if(rec_num > array[0])	
				array[0] = rec_num;
		}
		for(int i=1; i<array_len-1; i++){
			if(array[i]>array[i+1]){
				int x = array[i];
				array[i] = array[i+1];
				array[i+1] = x;
			}
		}
		if(array_len%2 == 0 && Rank != np-1){
			MPI_Send(&array[array_len-1], 1, MPI_INT, Rank+1, 2, MPI_COMM_WORLD);
			MPI_Recv(&rec_num1, 1, MPI_INT, Rank+1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(rec_num1 < array[array_len-1])
				array[array_len-1] = rec_num1;
		}


	}
}

void odd_phase(int np, int len, int* array, int array_len, int Rank){
	int start_index = 0;
	for(int i=0; i<Rank; i++)
		start_index += get_num_elements(np, len, i);
	int rec_num, rec_num1;
	if(start_index % 2 == 0){
		if(Rank != 0){
			MPI_Recv(&rec_num, 1, MPI_INT, Rank-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(&array[0], 1, MPI_INT, Rank-1, 2, MPI_COMM_WORLD);
			if(rec_num > array[0])	
				array[0] = rec_num;
		}
		for(int i=1; i<array_len-1; i++){
			if(array[i]>array[i+1]){
				int x = array[i];
				array[i] = array[i+1];
				array[i+1] = x;
			}
		}
		if(array_len%2 == 0 && Rank != np-1){
			MPI_Send(&array[array_len-1], 1, MPI_INT, Rank+1, 2, MPI_COMM_WORLD);
			MPI_Recv(&rec_num1, 1, MPI_INT, Rank+1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(rec_num1 < array[array_len-1])
				array[array_len-1] = rec_num1;
		}

	}
	else{
		for(int i=0; i<array_len-1; i++){
			if(array[i]>array[i+1]){
				int x = array[i];
				array[i] = array[i+1];
				array[i+1] = x;
			}
		}
		if(!(array_len%2 == 0) && Rank != np-1){
			MPI_Send(&array[array_len-1], 1, MPI_INT, Rank+1, 2, MPI_COMM_WORLD);
			MPI_Recv(&rec_num1, 1, MPI_INT, Rank+1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(rec_num1 < array[array_len-1])
				array[array_len-1] = rec_num1;
		}

	}
}


void print_array(int* array, int array_len){


    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int count = 0;
	if(world_rank == 0){
		for(int i=0; i<array_len; i++){
			printf("%d ",array[i]);
		}

        count += array_len;

		for(int j=1; j<world_size; j++){
			MPI_Status status;
			int rec_array[array_len];
			int len;
			MPI_Recv(rec_array, array_len, MPI_INT, j, 1, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_INT, &len);
			for(int k=0; k<len; k++){
				printf("%d ",rec_array[k]);
			}
			//printf("\n");
		}
		printf("\n");
	}
	else {
		MPI_Send(array, array_len, MPI_INT, 0, 1, MPI_COMM_WORLD);
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

	int len = atoi(argv[1]); 

	int array[len/world_size + 1];
	int array_len;
	
	if(world_size > len){
		if(world_rank == 0){
			printf("Number of Elements must be greater than Number of processes\n");
		}
	    	MPI_Finalize();
		return 0;
	}
	if(world_rank == 0){
		int arr[len];
		printf("Elements before Sorting\n");
		for(int i=0;i<len;i++){
			arr[i] = rand() % 100 + 1;
			printf("%d ", arr[i]);
		}
		printf("\n");
		int count = 0;

		for(int i=0; i<world_size; i++){
			int dis_size = get_num_elements(world_size, len, i);

			int arr_to_send[dis_size];
			for(int j=0; j<dis_size; j++){
				arr_to_send[j] = arr[count+j];

			}
			count += dis_size;
			MPI_Send(arr_to_send, dis_size, MPI_INT, i, 0, MPI_COMM_WORLD);			
		}
	}
	MPI_Status status;
	MPI_Recv(array, (len/world_size) + 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_INT, &array_len);

	for(int i=0; i<len; i++){
		even_phase(world_size, len, array, array_len, world_rank);
		MPI_Barrier(MPI_COMM_WORLD);
		odd_phase(world_size, len, array, array_len, world_rank);
		MPI_Barrier(MPI_COMM_WORLD);
	}
	if(world_rank == 0){
	    printf("Elements after  Sorting\n");
	}
	print_array(array, array_len);

    // Finalize the MPI environment.
    MPI_Finalize();
}

