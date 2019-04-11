/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 
*	    Student name : 
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm test code.
*
*/

//#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
//#include <asm/unistd.h>

#include "include/lab1_sched_types.h"

/*
 * you need to implement scheduler simlator test code.
 *
 */

// Queue implementation
struct Queue {
	int max, front, rear;
	int *data;
};

void qInit(struct Queue *q, int max) {
	q->max = max;
	q->front = q->rear = -1;
	q->data = malloc(sizeof(int) * max);
}

void qDestroy(struct Queue *q) {
	free(q->data);
}

void qPush(struct Queue *q, int param) {
	if (q->front == q->max - 1) {
		//double max
		q->max *= 2;
		int *newArray = malloc(sizeof(int) * q->max);
		for (int i = 0; i < q->max/2; i++) {
			newArray[i] = q->data[i];
		}
		free(q->data);
		q->data = newArray;
	}
	q->data[++(q->front)] = param;
	//printf("qPush front: %d\n",q->front);
	//printf("qPush rear: %d\n",q->rear);
}

int qPop(struct Queue *q) {
	if (q->front == q->rear) {
		printf("Warning! Empty queue exception! return -1.\n");
		return -1;
	}

	//printf("qPop front: %d\n",q->front);
	//printf("qPop rear: %d\n",q->rear);
	return q->data[++(q->rear)];
}

int qSize(struct Queue *q) {
	return q->front - q->rear;
}

void qPrint(struct Queue *q) {
	printf("[");
	for (int i = q->rear + 1; i <= q->front; i++)
		printf("%d ", q->data[i]);
	printf("]\n");
}

int main(int argc, char *argv[]){
	// queue test
	/*struct Queue q;
	qInit(&q, 3);
	qPush(&q, 10);
	qPush(&q, 20);
	qPush(&q, 30);
	qPush(&q, 40);
	qPush(&q, 50);
	qPush(&q, 60);
	qPrint(&q);
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));*/

	// input ---> int arr[][2] = {{1,2},{{process arrival time},{service time}}}
	int testData[5][2] = {{0, 3}, {2, 6}, {4, 4}, {6, 5}, {8, 2}};
	int mlfqResSize;
	int *mlfq = calcMLFQ(testData, 5, 2, &mlfqResSize);
	printf("result : ");
	for (int i = 0; i < mlfqResSize; i++)
		printf("%d ", mlfq[i]);
	printf("\n");
	return 0;
}

void printDev(int col, int leftServiceTimeArr[], struct Queue *l1, struct Queue *l2, struct Queue *l3, int procTime, int result[]) {
	printf("---------------------------------------------------------------------------\n");
	printf("left service time : ");
	for (int i = 0; i < col; i++) {
		printf("%d ", leftServiceTimeArr[i]);
	}
	printf("\n");
	printf("l1 : ");
	qPrint(l1);
	printf("l2 : ");
	qPrint(l2);
	printf("l3 : ");
	qPrint(l3);
	printf("result : ");
	for (int i = 0; i < procTime+1; i++) {
		switch (result[i]) {
			case 0:
				printf("A ");
				break;
			case 1:
				printf("B ");
				break;
			case 2:
				printf("C ");
				break;
			case 3:
				printf("D ");
				break;
			case 4:
				printf("E ");
				break;
			case -1:
				printf("? ");
		}
	}
	printf("\n");
}

