//prime.c
//Dr. Karlsson, Kevin Hilt
//CSC410
//October 27, 2014
//Purpose:  List all prime numbers less than given number n using 
//          a C implementation of Eratosthenes' sieve.
//Compile:
//          gcc -g -Wall -fopenmp -o prime prime.c -lm
//Run:      ./prime <n>
//Input:    n (generate all primes less than n) given at the command line
//Output:   Primes less than n and timing informaiton
//Notes:    The Usage() function was provided by Dr. Karlsson.

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) 
{
   fprintf(stderr, "usage: %s <n>\n", prog_name);  /* Change */
   fprintf(stderr, "   print all primes less than n\n");
   exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function:  mark_multiples
 * Purpose:   In passed marked array, mark all multiples of passed number
 * In arg:    number, user_maximum, marked[]
 */
void mark_multiples(int number, int user_maximum, int* marked)
{
    int i;
    
    //Mark all multiples of number less than user_maximum
    for(i = number + number; i < user_maximum; i += number)
    {
        marked[i] = 1;
    }
}

/*------------------------------------------------------------------
 * Function:  find_next_unmarked
 * Purpose:   Return next unmarked after passed number
 * In arg:    number, user_maximum, marked[]
 */
int find_next_unmarked(int number, int user_maximum, int* marked)
{
    int i = number + 1;
    
    //While still less than user_maximum
    while(i < user_maximum)
    {
        //See if i is unmarked
        if(marked[i] == 0)
        {
            return i;
        }
        
        i++; //Try next one if not
    }
    
    return user_maximum; //No unmarked spots left
}

/*------------------------------------------------------------------
 * Function:  print_primes
 * Purpose:   Prints all the primes (indexes with 0 in marked[]) and
 *            conditional line numbers for 10-number lines
 * In arg:    user_maximum, marked[]
 */
void print_primes(int user_maximum, int* marked)
{
    int line_number = 0;
    int i;
    
    for(i = 2; i < user_maximum; i++)
    {   
        //Print and increment line_number if marked[i] is prime
        if(marked[i] == 0)
        {
            //Print 10 primes per line
            if(line_number % 10 == 0)
            {
                printf("\n%d: ", line_number);
            }
        
            printf("%d ", i);
            line_number++;
        }
    }
}

/*------------------------------------------------------------------
 * Function:  parallel_static
 * Purpose:   Print all primes less than user_maximum with schedule(static, 1)
 * In arg:    thread_count, user_maximum, marked[]
 */
void parallel_static(int thread_count, int user_maximum, int* marked)
{
    int i;
    
#pragma omp parallel for num_threads(thread_count)  \
    shared (marked) schedule(static, 1)
    
    //Starting at i = 1 because find_next_prime's first call will set it to 2
    for(i = 1; i < user_maximum; i++)
    {
        //Set i to the next unmarked number
        i = find_next_unmarked(i, user_maximum, marked);
        
        //If i is user_maximum, there are no unmarked spots left
        if(i != user_maximum)
        {
            //Mark all multiples of i
            mark_multiples(i, user_maximum, marked);
        }
    }
}

/*------------------------------------------------------------------
 * Function:  parallel_dynamic
 * Purpose:   Print all primes less than user_maximum with schedule(dynamic, 1)
 * In arg:    thread_count, user_maximum, marked[]
 */
void parallel_dynamic(int thread_count, int user_maximum, int* marked)
{
    int i;
    
#pragma omp parallel for num_threads(thread_count)  \
    shared (marked) schedule(dynamic, 1)
    
    //Starting at i = 1 because find_next_prime's first call will set it to 2
    for(i = 1; i < user_maximum; i++)
    {
        //Set i to the next unmarked number
        i = find_next_unmarked(i, user_maximum, marked);
        
        //If i is user_maximum, there are no unmarked spots left
        if(i != user_maximum)
        {
            //Mark all multiples of i
            mark_multiples(i, user_maximum, marked);
        }
    }
}

/*------------------------------------------------------------------
 * Function:  debug_main
 * Purpose:   same as main, but runs debugging tests
 * In arg:    thread_count, user_maximum
 */
int debug_main(int thread_count)
{
    double start, finish, static_time, dynamic_time; /* Timing variables */
    int i, j, user_maximum;
    int* marked; //Will eventually by an array of size user_maximum

    for(user_maximum = 10; user_maximum < 10000001; user_maximum *= 10)
    {
        printf("n = %d:\n", user_maximum);
    
        for(j = 0; j < 10; j++)
        {
           marked = malloc(user_maximum * sizeof(int));
           if(marked == NULL)
           {
                printf("Could not allocate int array[%d]. Exiting.\n", 
                        user_maximum);
                return 1;
           }
           
           //Initiallize marked array to all unmarked
           for(i = 0; i < user_maximum; i++)
           {
                marked[i] = 0;
           }
           
           //Find primes with static
           start = omp_get_wtime();
           //Run with schedule(static, 1)
           parallel_static(thread_count, user_maximum, marked);
           finish = omp_get_wtime();
           static_time = finish - start;
           
           //Re-initiallize marked array to all unmarked
           for(i = 0; i < user_maximum; i++)
           {
                marked[i] = 0;
           }
           
           //Find primes with dynamic
           start = omp_get_wtime();
           //Run with schedule(dynamic, 1)
           parallel_dynamic(thread_count, user_maximum, marked);
           finish = omp_get_wtime();
           dynamic_time = finish - start;
           
           start = dynamic_time - static_time;
           
           if(start < 0) //Static took longer
           {
                printf("Dynamic was faster by %e\n", (start * -1));
           }
           else //Dynamic took longer
           {
                printf("Static was faster by %e\n", start);
           }
           
           free(marked);
        }
        
            printf("\n");
    }
   
   return 0; 
}

int main(int argc, char *argv[]) 
{
   int thread_count = 8;     /* Number of processes */
   int user_maximum;        //Specified by the user at the command line
   double start, finish, static_time, dynamic_time; /* Timing variables */
   int* marked; //Will eventually by an array of size user_maximum
   int i;

    #ifdef DEBUG
        //Do debug_main instead
        if(debug_main(thread_count))
        {
            return 1; //If degun_main() encountered an error
        }
        else
        {
            return 0; //debug_main() ran smoothly
        }
    #endif
    
    
   if (argc != 2) Usage(argv[0]);
      user_maximum =  strtoll(argv[1], NULL, 10);
         
   marked = malloc(user_maximum * sizeof(int));
   if(marked == NULL)
   {
        printf("Could not allocate int array[%d]. Exiting.\n", 
                user_maximum);
        return 1;
   }
   
   //Initiallize marked array to all unmarked
   for(i = 0; i < user_maximum; i++)
   {
        marked[i] = 0;
   }
   
   //Find primes with static
   printf("schedule(static, 1):");
   start = omp_get_wtime();
   //Run with schedule(static, 1)
   parallel_static(thread_count, user_maximum, marked);
   finish = omp_get_wtime();
   static_time = finish - start;
   print_primes(user_maximum, marked);
   printf("\nElapsed time = %e\n\n", static_time);
   
   //Re-initiallize marked array to all unmarked
   for(i = 0; i < user_maximum; i++)
   {
        marked[i] = 0;
   }
   
   //Find primes with dynamic
   printf("schedule(dynamic, 1):");
   start = omp_get_wtime();
   //Run with schedule(dynamic, 1)
   parallel_dynamic(thread_count, user_maximum, marked);
   finish = omp_get_wtime();
   dynamic_time = finish - start;
   print_primes(user_maximum, marked);
   printf("\nElapsed time = %e\n\n", dynamic_time);
   
   start = dynamic_time - static_time;
   
   if(start < 0) //Static took longer
   {
        printf("Dynamic was faster by %e\n\n", (start * -1));
   }
   else //Dynamic took longer
   {
        printf("Static was faster by %e\n\n", start);
   }
   
   free(marked);
   return 0;
}

