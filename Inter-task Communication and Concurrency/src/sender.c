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

int main(int argc, char *argv[])
{
	//Checks the number of arguments
	if ( argc != 6 ) {
		printf("Invalid arguments for producer");
		exit(1);
	}

	//-----------------------------------------------
	int numberInt; //Number of integers to produce
	int qSize; //size of the mqueue
	int numberProd; //Number of producers
	int numberCons;	//Number of consumers
	int customID = 0; //id given by us, not by the operating system
	//-----------------------------------------------

	//------------------------------------------------
	//Parse user input
	numberInt = atoi(argv[1]);
	qSize = atoi(argv[2]);
	numberProd = atoi(argv[3]);
	numberCons = atoi(argv[4]);
	customID = atoi(argv[5]);
	//------------------------------------------------

	printf("sender spawned with id %d, queue size %d \n", customID, qSize);

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

	//------------------------------------------------
	//open the queue based on the mode and attributes and name
	// O_CREAT if the queue doesn't exist, create it
	qdes  = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
	if (qdes == -1 ) {
		perror("mq_open() failed \n");
		exit(1);
	}
	//------------------------------------------------

	//------------------------------------------------
	//PRODUCER LOGIC
	//Each producer child executes this
	int numberToProduce;
	for (numberToProduce = 0; numberToProduce < numberInt; numberToProduce++)
	{
		if (numberToProduce % numberProd == customID) {
			//printf("producer %d found number to produce %d \n", customID, numberToProduce);
			//YAY this is the number I'm supposed to produce, send it to the queue
			//mq_send() adds the message pointed to by msg_ptr to the message queue referred to by the descriptor mqdes. The msg_len argument specifies the length of the message pointed to by msg_ptr; this length must be less than or equal to the queue's mq_msgsize attribute. Zero-length messages are allowed.
			//If the message queue is already full (i.e., the number of messages on the queue equals the queue's mq_maxmsg attribute), then, by default, mq_send() BLOCKS until sufficient space becomes available to allow the message to be queued, or until the call is interrupted by a signal handler. If the O_NONBLOCK flag is enabled for the message queue description, then the call instead fails immediately with the error.
			//If full, BLOCK this process
			int numberToSend = numberToProduce;
			//------------------------------------------------
			//Debugging logic, print status of the queue
			//------------------------------------------------
			printf("will send %d from customID: %d \n", numberToProduce, customID);
			if (mq_send(qdes, (char *) &numberToSend, sizeof(int), 0) == -1) {
				printf("send failed customID: %d \n", customID);
			}
		}

	}
	//------------------------------------------------

	//close the queue
	if (mq_close(qdes) == -1) {
		perror("mq_close() failed \n");
		exit(2);
	}

	return 0;
}
