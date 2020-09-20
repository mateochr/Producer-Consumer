#include "proco.h"

// Initializes circular buffer
void cb_init(circular_buffer *cb, size_t capacity, size_t sz)
{
    cb->buffer = malloc(capacity * sz);
    if(cb->buffer == NULL){
		printf("Could not allocate memory..Exiting! \n");
		exit(1);
		}
    // Handles error
    cb->buffer_end = (char *)cb->buffer + capacity * sz;
    cb->capacity = capacity;
    cb->count = 0;
    cb->sz = sz;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
}

// Destroys circular buffer
void cb_free(circular_buffer *cb)
{
    free(cb->buffer);
    // Clears out other fields too, just to be safe
}

// Adds item to circular buffer
void cb_push_back(circular_buffer *cb, const void *item)
{
    if(cb->count == cb->capacity)
        {
		printf("Access violation. Buffer is full\n");
		exit(1);
	}
    memcpy(cb->head, item, cb->sz);
    cb->head = (char*)cb->head + cb->sz;
    if(cb->head == cb->buffer_end)
    	cb->head = cb->buffer;
    cb->count++;
}

// Removes first item from circular item
void cb_pop_front(circular_buffer *cb, void *item)
{
    if(cb->count == 0)
        {
			printf("Access violation. Buffer is empty\n");
			exit(1);
	}
    memcpy(item, cb->tail, cb->sz);
    cb->tail = (char*)cb->tail + cb->sz;
    if(cb->tail == cb->buffer_end)
    cb->tail = cb->buffer;
    cb->count--;
}

// Checks if buffer is full, returns 1 if full
int cbFull(circular_buffer *cb){

	if(cb->count == cb-> capacity){
		return 1;
	}
	else{
		return 0;
	}
}

// Checks if buffer is empty, returns 1 if empty
int cbEmpty(circular_buffer *cb){

	if(cb->count == 0){
		return 1;
	}
	else{
		return 0;
	}
}

// Contains the produced number and the ID of the producer that produced it
typedef struct pr{
	int ID;
	char num;	
}PROD;

int p_done = 0; // 1 if all producers are done printing stdout

int p_asc = 1; // Used so the producers print in ascending order

int p_counter = 0;

void *execProds(void *args) {
	PRODUCERS *prods;
	prods = (PRODUCERS *) args;
	int *id;
	id = (int*) prods->threadID;
	int i;

	PROD p_id_n[n]; // Array of all produced numbers and their producer
	
	for(i = 0; i < prods->randoms; i++){
		
		pthread_mutex_lock(&mutex);
		
		// While buffer full wait
		while(cbFull(cb) == 1){
			pthread_cond_wait (&notFull, &mutex);		
		}
		// Produces a random number and pushes it in buffer
		char number = rand_r(&seed);						
		cb_push_back(cb, (void*)&number);
		
		total_nums--; // Decreases total numbers produced by 1
		
		// Saves ID and produced number in the p_id_n array
		p_id_n[p_counter].num = number;
		p_id_n[p_counter].ID = prods->threadID;
		p_counter++;

		fprintf(fp, "Producer %d: %d \n", prods->threadID,number); // Print in fp file

		// If the producer produces his last number, he decreases working_prods by 1
		if(i+1 == prods->randoms){
			--working_prods;
		}
		pthread_mutex_unlock (&mutex);
		pthread_cond_broadcast(&notEmpty);

		usleep (100000); // Sleeps to randomize the process					
	} 
	
	
	pthread_mutex_lock(&prod_stdout);
	
	// While not the wanted producer's turn, wait
	while(p_asc != prods->threadID){
		pthread_cond_wait(&prod_end, &prod_stdout);
	}	
	
	printf("\nProducer %d: ", p_asc);

	// Searches the numbers in p_id_n array, which have been produced by the producer with ID = p_asc
	for(i = 0; i < n; i++){
		if(p_asc == p_id_n[i].ID){
			printf("%d  ",p_id_n[i].num);			
		}
	}

	// The last producer wakes up the waiting consumers
	if(p_asc == w){
		p_done = 1;		
		printf("\n");
		pthread_cond_broadcast(&prods_done);
	}

	p_asc++;

	pthread_cond_broadcast(&prod_end); // Wakes up producers so the next one can print
	
	pthread_mutex_unlock (&prod_stdout);

	pthread_exit(id);
}



// Contains the consumed number and the ID of the consumer that consumed it
typedef struct co{
	int ID;
	char num;	
}CON;

int c_asc = 1;
int c_counter = 0;

