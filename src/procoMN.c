#include "proco.h"

int main (int argc, char *argv[])
{
	if(argc != 6){
		printf("\nERROR: the program should take 5 arguments!\n\n");
		exit(-1);
	}
	
	int producers = atoi(argv[1]);
	int consumers = atoi(argv[2]);
	int numOfRands = atoi(argv[3]);
	int circSize = atoi(argv[4]);
	seed = atoi(argv[5]);
	
	fp = fopen("./producers_in.txt", "w");
	fc = fopen("./consumers_out.txt", "w");
		
	working_prods = producers;
	w = producers;

	total_nums = producers*numOfRands;
	n = producers*numOfRands;	


	cb = (circular_buffer*)malloc(sizeof (struct circular_buffer));	
	cb_init(cb, circSize, 1);

	init();

	PRODUCERS prods[producers]; // Initialize array of PRODUCERS

	int rc;

	int i;
	int pID[producers]; // Producers ID
	pthread_t *p; 


	p = malloc(producers * sizeof(pthread_t));
	if (p == NULL) {
		printf("NOT ENOUGH MEMORY!\n");
		exit(-1);
	}
	
	// Create producer threads
	for(i = 0; i < producers; i++){
		prods[i].randoms = numOfRands; // Number of randoms
		pID[i] = i + 1;
		prods[i].threadID = pID[i]; // ID

		rc = pthread_create (&p[i], NULL, execProds, &prods[i]);
		if(rc != 0){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}
	
	int cID[consumers]; // Consumers ID
	pthread_t *c;

	c = malloc(consumers * sizeof(pthread_t));
	if (c == NULL) {
		printf("NOT ENOUGH MEMORY!\n");
		exit(-1);
	}

	// Create consumer threads
	for(i = 0; i < consumers; i++){
		cID[i] = i + 1;		
		rc = pthread_create (&c[i], NULL, execCons, &cID[i]);
		if(rc != 0){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}
	
	// Join producer threads
	for(i = 0; i < producers; i++){
		rc = pthread_join (p[i], NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
		
	}

	// Join consumer threads
	for(i = 0; i < consumers; i++){
		rc = pthread_join (c[i], NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
		
	}


	fclose(fp); // Close fp file
	fclose(fc); // Close fc file

	free(p);
	free(c);
	free(cb);

	destroy();

	printf("\n\n");

	return 1;
}
