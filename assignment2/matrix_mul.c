#include<stdio.h>
#include <stdlib.h>
#include<omp.h>
#include <time.h>

int main(int argc, char* argv[])
{
	int n = atoi(argv[1]);
	int m = atoi(argv[2]);
	int p = atoi(argv[3]);
	
	int **a = (int **)malloc(n * sizeof(int*));
	for(int i = 0; i < n; i++) 
		a[i] = (int *)malloc(m * sizeof(int));

	int **b = (int **)malloc(m * sizeof(int*));
	for(int i = 0; i < m; i++) 
		b[i] = (int *)malloc(p * sizeof(int));

	int **c = (int **)malloc(n * sizeof(int*));
	for(int i = 0; i < n; i++) 
		c[i] = (int *)malloc(p * sizeof(int));



	for(int i = 0; i < n; ++i){
		for(int j = 0; j < m; ++j){
			a[i][j] = rand() % 100 + 1;;
		}
	}

	for(int i = 0; i < m; ++i){
		for(int j = 0; j < p; ++j){
			b[i][j] = rand() % 100 + 1;;
		}
	}

  	time_t start, end;
	time(&start);
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < p; ++j){
		    for(int k = 0; k < m; ++k){
		        c[i][j] += a[i][k] * b[k][j];
		    }
		}
	}
	time(&end);
	double time_taken = difftime(end, start);
	printf("Matrix multiplication took %f seconds to execute\n", time_taken);



  	time_t start1, end1;
	time(&start1);
	#pragma omp parallel for
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < p; ++j){
		    for(int k = 0; k < m; ++k){
		        c[i][j] += a[i][k] * b[k][j];
		    }
		}
	}
	time(&end1);
	double time_taken1 = difftime(end1, start1);
 
	printf("Matrix multiplication took %f seconds to execute after parallalization\n", time_taken1);
	
	for(int i = 0; i < n; i++) 
		free(a[i]);
	free(a);


	for(int i = 0; i < m; i++) 
		free(b[i]);
	free(b);


	for(int i = 0; i < n; i++) 
		free(c[i]);
	free(c);

	
	return 0;
}
