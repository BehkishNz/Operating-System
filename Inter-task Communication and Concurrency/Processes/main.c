#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include "common.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

// the following code is the modified version of the code given in Lecture 6 and page 51 of the textbook given in the lab manual
//it will spawn a child.
// if the id<0 it's fail, if ==0 it's child process and if >0
int spawn (char* program, char** arg_list)
{
	pid_t child_pid;

	/* Duplicate this process.  */
	child_pid = fork ();
	if(child_pid < 0)
	{
		fprintf(stderr, "Fork Failed \n");
		return -1;
	}
	else  if (child_pid > 0)
	{ /* This is the parent process.  */
		return child_pid;
	}
	else {
		/* Now execute PROGRAM, searching for it in the path.  */
		execvp (program, arg_list);
		printf("Execvp error %s \n", strerror(errno));
		/* The execvp function returns only if an error occurs.  */
		fprintf (stderr, "an error occurred in execvp \n");
		abort ();
	}
}

int main(int argc, char *argv[])
{
	//Checks the number of arguments
	//we need 5 arguments
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
	double t3;

	int child_status;

	//-----------------------------------------------
	int numberInt; //Number of integers to produce
	int qSize; //size of the mqueue
	int numberProd; //Number of producers
	int numberCons;	//Number of consumers
	//-----------------------------------------------

	//------------------------------------------------
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

	/*EACH PRODUCER AND CONSUMER WILL OPEN THE QUEUE ON ITS OWN */
	//------------------------------------------------
	//QUEUE setup
	mqd_t qdes;
	/* queue name must start with '/'. man mq_overview */
	char* qname = "/mailbox1_mflaifel";
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;
	attr.mq_maxmsg  = qSize;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags   = 0;		/* a blocking queue  */
	//------------------------------------------------

//	qdes = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
//	if (qdes == -1) {
//		perror("main queue open failed \n");
//		exit(1);
//	}

	//------------------------------------------------
	//CONSUMER QUEUE used for consumers to terminate
	mqd_t consumerQueueDesc;
	/* queue name must start with '/'. man mq_overview */
	char* consumerQueueName = "/consumerQueue";
	mode_t consumerQueueMode = S_IRUSR | S_IWUSR;
	struct mq_attr consumerQueueAttr;
	consumerQueueAttr.mq_maxmsg  = 1;
	consumerQueueAttr.mq_msgsize = sizeof(int);
	consumerQueueAttr.mq_flags   = 0;		/* a blocking queue  */
	//------------------------------------------------

	//------------------------------------------------
	//Delete the queues at the beginning to clean up from previous exec
	//mq_unlink() removes the specified message queue name. The message queue name is removed immediately. The queue itself is destroyed once any other processes that have the queue open close their descriptors referring to the queue.
//	if (mq_unlink(qname) != 0) {
//		perror("mq_unlink() failed \n");
//	}
//	if (mq_unlink(consumerQueueName) != 0) {
//		perror("mq_unlick main consumerQueue \n");
//	}
	//------------------------------------------------

	//------------------------------------------------
	//CONSUMER QUEUE CREATION
	consumerQueueDesc = mq_open(consumerQueueName, O_RDWR | O_CREAT, consumerQueueMode, &consumerQueueAttr);
	if (consumerQueueDesc == -1) {
		perror("consumerQueue open failed \n");
		exit(1);
	}

	//Write initial value to consumer queue
	int consumerQueueInitialValue = numberInt;
	if (mq_send(consumerQueueDesc, (char *) &consumerQueueInitialValue, sizeof(int), 0) == -1) {
		perror("main consumerQueue send() \n");
	}

	//Close the consumer queue
	if (mq_close(consumerQueueDesc) == -1) {
		perror("main consumerQueue close() \n");
	}
	//------------------------------------------------


	pid_t producerPIDs[numberProd]; //Array holding pids of producers to wait on them
	pid_t consumerPIDs[numberCons];


	//------------------------------------------------
	//Timing
	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + tv.tv_usec/1000000.0;
	//------------------------------------------------

	//------------------------------------------------
	//fork as many producers as we need
	int i;
	for (i = 0; i < numberProd; i++) {
		//spawn consumer child
		char customIDString[100];
		sprintf(customIDString, "%d", i);
		//Argument list to be passed to the receiver
		char* arg_list[] = {
			"./sender.out",
			argv[1],
			argv[2],
			argv[3],
			argv[4],
			customIDString,
			NULL
		};
		producerPIDs[i] = spawn("./sender.out", arg_list);
	}

	//------------------------------------------------
	//fork as many consumers as we need
	int j;
	for (j=0; j < numberCons; j++) {
		//spawn receiver as child of sender
		char customIDString[100];
		sprintf(customIDString, "%d", j);
		//Argument list to be passed to receiver
		char* arg_list[] = {
			"./receiver.out",
			argv[1],
			argv[2],
			argv[3],
			argv[4],
			customIDString, //custom id converted to string
			NULL      /* The argument list must end with a NULL.  */
		};
		consumerPIDs[j] = spawn("./receiver.out", arg_list);
	}
	//------------------------------------------------

	//------------------------------------------------
	//Parent producer should wait on all the producers
	int wProducer;
	for (wProducer = 0; wProducer < numberProd; wProducer++) {
		waitpid(producerPIDs[wProducer], &child_status, 0);
		//if (WIFEXITED (child_status))
			//printf ("the child process exited normally, with exit code %d \n", WEXITSTATUS (child_status));
		//else
			//printf("the child process exited abnormally\n");
	}

	//printf("Producers finished \n");
	//------------------------------------------------

	//------------------------------------------------
	//Parent producer should wait on all the consumers
	int wConsumer;
	for (wConsumer = 0; wConsumer < numberCons; wConsumer++) {
		waitpid(consumerPIDs[wConsumer], &child_status, 0);
		//if (WIFEXITED (child_status))
			//printf ("the child process exited normally, with exit code %d \n", WEXITSTATUS (child_status));
		//else
			//printf("the child process exited abnormally\n");
	}

	//printf("Consumers finished \n");
	//------------------------------------------------

	//------------------------------------------------
	//Timing
	gettimeofday(&tv, NULL);
	t2 = tv.tv_sec + tv.tv_usec/1000000.0;
	//------------------------------------------------

	//close queues
//	if (mq_close(qdes) == -1) {
//		perror("mq_close() failed in main \n");
//		exit(2);
//	}
//	if (mq_close(qdes) == -1) {
//		perror("mq_close() failed in main \n");
//		exit(2);
//	}

	//mq_unlink() removes the specified message queue name. The message queue name is removed immediately. The queue itself is destroyed once any other processes that have the queue open close their descriptors referring to the queue.
	if (mq_unlink(qname) != 0) {
		perror("mq_unlink() failed \n");
		exit(3);
	}
	else {
		//printf("main queue deleted \n");
	}

	if (mq_unlink(consumerQueueName) != 0) {
		perror("mq_unlick main consumerQueue \n");
	}
	else {
		//printf("main consumerQueue deleted \n");
	}

	printf("System execution time: %f seconds \n", (t2-t1));

	return 0;
}
