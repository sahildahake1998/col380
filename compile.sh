# gcc -g -Wall -fopenmp lu_decomposition_serial.c -o lu_decomposition_serial
gcc -g -Wall -O3 -fopenmp lu_decomposition_openmp.c -o lu_decomposition_openmp 
gcc -O3 -pthread lu_decomposition_pthread.c -o lu_decomposition_pthread