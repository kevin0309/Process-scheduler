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
		printf("Warning! Empty queue exception! return 0.\n");
		return 0;
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
	int testData[3][2] = {{0, 5}, {2, 3}, {4, 4}};
	calcMLFQ(testData, 3, 1);
	
	return 0;
}

int* calcMLFQ(int data[][2], int col, int timeQuantum) {
	if (col == 0) {
		printf("Empty data!!!\n");
		return data[0];
	}
	//Initialize 3 queues for MLFQ
	struct Queue l1; qInit(&l1, 100);
	struct Queue l2; qInit(&l2, 100);
	struct Queue l3; qInit(&l3, 100);
	
	//Analyze input data
	int maxArriveTime = 0;
	int minArriveTime = data[0][0];
	int totalServiceTime = 0; 
	int *leftServiceTimeArr = malloc(sizeof(int) * col);
	for (int i = 0; i < col; i++) {
		if (data[i][0] > maxArriveTime)
			maxArriveTime = data[i][0];
		if (data[i][0] < minArriveTime)
			minArriveTime = data[i][0];
		totalServiceTime += data[i][1];
		leftServiceTimeArr[i] = data[i][1];
	}
			
	int *result = malloc(sizeof(int) * totalServiceTime);
	//Main process roof start
	int procTime = 0;
	while (1) {
		for (int i = 0; i < col; i++)
			if (data[i][0] == procTime)
				qPush(&l1, i);
		
		//작업 진행하는 부분 추가 (time quantum 고려) 
		for (int i = 0; i < timeQuantum; i++) {
			//if ()
		}
		
		procTime++;
		//break condition 1. Exit when all services are finised
		if (procTime > minArriveTime + totalServiceTime) {
			printf("service finished (condition 1)\n");
			break;
		}
		//break condition 2. Exit when procTime is appropriate or the queues are all empty
		if (procTime > maxArriveTime && qSize(&l1) == 0 && qSize(&l2) && qSize(&l3) || procTime > 100 * maxArriveTime) {/////마지막 100배 조건은 개발용 
			printf("service finished (condition 2)\n");
			break;
		}
		
		/*if (procTime == 5) {
			qPrint(&l1);
			qPrint(&l2);
			qPrint(&l3);
			break;
		}*/
	}
	return result;
}

