#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <errno.h>

//Queue implementation inspired by http://www.cs.uah.edu/~rcoleman/Common/CodeVault/Code/Code130_Queue.html

//-----------------------------------------------
//Global variables

//Arguments from command line
int numberInt; //Number of integers to produce
int qSize; //size of the mqueue
int numberProd; //Number of producers
int numberCons;	//Number of consumers

typedef struct {
	int *buffer;
	int head;
	int tail;
} Buffer;

//Critical variables
Buffer queue;
//-----------------------------------------------
//Semaphores and Mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t spaces;
sem_t items;
sem_t consumptionCount;
//-----------------------------------------------


//-----------------------------------------------

void *producer(void *threadArg) {

	int customID = *(int*)threadArg;

	int numberToProduce;
	for (numberToProduce = 0; numberToProduce < numberInt; numberToProduce++) {
		if (numberToProduce % numberProd == customID) {
			//printf("producer %d found number to produce %d \n", customID, numberToProduce);
			//YAY this is the number I'm supposed to produce, send it to the queue
			//If full, BLOCK this thread
			int numberToSend = numberToProduce;
			//------------------------------------------------
			//Debugging logic, print status of the queue
			//------------------------------------------------

			sem_wait(&spaces);
			pthread_mutex_lock(&lock);

			//enqueue
			//if empty, set head and tail to zero
			if ((queue.head == -1) && (queue.tail == -1)) {
				queue.head = 0;
				queue.tail = 0;
				//printf("producer %d set head,tail =0 \n",customID);
			}
			else {
				queue.tail = (queue.tail + 1) % qSize;
				//printf("producer %d increased tail %d \n",customID, queue.tail);
			}

			queue.buffer[queue.tail] = numberToSend;

			//printf("producer %d sent %d \n", customID, numberToSend);

			pthread_mutex_unlock(&lock);
			sem_post(&items);
		}
	}

	pthread_exit(0);
}


void *consumer(void *threadArg) {
	int customID = *(int *) threadArg;
	//int customID = (int) threadArg;

	//printf("receiver %d will enter while loop \n", customID);
	while (1) {
		//If consumption count is 1, then we are done consuming all the numbers and this fails
		if (sem_trywait(&consumptionCount) == -1) {
			break;
		}

		sem_wait(&items);
		pthread_mutex_lock(&lock);

		//----------------------------------------------------
		int numberTakenFromBuffer;
		//printf("receiver %d taking from head %d \n",customID,queue.head);
		numberTakenFromBuffer = queue.buffer[queue.head];
		queue.head = (queue.head + 1) % qSize;

		//printf("receiver customID: %d, received %d \n", customID, numberTakenFromBuffer);

		//Check if number is perfect square
		int squareRootOfNumberTakenFromBuffer = sqrt(numberTakenFromBuffer);
		if ((squareRootOfNumberTakenFromBuffer * squareRootOfNumberTakenFromBuffer) == numberTakenFromBuffer) {
			//number is a perfect square
			//printf("perfect square %d found by %d sqrt: %d \n", numberTakenFromBuffer, customID, squareRootOfNumberTakenFromBuffer);
			printf("%d %d %d \n",customID,numberTakenFromBuffer,squareRootOfNumberTakenFromBuffer);
		}
		//----------------------------------------------------

		pthread_mutex_unlock(&lock);
		sem_post(&spaces);
	}

	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	//Checks the number of arguments
	if ( argc != 5 ) {
		printf("Usage: ./producer N B P C");
		printf("The qname must start with a \"/\". \n");
		exit(1);
	}

	//-----------------------------------------------
	//variables for timing
	struct timeval tv;
	double t1;
	double t2;
	//-----------------------------------------------

	//-----------------------------------------------
	//Parse user input
	numberInt = atoi(argv[1]);
	qSize = atoi(argv[2]);
	numberProd = atoi(argv[3]);
	numberCons = atoi(argv[4]);
	//------------------------------------------------

	//------------------------------------------------
	//Error checking, we don't want stupid inputs
	if ((numberInt <= 0) || (qSize <= 0) || (numberProd < 1) || (numberCons < 1)) {
		printf("invalid arguments, don't make sense \n");
		exit(1);
	}
	else {
		//printf("Numbers to produce %d \n",numberInt);
	}
	//------------------------------------------------

	//Array holding references to threads to join them
	pthread_t producerThreads[numberProd];
	pthread_t consumerThreads[numberCons];

	//------------------------------------------------
	//Initialize queue
	queue.buffer = (int *) malloc(sizeof(int) * qSize);
	queue.head = -1;
	queue.tail = -1;
	//------------------------------------------------


	//------------------------------------------------
	//Initialize semaphores
	sem_init(&spaces, 0, qSize);
	sem_init(&items, 0, 0);
	sem_init(&consumptionCount, 0, numberInt);
	//------------------------------------------------

	//------------------------------------------------
	//Timing
	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + tv.tv_usec/1000000.0;
	//------------------------------------------------

	//------------------------------------------------
	//create as many producer and consumer threads as we need
	int i;
	for (i = 0; i < numberProd; i++) {
		int threadID = i;
		int *customID = (int *) malloc(sizeof(int));
		*customID = i;
		pthread_create(&producerThreads[i], NULL, &producer, (void *)customID);

	}

	int j;
	for (j = 0; j < numberCons; j++) {
		int threadID = j;
		//pthread_create(&consumerThreads[j], NULL, &consumer, (void *)threadID);
		int *customID = (int *) malloc(sizeof(int));
		*customID = j;
		pthread_create(&consumerThreads[j], NULL, &consumer, (void *)customID);
	}
	//------------------------------------------------


	//------------------------------------------------
	//join producer and consumer threads
	int w1;
	for (w1 = 0; w1 < numberProd; w1++) {
		pthread_join(producerThreads[w1], NULL);
	}

	int w2;
	for (w2 = 0; w2 < numberCons; w2++) {
		pthread_join(consumerThreads[w2], NULL);
	}
	//------------------------------------------------

	//------------------------------------------------
	//Timing
	gettimeofday(&tv, NULL);
	t2 = tv.tv_sec + tv.tv_usec/1000000.0;
	//------------------------------------------------

	printf("System execution time: %f seconds\n", t2-t1);

	//------------------------------------------------
	//Cleanup
	if ((sem_destroy(&spaces) && sem_destroy(&items) && sem_destroy(&consumptionCount)) == 0) {
		//printf("semaphores destroyed \n");
	}
	else {
		//perror("semaphore destroy failed \n");
	}
	//------------------------------------------------

	return 0;
}
