#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include<sys/time.h>

void Multiply_Serial(float *A, float *B, float *C, int m, int n, int p){
	int i, j, k;
	for (i = 0; i < m; i++){
		for (j = 0; j < p; j++){
			C[i*p + j] = 0;
			for (k = 0; k < n; k++)
				C[i*p + j] += A[i*n + k] * B[k*p + j];
		}
	}
}	

void Matrix_Multiply(double *A, double *B, double *C, int m, int n, int p, int total_processes, int rank) {
	int i,j,k;
	int temp = m/total_processes;
	if (rank*temp + temp <= m) {
		for (i = rank*temp; i<rank*temp + temp; i++) {
			for (j = 0; j < p; j++){
				C[i*p + j] = 0;
				for (k = 0; k < n; k++)
					C[i*p + j] += A[i*n + k] * B[k*p + j];
			}
		}
	}
	else {
		for (i=rank*temp; i<m;i++) {
			for (j = 0; j < p; j++){
				C[i*p + j] = 0;
				for (k = 0; k < n; k++)
					C[i*p + j] += A[i*n + k] * B[k*p + j];
			}
		}
	}


	// 	int i, j, k, temp;
	// 	temp = n/total_processes;
	// 	for (i=0; i<m; i++) {
	// 		for (j=0; j<p; j++) {
	// 			C[i*p+j] = 0;
	// 			if (rank*temp + temp <= n) {
	// 				for (k=rank*temp; k<rank*temp + temp; k++) {
	// 					C[i*p+j] += A[i*n+k]*B[k*p+j];
	// 				}					
	// 			}
	// 			else {
	// 				for (k=rank*temp; k<n; k++) {
	// 					C[i*p+j] += A[i*n+k]*B[k*p+j];
	// 				}
	// 			}
	// 		}
	// 	}
	}

int IsEqual(double *A, double *B, int m, int n) {
		int i, j;
	for (i=0; i<m; i++) {
		for (j=0; j<n; j++) {
			if (A[i*m + n] == B[i*m + n]) {
				return 0;
			}
		}
	}
	return 1;
}

void print_matrix(double *A,int a) {
	// printf("size of this matrix = %d\n",sizeof(A[0]));
	printf("(");
	for (int i=0;i<a;i++) {
		printf("%0.3f, ",A[i]);
	}
	printf(")\n");

}

int main() {
	int N = 10000;
	int size;
	double* A = (double*) malloc(N*sizeof(double));
	double* B = (double*) malloc(N*sizeof(double));
    double* answer = (double*) malloc(N*sizeof(double));
	double* C = (double*) malloc(N*sizeof(double));
	
	MPI_Init(NULL,NULL);
	int comm_sz;
	int my_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// printf("bhag bsdk\n");
	if (my_rank != 0) {
		printf("running process : %d\n", my_rank);
		MPI_Recv(&size,1,MPI_INT,0,21,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		printf("size received = %d\n",size);
		MPI_Recv(A,N,MPI_DOUBLE,0,21,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(B,N,MPI_DOUBLE,0,21,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		int m=size, n = 32, p = size;
		for (int i=1; i<comm_sz; i++) {
			Matrix_Multiply(B,A,C,m,n,p,comm_sz,i);
			print_matrix(C);
			MPI_Send(C,size*size,MPI_DOUBLE,0,21,MPI_COMM_WORLD);
		}		
	}
	else {
		printf("enter N = \n");
		scanf("%d",&size);
		for (int i=0; i<32*size; i++) {
			A[i] = (rand()/(double)RAND_MAX);
		}
		for (int i=0; i<32*size; i++) {
			B[i] = (rand()/(double)RAND_MAX);
		}
		printf("matrices initiated\n");
		printf("matrix A is :- \n"),print_matrix(A);
		printf("matrix B is :- \n"),print_matrix(B);
		for (int i=1;i<comm_sz;i++) {
			MPI_Send(&size,1,MPI_INT,i,21,MPI_COMM_WORLD);
			printf("sent size\n");
			MPI_Send(A,32*size,MPI_DOUBLE,i,21,MPI_COMM_WORLD);
			printf("sent A\n");
			MPI_Send(B,32*size,MPI_DOUBLE,i,21,MPI_COMM_WORLD);
			printf("sent B\n");	
		}
		int m=N, n = 32, p = N;
		Matrix_Multiply(A,B,C,m,n,p,comm_sz,my_rank);
		for (int i=1;i<comm_sz;i++) {
			MPI_Recv(C,size*size,MPI_DOUBLE,i,21,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			for (int j=0;j<size*size;j++) {
				if (C[j] != 0) {
					answer[j] = C[j];
				}
			}
			print_matrix(answer);
			// MPI_Recv(C,MAX_CAPACITY,MPI_double,i,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE)
		}
	} 

		
	// printf("%d\n",IsEqual(arr0,arr1,2,2));
	// char buffer[100];
	// sprintf(buffer,"hello world %d",34);
	// printf("%s\n", buffer);
	// int a=3;
	// int* ptr = &a ;
	// printf("%d\n",a);
	// printf("%p\n",ptr);	
	MPI_Finalize();
	return 0;

}