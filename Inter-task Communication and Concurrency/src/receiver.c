#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <signal.h>
#include "common.h"

int main(int argc, char *argv[])
{
	//checks the number of arguments
	if ( argc != 6 ) {
		printf("invalid receiver arguments, rec: %d \n", argc);
		exit(1);
	}

	//-----------------------------------------------
	int numberInt;
	int qSize;
	int numberProd;
	int numberCons;
	int customID = 0;
	//-----------------------------------------------

	//------------------------------------------------
	//Parse user input
	numberInt = atoi(argv[1]);
	qSize = atoi(argv[2]);
	numberProd = atoi(argv[3]);
	numberCons = atoi(argv[4]);
	customID = atoi(argv[5]);
	//------------------------------------------------

	//printf("receiver spawned with id %d \n", customID);

	//------------------------------------------------
	//QUEUE setup
	mqd_t qdes;
	char  *qname = "/mailbox1_mflaifel"; //name of the queue
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr; //queue attributes
	attr.mq_maxmsg  = qSize; //set max num of messages
	attr.mq_msgsize = sizeof(int); //max message size
	attr.mq_flags   = 0;	/* a blocking queue  */
	//------------------------------------------------

	//------------------------------------------------
	//CONSUMER QUEUE used for consumers to terminate
	mqd_t consumerQueueDesc;
	char* consumerQueueName = "/consumerQueue";
	mode_t consumerQueueMode = S_IRUSR | S_IWUSR;
	struct mq_attr consumerQueueAttr;
	consumerQueueAttr.mq_maxmsg  = 1;
	consumerQueueAttr.mq_msgsize = sizeof(int);
	consumerQueueAttr.mq_flags   = 0;		/* a blocking queue  */
	//------------------------------------------------

	//------------------------------------------------
	//open the queue based on the mode and attributes and name
	qdes  = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
	if (qdes == -1 ) {
		//If all the producers have run, and they've set some flag then  exit normally
		perror("mq_open() \n");
		exit(1);
	}
	//------------------------------------------------

	//------------------------------------------------
	//open the consumer queue
	consumerQueueDesc  = mq_open(consumerQueueName, O_RDWR | O_CREAT, consumerQueueMode, &consumerQueueAttr);
	if (consumerQueueDesc == -1 ) {
		perror("consumerQueue open() \n");
		exit(1);
	}
	//------------------------------------------------

	while(1) {
		int numberTakenFromBuffer;
		int consumptionCount;

		printf("receiver %d trying to receive count \n",customID);
		//Check if we need to terminate
		if ((mq_receive(consumerQueueDesc, (char *) &consumptionCount, sizeof(int),0)) == -1) {
			perror("consumerQueue receive \n");
		}

		printf("consumptionCount is %d from consumer %d \n", consumptionCount,customID);
		if (consumptionCount <= 0) {
			//put consumptionCount back for other consumers, and we know that we should exit
			printf("consumer %d found consumptionCount <= 0 \n", customID);
			if ((mq_send(consumerQueueDesc, (char *) &consumptionCount, sizeof(int), 0) == -1)) {
				perror("consumerQueue send 2 \n");
			}
			break;
		}

		if ((mq_send(consumerQueueDesc, (char *) &consumptionCount, sizeof(int), 0) == -1)) {
			perror("consumerQueue send 2 \n");
		}

		/*Race condition was that two consumers would get past this point
		 both thinking that consumptionCount is not 0 (actual value is 1)
		 first one takes number from the buffer, but the second one hangs on mq_timedreceive because buffer is now empty
		 Solution: we send -1 in the main queue before breaking
		 first one that took the last number from buffer, will be able to loop again, this time seeing consumption count 0 and putting -1 in the buffer
		 second one that hung on timedreceive will now get a -1 in the buffer and continue, loop again, see 0 and break
		 */

		//This is passed to mq_timedreceive as an upper bound for time that process will remain blocked
		//struct timespec ts = {time(0) + 5, 0};
		struct timespec ts = {time(0) + 1, 0};
		//	only block for a limited time if the queue is empty

		//	mq_receive() REMOVES the oldest message with the highest priority from the message queue referred to by the descriptor mqdes, and places it in the buffer pointed to by msg_ptr.
		//	If queue is empty, then, by default, mq_receive() blocks until a message becomes available, or the call is interrupted by a signal handler. If the O_NONBLOCK flag is enabled for the message queue description, then the call instead fails immediately with the error EAGAIN.
		//
		//	IN THIS CASE, mq_timedreceive WILL BLOCK, because they didn't set the O_NONBLOCK flag.
		//	if return value is -1 that means read failed, returns bytes read
		//	mq_timedreceive() behaves just like mq_receive(), except that if the queue is empty and the O_NONBLOCK flag is not enabled for the message queue description, then abs_timeout points to a structure which specifies a ceiling on the time for which the call will block. If no message is available, and the timeout has already expired by the time of the call, mq_timedreceive() returns immediately.

		if (mq_timedreceive(qdes, (char *) &numberTakenFromBuffer, sizeof(int), 0, &ts) == -1) {
			perror("mq_timedreceive() failed \n");
		}
		else {
			//check if -1, if yes, continue
			if (numberTakenFromBuffer == -1) {
				//consumption finished
				printf("receiver %d found -1 \n",customID);
				break;
			}

			printf("receiver %d received %d \n",customID,numberTakenFromBuffer);

			int squareRootOfNumberTakenFromBuffer;
			squareRootOfNumberTakenFromBuffer = sqrt(numberTakenFromBuffer);
			//printf("receiver customID: %d, received %d \n", customID, numberTakenFromBuffer);
			if ((squareRootOfNumberTakenFromBuffer * squareRootOfNumberTakenFromBuffer) == numberTakenFromBuffer) {
				//number is a perfect square
				//printf("perfect square %d found by %d sqrt: %d \n", numberTakenFromBuffer, customID, squareRootOfNumberTakenFromBuffer);
				printf("%d %d %d \n",customID, numberTakenFromBuffer, squareRootOfNumberTakenFromBuffer);
			}

			//Decrement the consumption count and send it to the consumption queue
			if ((mq_receive(consumerQueueDesc, (char *) &consumptionCount, sizeof(int),0)) == -1) {
				perror("consumerQueue receive \n");
			}
			consumptionCount--;
			if (mq_send(consumerQueueDesc, (char *)&consumptionCount, sizeof(int),0) == -1) {
				perror("consumerQueue send");
			}
		}
	}


	//send dummy var -1 here
	int numberToSend = -1;
	if (mq_send(qdes, (char *) &numberToSend, sizeof(int), 0) == -1) {
		perror("mq_send -1 \n");
	}
	else {
		printf("consumer %d put -1 in buffer \n",customID);
	}

	printf("consumer will self-terminate %d \n", customID);

	//------------------------------------------------
	//Close the queue
	if (mq_close(qdes) == -1) {
		perror("mq_close() failed \n");
		exit(2);
	}

	//------------------------------------------------
	//Close the consumer queue
	if (mq_close(consumerQueueDesc) == -1) {
		perror("mq_close() failed \n");
		exit(2);
	}
	//------------------------------------------------
	
	return 0;
}
