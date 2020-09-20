# Producer-Consumer
Producer-Consumer system that uses POSIX threads. In this implementation one or more thread-producers generate random numbers
and insert them at the tail of the circular buffer. Meanwhile one more thread-consumers extract the random numbers from the other end of the circular buffer.
**Mutual exclusion** exists, meaning there is no inserting or/and exctracting from the same position of the circular buffer. Also both producers and consumers
are prevented from adding/removing in/from the circular buffer while it's full/empty (**Conditional waiting**).

The input must be five variables or the program will not run (e.g. 3 4 24 15 3). 3 producers, 4 consumers, 24 is the quantity of the random numbers, 15 is capacity of the circular buffer and 3 wokrs as a seed for the random number generator.

## HowTo



**First case of 1 producer and 1 consumer**

`gcc proco1.c proco-common.c -o proco`

`proco 1 1 20 10 1`

**Second case of N producers and M consumers**

`gcc proco2.c proco-common.c -o proco`

`proco 4 2 20 10 1`