void *execCons(void *args) {
	int flag = 0; // flag = 1 if all producers have stopped and all numbers have been produced
	int *id;
	id = (int*) args;
	CON c_id_n[n];
	
	do{		
		pthread_mutex_lock (&mutex);
			
		// While buffer is empty
		while(cbEmpty(cb) == 1){
			// If all producers have stopped and all numbers have been produced
			// stop the thread and unlock the mutex
			if(total_nums <= 0 && working_prods <= 0){
				pthread_mutex_unlock (&mutex);	
				flag = 1;
				break;
			}
						
			pthread_cond_wait(&notEmpty, &mutex);	
		}
		// Stop while
		if(flag == 1){
			break;
		}
		
		// Reads from buffer 		
		char *tmp_read = (char*) malloc(1);
		cb_pop_front(cb, (void*)tmp_read);

		// Saves ID and consumed number in the c_id_n array
		c_id_n[c_counter].num = *tmp_read;
		c_id_n[c_counter].ID = *id;
		c_counter++;
		
		// Writes in file and free tmp_read	
		fprintf(fc, "Consumer %d: %d \n", *id, *tmp_read);
		free(tmp_read); 
			
		pthread_mutex_unlock (&mutex);
		pthread_cond_broadcast(&notFull);
		
		usleep(200000); // Sleeps to randomize the process	
			
	}while(working_prods != 0 || cb->count > 0); // While there are producers producing or the buffer is not empty
	
	
	pthread_mutex_lock(&con_stdout);
	int i;
	
	// While producers have not finished printing
	while(p_done == 0){	
		pthread_cond_wait(&prods_done, &con_stdout);
	}
	
	// While not the wanted consumer's turn wait
	while(c_asc != *id){
		pthread_cond_wait(&con_end, &con_stdout);
	}
	
	int consumed = 0; // consumed = 1 if the consumer has consumed at least one number 
	printf("\nConsumer %d: ", c_asc);
	// Searches the numbers in c_id_n array, which have been consumed by the consumer with ID = c_asc
	for(i = 0; i < n; i++){	
		if(c_asc == c_id_n[i].ID){
			printf("%d  ",c_id_n[i].num);
			consumed = 1;			
		}
	}

	if (consumed == 0) {
		printf("did not consume any number");
	}
	
	c_asc++;
	
	pthread_cond_broadcast(&con_end); // Wakes up consumers so the next one can print
	
	pthread_mutex_unlock (&con_stdout);

	pthread_exit(id);	
}


void init(){

	int rc;

	// Initialize mutex
	rc = pthread_mutex_init(&mutex, NULL);
	if(rc != 0){
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc); 
		exit(-1);
	}

	// Initialize prod_stdout
	rc = pthread_mutex_init(&prod_stdout, NULL);
	if(rc != 0){
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc); 
		exit(-1);
	}

	// Initialize con_stdout
	rc = pthread_mutex_init(&con_stdout, NULL);
	if(rc != 0){
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc); 
		exit(-1);
	}

	// Initialize notEmpty condition variable 
	rc = pthread_cond_init(&notEmpty, NULL);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_init() is %d\n", rc); 
		exit(-1); 
	}

	// Initialize notFull condition variable
	rc = pthread_cond_init(&notFull, NULL);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_init() is %d\n", rc); 
		exit(-1); 
	}

	// Initialize prods_done condition variable 
	rc = pthread_cond_init(&prods_done, NULL);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_init() is %d\n", rc); 
		exit(-1); 
	}

	// Initialize prod_end condition variable 
	rc = pthread_cond_init(&prod_end, NULL);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_init() is %d\n", rc); 
		exit(-1); 
	}

	// Initialize con_end condition variable 
	rc = pthread_cond_init(&con_end, NULL);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_init() is %d\n", rc); 
		exit(-1); 
	}
}

void destroy(){

	int rc;

	// Destroy mutex
	rc = pthread_mutex_destroy(&mutex);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc); 
		exit(-1); 
	}

	// Destroy prod_stdout
	rc = pthread_mutex_destroy(&prod_stdout);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc); 
		exit(-1); 
	}

	// Destroy con_stdout
	rc = pthread_mutex_destroy(&con_stdout);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc); 
		exit(-1); 
	}

	// Destroy notEmpty condition variable
	rc = pthread_cond_destroy(&notEmpty);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1); 
	}

	// Destroy notFull condition variable
	rc = pthread_cond_destroy(&notFull);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1); 
	}

	// Destroy prods_done condition variable
	rc = pthread_cond_destroy(&prods_done);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1); 
	}

	// Destroy prod_end condition variable
	rc = pthread_cond_destroy(&prod_end);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1); 
	}

	// Destroy con_end condition variable
	rc = pthread_cond_destroy(&con_end);
	if (rc != 0){ 
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1); 
	}
}
