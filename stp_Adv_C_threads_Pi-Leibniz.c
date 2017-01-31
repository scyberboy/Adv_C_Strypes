#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#define NUM_THREADS 8

const double precision = 1e-12;
double fPi = 0.0;



typedef struct thread_data_t
{
   pthread_t th_id;
	int th_num;
	float sign;
	int divisor;
	double result;
} thread_data;


void * worker_func( void * t_data )
{
   thread_data *w_res_ptr = (thread_data *) t_data;
   unsigned long long l_divisor = w_res_ptr->divisor;
   float l_sign = w_res_ptr->sign;
   
   while( 1.0 / l_divisor > precision )
   {
      w_res_ptr->result += ( ( l_sign * 1.0 ) / l_divisor );
      
      l_divisor += 2*NUM_THREADS; // -3, +5, -7, +9, -11, +13, etc...
      l_sign = ( NUM_THREADS % 2 == 0) ? l_sign : -1.0 * l_sign;            
   }

   return NULL;
}


int main (int argc, char* argv[])
{
	int i = 0;
	thread_data thr_data[NUM_THREADS] = {{0}};

	// spawn the threads
	for( i = 0 ; i < NUM_THREADS ; i++ )
	{
	   thr_data[i].sign = ( i % 2 == 0 ) ? -1 : 1;
	   thr_data[i].divisor = 3 + 2*i;
	   thr_data[i].th_num = i+1;
	   thr_data[i].result = 0.0;
	   
	   pthread_create( &thr_data[i].th_id, NULL, worker_func, (void *)&thr_data[i] );
	}

	// wait for threads to do 'some' work :)
	sleep(1);

	// calc Pi
	fPi = 1.0;

	// loop thru thread's data and add each result (join all threads)
	for( i = 0 ; i < NUM_THREADS ; i++ )
	{
	   pthread_join( thr_data[i].th_id, NULL );
	   thread_data *l_res_ptr = &thr_data[i];
	   
	   fPi += l_res_ptr->result;

	   printf( "Worker %d(%p) yields %.11g ==> 1/4th of fPi(%.11g)\n", l_res_ptr->th_num, l_res_ptr, l_res_ptr->result, fPi );
	   
	}

	printf( "finish: fPi(%.12g)\n", 4.0 * fPi );

	return 0;
}
