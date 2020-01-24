#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<omp.h>
#include<string.h>
#define N 10000
int main(int argc, char *argv[]){
    if(argc == 3){
        struct timeval start, end; 
        gettimeofday(&start, NULL); 
        int n, i, j, k, k_dash, max_threads, n_dash;
        double temp, max, time_taken;
        n = atoi(argv[1]);
        max_threads = atoi(argv[2]);
        omp_set_num_threads(max_threads);
        int id, startIndex, stopIndex, nthrds;

        double **a = (double **)malloc(N * sizeof(double *));
        double **l = (double **)malloc(N * sizeof(double *));
        double **u = (double **)malloc(N * sizeof(double *));

        for (i=0; i<N; i++){
            a[i] = (double *)malloc(N * sizeof(double)); 
            l[i] = (double *)malloc(N * sizeof(double)); 
            u[i] = (double *)malloc(N * sizeof(double));
        }

        // Take a[n][n] as input
        #pragma omp parallel for num_threads(max_threads) collapse(2)
        for(i=0;i<n;i++){
            for(j=0;j<n;j++){
                a[i][j] = drand48()*1000;
            }
        }
        double maxArray[max_threads];
        int k_dashArray[max_threads];
        for(i=0;i<max_threads;i++){
            maxArray[i] = 0;
        }

        int* pi = (int*) malloc(n * sizeof(int));
        
        #pragma omp parallel for num_threads(max_threads) collapse(2)
        for(i=0;i<n;i++){
            for(j=0;j<n;j++){
                u[n][n] = 0;
                if(i==j){
                    l[i][j] = 1;
                    pi[i] = i;
                }
                else{
                    l[i][j] = 0;
                }
            }
        }

        printf("Entered the main forloop\n");

        for(k=0;k<n;k++){
            printf("%d\n",k);

            #pragma omp parallel
            {
                nthrds = omp_get_num_threads();
                n_dash = n/nthrds;
                id = omp_get_thread_num();
                startIndex = id*n_dash;
                if(id != nthrds-1){
                    stopIndex = startIndex + n_dash;
                }
                else{
                    stopIndex = n;
                }
                max = a[startIndex][k];
                k_dash = startIndex;
                for(i = startIndex+1; i < stopIndex; i++){
                    if(max < abs(a[i][k])){
                        max = abs(a[i][k]);
                        k_dash = i;
                    }
                }
                maxArray[id] = max;
                k_dashArray[id] = k_dash;
            }

            max = maxArray[0];
            k_dash = k_dashArray[0];
            for (i=1;i<max_threads;i++){
                if(maxArray[i] > max){
                    max = maxArray[i];
                    k_dash = k_dashArray[i];
                }
            }

            // max = 0;
            // for(i=k;i<n;i++){
            //     if(max < abs(a[i][k])){
            //         max = abs(a[i][k]);
            //         k_dash = i;
            //     }
            // }
            
            if(max == 0){
                printf("Singular Matrix\n");
            }
            
            temp = pi[k];
            pi[k] = pi[k_dash];
            pi[k_dash] = temp;
            
            #pragma omp parallel for num_threads(max_threads)
            for(j=0;j<n;j++){
                temp = a[k][j];
                a[k][j] = a[k_dash][j];
                a[k_dash][j] = temp;
            }
        
            #pragma omp parallel for num_threads(max_threads)
            for(j=0;j<k;j++){
                temp = l[k][j];
                l[k][j] = l[k_dash][j];
                l[k_dash][j] = temp;
            }
        
            u[k][k] = a[k][k];
        
            #pragma omp parallel for num_threads(max_threads)
            for(i=k+1;i<n;i++){
                l[i][k] = a[i][k]/u[k][k];
                u[k][i] = a[k][i];
            }

            #pragma omp parallel for num_threads(max_threads) collapse(2)
            for(i=k+1;i<n;i++){
                for(j=k+1;j<n;j++){
                    a[i][j] -= l[i][k]*u[k][j];
                }
            }
        }

        gettimeofday(&end, NULL); 
        time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
        time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6; 


        printf("Time Taken: %lf\n",time_taken);
        free(a);
        free(l);
        free(u);
        free(pi);
    }
    else{
        printf("Command line argument error!\n");
    }
    return 0;
}