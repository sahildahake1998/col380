#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<string.h>
#include<pthread.h>
#define N 10000

int max_threads;
int n;
double **a;
double **l;
double **u;
double *localMax;
int *maxK;
int k;
int threadID = 0;

void swap(double *x, double *y){ 
    double temp = *x;
    *x = *y;
    *y = temp; 
} 

void* maximum(void* arg){
    int i, myThreadID = threadID++;
    int n_dash = (n/max_threads);
    int startIndex = myThreadID*n_dash;
    int stopIndex;
    if(myThreadID != max_threads-1){
        stopIndex = startIndex + n_dash;
    }
    else{
        stopIndex = n;
    }
    double maxs = a[startIndex][k];
    int maxk = startIndex; 
    for(i=startIndex+1; i<stopIndex; i++){ 
        if(a[i][k] > maxs){
            maxs = a[i][k];
            maxk = i;
        }
    }
    localMax[myThreadID] = maxs;
    maxK[myThreadID] = maxk;
    pthread_exit(NULL);
}

void* computeA(void* arg){
    int i, j, myThreadID = threadID++;
    int n_dash = (n-k-1)/max_threads;
    int startIndex = k+1+(myThreadID*n_dash);
    int stopIndex;
    if(myThreadID != max_threads-1){
        stopIndex = startIndex + n_dash;
    }
    else{
        stopIndex = n;
    }
    for(i=startIndex;i<stopIndex;i++){
        for(j=k+1;j<n;j++){
            a[i][j] -= l[i][k]*u[k][j];
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    if(argc == 3){
        struct timeval start, end; 
        gettimeofday(&start, NULL); 
        int i, j, k_dash, n_dash;
        double temp, max, time_taken;
        n = atoi(argv[1]);
        max_threads = atoi(argv[2]);
        int id, startIndex, stopIndex, nthrds;
        localMax = (double*)malloc(max_threads * sizeof(double));
        maxK = (int*)malloc(max_threads * sizeof(int));
        
        a = (double **)malloc(N * sizeof(double *));
        l = (double **)malloc(N * sizeof(double *));
        u = (double **)malloc(N * sizeof(double *));

        for (i=0; i<N; i++){
            a[i] = (double *)malloc(N * sizeof(double)); 
            l[i] = (double *)malloc(N * sizeof(double)); 
            u[i] = (double *)malloc(N * sizeof(double));
        }

        // Take a[n][n] as input
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

        pthread_t threads[max_threads];
        printf("Entered the main forloop\n");

        for(k=0;k<n;k++){
            printf("%d\n",k);
            threadID = 0;
            for(i=0;i<max_threads;i++){
                pthread_create(&threads[i], NULL, &maximum, NULL);
            }

            for (i = 0; i<max_threads; i++){
                pthread_join(threads[i], NULL);
            }
            
            max = localMax[0];
            k_dash = maxK[0];
            for (i = 1; i < max_threads; i++){
                if(localMax[i] > max){
                    max = localMax[i];
                    k_dash = maxK[i];
                }
            } 

            if(max == 0){
                printf("Singular Matrix\n");
            }
            
            temp = pi[k];
            pi[k] = pi[k_dash];
            pi[k_dash] = temp;
            
            for(j=0;j<n;j++){
                swap(&a[k][j], &a[k_dash][j]);
                if(j<k){
                    swap(&l[k][j], &l[k_dash][j]);    
                }
            }
        
            u[k][k] = a[k][k];
        
            for(i=k+1;i<n;i++){
                l[i][k] = a[i][k]/u[k][k];
                u[k][i] = a[k][i];
            }
            
            threadID = 0;
            
            for(i=0;i<max_threads;i++){
                pthread_create(&threads[i], NULL, &computeA, NULL);
            }
            
            for (i = 0; i<max_threads; i++){
                pthread_join(threads[i], NULL);
            }


            
            // for(i=k+1;i<n;i++){
            //     for(j=k+1;j<n;j++){
            //         a[i][j] -= l[i][k]*u[k][j];
            //     }
            // }

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