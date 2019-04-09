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

#include <aio.h>
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
#include <asm/unistd.h>

#include "lab1_sched_types.h"

/*
 * you need to implement scheduler simlator test code.
 *
 */
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

int main(int argc, char *argv[]){
	printf("Hello World!\n");
	struct Queue q;
	qInit(&q, 3);
	qPush(&q, 1);
	qPush(&q, 2);
	qPush(&q, 3);
	qPush(&q, 4);
	qPush(&q, 5);
	qPush(&q, 6);
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
	printf("%d\n", qPop(&q));
}

