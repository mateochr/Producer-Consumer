# Producer-Consumer
In this implementation one or more thread-producers generate random numbers and insert them at the tail of the circular buffer. Meanwhile one or more thread-consumers extract the random numbers from the other end of the circular buffer. **Mutual exclusion** exists, meaning there is no inserting or/and exctracting from the same position of the circular buffer at the same time. The producers are prevented while the queue is full and the consumers while its empty (**Conditional waiting**). Each producer and consumer have an id given to them by the basic thread while creating them, the id is a serial number starting from 1.

**The output of the program are two txt files**

producers_in.txt: The producers record a line (Producer id: -27) for every number they push in the queue.

consumers_out.txt: The consumers record a line (Consumer id: -27) for every number they extract.

Additionally every consumer and producer print the number they inserted/removed.

### proco.h

Header file with global variables and declarations of functions.

### procoMN.c

The main function that checks if the input is valid and creates the producer/consumer threads.

### proco-common.c

Definition of functions that ensure the proper operation of the circular buffer, along with lock mutexes and conditionals to provide thread synchronization.

## HowTo
The input must be five variables or the program will not run (e.g. 3 4 24 15 3). 3 producers, 4 consumers, 24 is the quantity of the random numbers, 15 is capacity of the circular buffer and 3 wokrs as a seed for the random number generator.
Open a cmd in the folder that contains the files and type the commands below.
**N producers and M consumers**

`gcc procoMN.c proco-common.c -o proco`

`proco 4 2 20 10 1`
