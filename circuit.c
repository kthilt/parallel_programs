//circuit.c
//Dr. Karlsson, Kevin Hilt
//CSC410
//October 27, 2014
//Purpose:  Determine circuit satisfiability for 16 inputs using both static
//          and dynamic scheduling in openmp.
//Compile:
//          gcc -g -Wall -fopenmp -o circuit circuit.c -lm
//Run:      ./circuit <n>
//Input:    n threads given at the command line
//Output:   Messages signalling the solutions and timing information
//Notes:    The majority of this code was provided by Dr. Karlsson.

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* Return 1 if 'i'th bit of 'n' is 1; 0 otherwise */
#define EXTRACT_BIT(n,i) ((n&(1<<i))?1:0)

/* Function:  check_circuit()
 * Purpose:   Check if a given input produces TRUE (a one) 
 *
 * In args:      id, the process id
 *                z, the current combination
 * out:           1, if the the input TRUE in the circuit
 *                0, else
 */
int check_circuit (int id, int z) 
{
   int v[16];        /* Each element is a bit of z */
   int i;

   for (i = 0; i < 16; i++) v[i] = EXTRACT_BIT(z,i);
   if ((v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
      && (!v[3] || !v[4]) && (v[4] || !v[5])
      && (v[5] || !v[6]) && (v[5] || v[6])
      && (v[6] || !v[15]) && (v[7] || !v[8])
      && (!v[7] || !v[13]) && (v[8] || v[9])
      && (v[8] || !v[9]) && (!v[9] || !v[10])
      && (v[9] || v[11]) && (v[10] || v[11])
      && (v[12] || v[13]) && (v[13] || !v[14])
      && (v[14] || v[15])) 
   {
      #ifndef DEBUG
      printf ("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", id,
         v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],
         v[10],v[11],v[12],v[13],v[14],v[15]);
      fflush (stdout);
      #endif
      return 1;
   } else return 0;
}

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) 
{
   fprintf(stderr, "usage: %s <n>\n", prog_name);  /* Change */
   fprintf(stderr, "   n is the number of threads >= 1\n");
   exit(0);
}  /* Usage */


/*------------------------------------------------------------------
 * Function:  parallel_static
 * Purpose:   Run tests with schedule(static, 1)
 * In arg:    thread_count
 */
int parallel_static(int thread_count)
{
    int i;
    int global_count = 0;
    int input_maximum = 65536;
    
#pragma omp parallel for num_threads(thread_count)  \
    shared(global_count) schedule(static, 1)
    
    for(i = 0; i < input_maximum; i++)
    {
        //If the current number is a solution, increment the count
        if( check_circuit(omp_get_thread_num(), i) )
        {
            global_count++;
        }
    }
    
    return global_count;
}

/*------------------------------------------------------------------
 * Function:  parallel_dynamic
 * Purpose:   Run tests with schedule(dynamic, 1)
 * In arg:    thread_count
 */
int parallel_dynamic(int thread_count)
{
    int i;
    int global_count = 0;
    int input_maximum = 65536;
    
#pragma omp parallel for num_threads(thread_count)  \
    shared(global_count) schedule(dynamic, 1)
    
    for(i = 0; i < input_maximum; i++)
    {
        //If the current number is a solution, increment the count
        if( check_circuit(omp_get_thread_num(), i) )
        {
            global_count++;
        }
    }
    
    return global_count;
}

int main (int argc, char *argv[]) 
{
   int global_count;     /* Total number of solutions */
   int thread_count;     /* Number of processes */
   double start, finish, static_time, dynamic_time; /* Timing variables */
   int i;
   int loop_number; //Equal to thread_count unless DEBUG is defined
   int trial_number = 1; //Equal to 1 unless DEBUG is defined

    #ifndef DEBUG
   if (argc != 2) Usage(argv[0]);
   {
      thread_count =  strtoll(argv[1], NULL, 10);
      loop_number = thread_count; //Only go through loop once
   }
   #else
      thread_count = 2; //Start at two processors
      loop_number = 64; //Loop until 64 processors
      trial_number = 10; //Do 10 trials per processor count
   #endif
   
   for(thread_count = thread_count; thread_count <= loop_number; thread_count *= 2)
   {
   
   //Run tests with static
   #ifndef DEBUG
   printf("schedule(static, 1):\n");
   #else
   printf("%d processors:\n", thread_count);
   #endif
   
       for(i = 0; i < trial_number; i++) //Runs once unless DEBUG is defined
       {
       
           start = omp_get_wtime();
           global_count = parallel_static(thread_count); //Run with schedule(static, 1)
           finish = omp_get_wtime();
           static_time = finish - start;
           #ifndef DEBUG
           printf("Total solutions: %d\nTime: %e\n\n", global_count, static_time);
           
           //Run tests with dynamic
           printf("schedule(dynamic, 1):\n");
           #endif
           start = omp_get_wtime();
           global_count = parallel_dynamic(thread_count); //Run with schedule(dynamic, 1)
           finish = omp_get_wtime();
           dynamic_time = finish - start;
           #ifndef DEBUG
           printf("Total solutions: %d\nTime: %e\n\n", global_count, dynamic_time);
           #endif
           
           start = dynamic_time - static_time;
           if(start < 0) //Static took longer
           {
                printf("Dynamic was faster by %e\n", (start * -1));
           }
           else //Static took longer
           {
                printf("Static was faster by %e\n", start);
           }
       
       }
       
       printf("\n");
   }

   return 0;
}

