/*  gcc pth_pi_busy1.c -lm -lpthread -o pth_pi_busy1
 *  ./pth_py_busy1 <nro threads> <n>
 */        
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

const int MAX_THREADS = 1024;

long thread_count;
long long n;
int flag;
double sum;

void* Thread_sum(void* rank);
void Get_args(int argc, char* argv[]); // solo ejecuta el thread principal
void Instrucciones(char* prog_name);
double Serial_pi(long long n);

/// Add in the terms computed by the thread running this 
void* Thread_sum(void* rank) 
{
   long my_rank = (long) rank;
   double factor;
   long long i;
   long long my_n = n/thread_count;
   long long primer_i = my_n*my_rank;
   long long ultimo_i = primer_i + my_n;

   if (primer_i % 2 == 0)
      factor = 1.0;
   else
      factor = -1.0;

   for (i = primer_i; i < ultimo_i; i++, factor = -factor) //bucle while dentro
   {
      while (flag != my_rank);
      sum += factor/(2*i+1);  
      flag = (flag+1) % thread_count;
   }
   return NULL;
} 

double Serial_pi(long long n) 
{
   double sum = 0.0;
   long long i;
   double factor = 1.0;

   for (i = 0; i < n; i++, factor = -factor) 
   {
      sum += factor/(2*i+1);
   }
   return 4.0*sum;
}

void Get_args(int argc, char* argv[]) 
{
   if (argc != 3) Instrucciones(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Instrucciones(argv[0]);
   n = strtoll(argv[2], NULL, 10);
   if (n <= 0) Instrucciones(argv[0]);
} 

void Instrucciones(char* prog_name) 
{
   fprintf(stderr, "Instrucciones: %s <number of threads> <n>\n", prog_name);
   fprintf(stderr, "   n is the number of terms and should be >= 1\n");
   fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
   exit(0);
}


int main(int argc, char* argv[]) 
{
   long       thread;  // Usa long en caso de sistemas de 64-bits
   pthread_t* thread_handles;
   double inicio, final, transcurrido;

   Get_args(argc, argv); // obtiene el numero de threads y nro de terminos desde linea de comando

   thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t)); 
   
   GET_TIME(inicio);
   sum = 0.0;
   flag = 0;
   for (thread = 0; thread < thread_count; thread++)  
      pthread_create(&thread_handles[thread], NULL,
          Thread_sum, (void*)thread);  

   for (thread = 0; thread < thread_count; thread++) 
      pthread_join(thread_handles[thread], NULL); 
   GET_TIME(final);
   transcurrido = final - inicio;

   sum = 4.0*sum;
   printf("Con n = %lld terminos,\n", n);
   printf("   Calculo de pi con threads  = %.15f\n", sum);
   printf("   Tiempo transcurrido = %e segundos\n", transcurrido);

   GET_TIME(inicio);
   sum = Serial_pi(n);
   GET_TIME(final);
   transcurrido = final - inicio;
   printf("   Calculo serial de pi = %.15f\n", sum);
   printf("   Tiempo transcurrido = %e seconds\n", transcurrido);
   printf("   Calculo de pi con math lib    = %.15f\n", 4.0*atan(1.0));
   
   free(thread_handles);
   return 0;
} 
