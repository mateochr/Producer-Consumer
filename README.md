# Producer-Consumer
In this implementation one or more thread-producers generate random numbers and insert them at the tail of the circular buffer. Meanwhile one more thread-consumers extract the random numbers from the other end of the circular buffer. **Mutual exclusion** exists, meaning there is no inserting or/and exctracting from the same position of the circular buffer at the same time. The producers are prevented while the queue is full and the consumers while its empty (**Conditional waiting**). Each producer and consumer have an id given to them by the basic thread while creating them, the id is a serial number starting from 1.

**The output of the program are two txt files**

producers_in.txt: The producers record a line (Producer id: -27) for every number they push in the queue.

consumers_out.txt: The consumers record a line (Consumer id: -27) for every number they extract.

Additionally every consumer and producer print the number they inserted/removed.


## HowTo
The input must be five variables or the program will not run (e.g. 3 4 24 15 3). 3 producers, 4 consumers, 24 is the quantity of the random numbers, 15 is capacity of the circular buffer and 3 wokrs as a seed for the random number generator.

**N producers and M consumers**

`gcc proco2.c proco-common.c -o proco`

`proco 4 2 20 10 1`
