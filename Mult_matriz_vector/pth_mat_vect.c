/* 
 *     m, n: order of matrix
 *     A, x: the matrix and the vector to be multiplied
      gcc -g -Wall -o pth_mat_vect pth_mat_vect.c -lpthread
 *    ./ pth_mat_vect <thread_count>
 *
 * Notes:  
 *     1.  Local storage for A, x, y is dynamically allocated.
 *     2.  Number of threads (thread_count) should evenly divide both 
 *         m and n.  The program doesn't check for this.
 *     3.  We use a 1-dimensional array for A and compute subscripts
 *         using the formula A[i][j] = A[i*n + j]
 *     4.  Distribution of A, x, and y is logical:  all three are 
 *         globally shared.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int     thread_count;
int     m, n;
double* A;
double* x;
double* y;

//FUnciones seriales
void Instrucciones(char* prog_name);
void Leer_matriz(char* prompt, double A[], int m, int n);
void Leer_vector(char* prompt, double x[], int n);
void Imprimir_matrix(char* title, double A[], int m, int n);
void Imprimir_vector(char* title, double y[], double m);

// FUnciones paralelas
void *Pth_mat_vect(void* rank);



void Instrucciones (char* prog_name) {
   fprintf(stderr, "Instrucciones: %s <thread_count>\n", prog_name);
   exit(0);
}

void Leer_matriz(char* prompt, double A[], int m, int n) {
   int             i, j;

   printf("%s\n", prompt);
   for (i = 0; i < m; i++) 
      for (j = 0; j < n; j++)
         scanf("%lf", &A[i*n+j]);
} 

void Leer_vector(char* prompt, double x[], int n) {
   int   i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++) 
      scanf("%lf", &x[i]);
} 

void *Pth_mat_vect(void* rank) {
   long my_rank = (long) rank;
   int i, j;
   int local_m = m/thread_count; 
   int my_first_row = my_rank*local_m;
   int my_last_row = (my_rank+1)*local_m - 1;

   for (i = my_first_row; i <= my_last_row; i++) {
      y[i] = 0.0;
      for (j = 0; j < n; j++)
          y[i] += A[i*n+j]*x[j];
   }

   return NULL;
}

void Imprimir_matrix( char* title, double A[], int m, int n) {
   int   i, j;

   printf("%s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++)
         printf("%4.1f ", A[i*n + j]);
      printf("\n");
   }
} 

void Imprimir_vector(char* title, double y[], double m) {
   int   i;

   printf("%s\n", title);
   for (i = 0; i < m; i++)
      printf("%4.1f ", y[i]);
   printf("\n");
} 


int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles;

   if (argc != 2) Instrucciones(argv[0]);
   thread_count = atoi(argv[1]);
   thread_handles = malloc(thread_count*sizeof(pthread_t));

   printf("Enter m and n\n");
   scanf("%d%d", &m, &n);

   A = malloc(m*n*sizeof(double));
   x = malloc(n*sizeof(double));
   y = malloc(m*sizeof(double));
   
   Leer_matriz("Ingrese la matriz", A, m, n);
   Imprimir_matrix("Matriz leida: ", A, m, n);

   Leer_vector("Ingrese el vector", x, n);
   Imprimir_vector("Vector leido: ", x, n);

   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
         Pth_mat_vect, (void*) thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);

   Imprimir_vector("El producto es:", y, m);

   free(A);
   free(x);
   free(y);

   return 0;
}