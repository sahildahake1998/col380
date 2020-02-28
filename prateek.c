#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
#define N 10000
int main(int argc, char *argv[]){
    if(argc == 3){
        clock_t start, end;
        start = clock();
        int n, i, j, k, k_dash, num_threads;
        double temp, max;
        
        n = atoi(argv[1]);
        num_threads = atoi(argv[2]);
        omp_set_num_threads(num_threads);

        double **a = (double **)malloc(N * sizeof(double *));
        double **l = (double **)malloc(N * sizeof(double *));
        double **u = (double **)malloc(N * sizeof(double *));
        // printf("before pragma\n");
        // #pragma omp parallel 
        // {
			for (i=0; i<n; i++){
	            a[i] = (double *)malloc(N * sizeof(double)); 
	            l[i] = (double *)malloc(N * sizeof(double)); 
	            u[i] = (double *)malloc(N * sizeof(double));
	        }
	        // Take a[n][n] as input
	        for(i=0;i<n;i++){
	            for(j=0;j<n;j++){
		                a[i][j] = drand48();
		            }
		        }
		    // printf("executed parallel\n");
    	// }
                
        printf("initialisation done \n");
        for (int i=0;i<sizeof(&a);i++) {
            for (int j=0; j<sizeof(&a[i]);j++) {
                printf("(%d,%d) = ",i,j);
                printf("%f",a[i][j]);
                printf("\n");
            }
        }
        
        int* pi = (int*) malloc(n * sizeof(int));;
        
        printf("came outside pi \n");

        for(i=0;i<n;i++){
            for(j=0;j<n;j++){
                u[n][n] = 0;
            }
        }

        for(i=0;i<n;i++){
            for(j=0;j<n;j++){
                // printf("enter for\n");
                if(i==j){
                    // printf("enter equal\n");
                    l[i][j] = 1;
                }
                else{
                    l[i][j] = 0;
                }
            }
        }
        
        for(i=0;i<n;i++){
            pi[i] = i;
        }
        
        printf("Entered the main forloop\n");
        for(k=0;k<n;k++){
            max = 0;
            for(i=k;i<n;i++){
                if(max < abs(a[i][k])){
                    max = abs(a[i][k]);
                    k_dash = i;
                }
            }
            if(max == 0){
                printf("Singular Matrix\n");
            }
            //swap π[k] and π[k']
            temp = pi[k];
            pi[k] = pi[k_dash];
            pi[k_dash] = temp;
            //swap a(k,:) and a(k',:)
            // #pragma omp parallel
            // { 
            for(j=0;j<n;j++){
                temp = a[k][j];
                a[k][j] = a[k_dash][j];
                // #pragma omp critical
                a[k_dash][j] = temp;
            }
            //swap l(k,1:k-1) and l(k',1:k-1)
            for(j=0;j<k;j++){
                temp = l[k][j];
                l[k][j] = l[k_dash][j];
                // #pragma omp critical
                l[k_dash][j] = temp;
            }
            u[k][k] = a[k][k];
            
            for(i=k+1;i<n;i++){
                l[i][k] = a[i][k]/u[k][k];
                // #pragma omp critical
                u[k][i] = a[k][i];
            }
            // }
            // #pragma omp parallel for
            for(i=k+1;i<n;i++){
                for(j=k+1;j<n;j++){
                    a[i][j] = a[i][j] - l[i][k]*u[k][j];
                }
            }
            // #pragma omp barrier
        }
        end = clock();
        printf("Time Taken: %lf\n",(double)(end - start)/CLOCKS_PER_SEC);
        for(int i=0; i<n;i++){
            free(a[i]);
            free(l[i]);
            free(u[i]);
        }
        free(pi);
    }
    else{
        printf("Command line argument error!\n");
    }
    return 0;
}