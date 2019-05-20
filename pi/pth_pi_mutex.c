/* gcc pth_pth_pi_mutex.c -lm -lpthread -o pth_pi_mutex
   ./pth_pi_mutex.c <nro threads> <n>
   Esta version usa mutex para proteger la sección crítica
*/        
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

const int MAX_THREADS = 1024;

long thread_count;
long long n;
double sum;
pthread_mutex_t mutex;

void* Thread_sum(void* rank);
void Get_args(int argc, char* argv[]); // solo ejecuta el thread principal
void Instrucciones(char* prog_name);
double Serial_pi(long long n);

void* Thread_sum(void* rank) 
{
   long my_rank = (long) rank;
   double factor;
   long long i;
   long long my_n = n/thread_count;
   long long my_first_i = my_n*my_rank;
   long long my_last_i = my_first_i + my_n;
   double my_sum = 0.0;

   if (my_first_i % 2 == 0)
      factor = 1.0;
   else
      factor = -1.0;

   for (i = my_first_i; i < my_last_i; i++, factor = -factor) 
   {
      my_sum += factor/(2*i+1);
   }
   pthread_mutex_lock(&mutex);
   sum += my_sum;
   pthread_mutex_unlock(&mutex);

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
   fprintf(stderr, "   n es el numero de terminos y debe ser >= 1\n");
   fprintf(stderr, "   n debe ser divisible entre el numero de threads\n");
   exit(0);
}

int main(int argc, char* argv[]) 
{
   long       thread;
   pthread_t* thread_handles;
   double inicio, final, transcurrido;

   Get_args(argc, argv); // obtiene el nro de threads desde linea de comando

   thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t)); 
   pthread_mutex_init(&mutex, NULL);
   sum = 0.0;

   GET_TIME(inicio);
   for (thread = 0; thread < thread_count; thread++)  
      pthread_create(&thread_handles[thread], NULL,
          Thread_sum, (void*)thread);  

   for (thread = 0; thread < thread_count; thread++) 
      pthread_join(thread_handles[thread], NULL); 
   GET_TIME(final);
   transcurrido = final - inicio;

   sum = 4.0*sum;
   printf("Con n = %lld terminos,\n", n);
   printf("   Calculo de pi = %.15f\n", sum);
   printf("El tiempo transcurrido es %e seconds\n", transcurrido);
   GET_TIME(inicio);
   sum = Serial_pi(n);
   GET_TIME(final);
   transcurrido = final - inicio;
   printf("   Calculo serial de pi  = %.15f\n", sum);
   printf("El tiempo transcurrido es %e seconds\n", transcurrido);
   printf(" Calculo de pi usando mat lib = %.15f\n", 4.0*atan(1.0));
   
   pthread_mutex_destroy(&mutex);
   free(thread_handles);
   return 0;
}