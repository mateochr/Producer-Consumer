// The header file

#include "stdio.h" 
#include "sys/types.h" 
#include "stdlib.h" 
#include "string.h" 
#include "pthread.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/wait.h"

// Struct of circular_buffer
typedef struct circular_buffer{ 
	void *buffer; // data buffer 
	void *buffer_end; // end of data buffer 
	size_t capacity;  // maximum number of items in the buffer		
	size_t count; // number of items in the buffer 
	size_t sz; // size of each item in the buffer 
	void *head; // pointer to head 
	void *tail; // pointer to tail 
} circular_buffer;

// Struct of PRODUCERS
typedef struct prodc{
	int threadID; // ID of each producer thread
	int randoms; // Numbers that each producer will produce	
}PRODUCERS;

unsigned int seed; // the seed

circular_buffer *cb; //buffer

pthread_mutex_t mutex; // Mutex to lock buffer
pthread_cond_t notEmpty; // Buffer not empty
pthread_cond_t notFull; // Buffer not full

pthread_mutex_t prod_stdout; // Mutex to print stdout
pthread_cond_t prod_end; // A producer is done printing stdout
pthread_cond_t prods_done; // All producers done printing stdout

pthread_mutex_t con_stdout; // Mutex to print stdout
pthread_cond_t con_end; // A consumer is done printing stdout

FILE *fp; // Producers write in this file
FILE *fc; // Consumers write in this file

int working_prods; // Number of producers still working (decrease 1 after each producer is done)
int total_nums; // Number of total numbers to produce (number of producers*number of random numbers, decrease 1 after we produce a number)

int w; // Number of producers
int n; // Number of total numbers to produce (number of producers*number of random numbers)


void cb_init(circular_buffer *cb, size_t capacity, size_t sz);
void cb_free(circular_buffer *cb);
void cb_push_back(circular_buffer *cb, const void *item);
void cb_pop_front(circular_buffer *cb, void *item);
int cbEmpty(circular_buffer *cb);
int cbFull(circular_buffer *cb);

void *execProds(void *args);

void *execCons(void *args);

void init(); // Initialize mutexes and condition variables

void destroy(); // Destroy mutexes and condition variables