int* calcMLFQ(int data[][2], int col, int timeQuantum, int *resSize) {
	if (col == 0) {
		printf("calcMLFQ -> Empty data!!!\n");
		int err[1] = {-1};
		return err;
	}
	//Initialize 3 queues for MLFQ.
	struct Queue l1; qInit(&l1, 100);	//highest priority
	struct Queue l2; qInit(&l2, 100);
	struct Queue l3; qInit(&l3, 100);	//lowest priority
	
	//Analyze input data.
	int lastArriveTime = 0;									//last arrive time between processes
	int firstArriveTime = data[0][0];						//first arrive time between processes
	int totalServiceTime = 0; 								//total process service time from first process arrival	
	int *leftServiceTimeArr = malloc(sizeof(int) * col);	//remaining service time temporary storage of processes
	for (int i = 0; i < col; i++) {
		if (data[i][0] > lastArriveTime)
			lastArriveTime = data[i][0];
		if (data[i][0] < firstArriveTime)
			firstArriveTime = data[i][0];
		totalServiceTime += data[i][1];
		leftServiceTimeArr[i] = data[i][1];
	}
	
	//Set empty result data.
	int *result = malloc(sizeof(int) * totalServiceTime);
	for (int i = 0; i < totalServiceTime; i++)
		result[i] = -1;
	
	//Main process roof start.
	int procTime = 0;				//CPU process timer
	int curProc;					//the process currently being processed by CPU
	int quantumTimer = timeQuantum;	//time quantum counter
	while (1) {
		/////
		//printf("===================================================================================================\nprocess timer : %d\n===================================================================================================\n", procTime);
		/////
		
		//Add new process to Queue1(highest priority) when there is a process currently in procTime.
		for (int i = 0; i < col; i++)
			if (data[i][0] == procTime)
				for (int j = 0; j < data[i][1]; j++)
					qPush(&l1, i);
		
		/////
		//printf("1. Add ");
		//printDev(col, leftServiceTimeArr, &l1, &l2, &l3, procTime, result);
		/////
		
		//When a new process is inserted, if there is a process that was previously running, drop it to the queue below.
		int latestProc = result[procTime-1];
		if (qSize(&l1) > 0) {
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) > leftServiceTimeArr[latestProc]) {
				for (int i = 0; i < leftServiceTimeArr[latestProc]; i++)
					qPush(&l2, qPop(&l1));
				quantumTimer = timeQuantum;
			}
		}
		else if (qSize(&l2) > 0) {
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) > leftServiceTimeArr[latestProc]) {
				for (int i = 0; i < leftServiceTimeArr[latestProc]; i++)
					qPush(&l3, qPop(&l2));
				quantumTimer = timeQuantum;
			}
		}
		
		/////
		//printf("2. Quantum 1 ");
		//printDev(col, leftServiceTimeArr, &l1, &l2, &l3, procTime, result);
		/////
		
		//Execute process in the highest priority queue. 
		if (qSize(&l1) > 0)
			curProc = qPop(&l1);
		else if (qSize(&l2) > 0)
			curProc = qPop(&l2);
		else
			curProc = qPop(&l3);
		result[procTime] = curProc;		//Push to result.
		leftServiceTimeArr[curProc]--;
		
		/////
		//printf("3. Process ");
		//printDev(col, leftServiceTimeArr, &l1, &l2, &l3, procTime, result);
		/////
		
		//When there are remaining jobs and there are no other processes scheduled in scheduler, drop it to the queue below.
		latestProc = result[procTime];
		if (qSize(&l1) > 0) {
			if (latestProc == l1.data[l1.rear+1])
				quantumTimer--;
			else
				quantumTimer = timeQuantum;
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) > leftServiceTimeArr[latestProc]) {
				for (int i = 0; i < leftServiceTimeArr[latestProc]; i++)
					qPush(&l2, qPop(&l1));
				quantumTimer = timeQuantum;
			}
		}
		else if (qSize(&l2) > 0) {
			if (latestProc == l2.data[l2.rear+1])
				quantumTimer--;
			else
				quantumTimer = timeQuantum;
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) > leftServiceTimeArr[latestProc]) {
				for (int i = 0; i < leftServiceTimeArr[latestProc]; i++)
					qPush(&l3, qPop(&l2));
				quantumTimer = timeQuantum;
			}
		}
		else { 
			if (latestProc == l3.data[l3.rear+1])
				quantumTimer--;
			else
				quantumTimer = timeQuantum;
			if (quantumTimer <= 0) {
				for (int i = 0; i < leftServiceTimeArr[latestProc]; i++)
					qPush(&l3, qPop(&l3));	//Push to itself because there is no queue below.
				quantumTimer = timeQuantum;
			}
		}
			
		/////
		//printf("4. Quantum ");
		//printDev(col, leftServiceTimeArr, &l1, &l2, &l3, procTime, result);
		/////
		
		procTime++;
		//Exit when procTime is appropriate or the queues are all empty
		if (procTime > lastArriveTime && qSize(&l1) == 0 && qSize(&l2) == 0 && qSize(&l3) == 0)
			break;
	}
	*resSize = totalServiceTime;
	return result;
}

