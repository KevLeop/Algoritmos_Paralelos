//    ./pth_mat_vect <thread_count> <m> <n>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

int     thread_count;
int     m, n;
double* A;
double* x;
double* y;
//Funciones seriales
void Instrucciones(char* prog_name);
void Generar_matriz(double A[], int m, int n);
void Leer_matriz(char* prompt, double A[], int m, int n);
void Generar_vector(double x[], int n);
void Leer_vector(char* prompt, double x[], int n);
void Imprimir_matriz(char* title, double A[], int m, int n);
void Imprimir_vector(char* title, double y[], double m);

// Funcion paralela
void *Pth_mat_vect(void* rank);


void Instrucciones (char* prog_name) {
   fprintf(stderr, "Instrucciones: %s <thread_count> <m> <n>\n", prog_name);
   exit(0);
} 

void Leer_matriz(char* prompt, double A[], int m, int n) {
   int             i, j;

   printf("%s\n", prompt);
   for (i = 0; i < m; i++) 
      for (j = 0; j < n; j++)
         scanf("%lf", &A[i*n+j]);
}


void Generar_matriz(double A[], int m, int n) {
   int i, j;
   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         A[i*n+j] = random()/((double) RAND_MAX);
}


void Generar_vector(double x[], int n) {
   int i;
   for (i = 0; i < n; i++)
      x[i] = random()/((double) RAND_MAX);
} 

void Leer_vector(char* prompt, double x[], int n) {
   int   i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++) 
      scanf("%lf", &x[i]);
}

void *Pth_mat_vect(void* rank) {
   long my_rank = (long) rank;
   int i;
   int j; 
   int local_m = m/thread_count; 
   int my_first_row = my_rank*local_m;
   int my_last_row = my_first_row + local_m;
   register int sub = my_first_row*n;
   double start, finish;
   double temp;

   GET_TIME(start);
   for (i = my_first_row; i < my_last_row; i++) {
      y[i] = 0.0;
      for (j = 0; j < n; j++) {
          temp = A[sub++];
          temp *= x[j];
          y[i] += temp;
      }
   }
   GET_TIME(finish);
   printf("Thread %ld > Elapsed time = %e seconds\n", 
      my_rank, finish - start);

   return NULL;
}


void Imprimir_matriz( char* title, double A[], int m, int n) {
   int   i, j;

   printf("%s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++)
         printf("%6.3f ", A[i*n + j]);
      printf("\n");
   }
}

void Imprimir_vector(char* title, double y[], double m) {
   int   i;

   printf("%s\n", title);
   for (i = 0; i < m; i++)
      printf("%6.3f ", y[i]);
   printf("\n");
}


int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles;

   if (argc != 4) Instrucciones(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   m = strtol(argv[2], NULL, 10);
   n = strtol(argv[3], NULL, 10);


   thread_handles = malloc(thread_count*sizeof(pthread_t));
   A = malloc(m*n*sizeof(double));
   x = malloc(n*sizeof(double));
   y = malloc(m*sizeof(double));
   
   Generar_matriz(A, m, n);

   Generar_vector(x, n);


   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
         Pth_mat_vect, (void*) thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);

   free(A);
   free(x);
   free(y);

   return 0;
}  /* main */