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
	calcMLFQ(testData, 5, 2);
	
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
	/////dev
	printf("left service time : ");
	for (int i = 0; i < col; i++) {
		printf("%d ", leftServiceTimeArr[i]);
	}
	printf("\n");
	/////
			
	int *result = malloc(sizeof(int) * totalServiceTime);
	for (int i = 0; i < totalServiceTime; i++)
		result[i] = -1;
	//Main process roof start
	int procTime = 0;
	int quantumTimer = timeQuantum;
	int curProc = -2;
		printf("current process : %d\n", curProc);
	while (1) {
		printf("===================================================================================================\nprocess timer : %d\n===================================================================================================\n", procTime);
		for (int i = 0; i < col; i++)
			if (data[i][0] == procTime)
				for (int j = 0; j < data[i][1]; j++)
					qPush(&l1, i);
		
		/////dev
		printf("---------------------------------------------------------------------------\nadd.\n---------------------------------------------------------------------------\n");
		printf("left service time : ");
		for (int i = 0; i < col; i++) {
			printf("%d ", leftServiceTimeArr[i]);
		}
		printf("\n");
		printf("l1 : ");
		qPrint(&l1);
		printf("l2 : ");
		qPrint(&l2);
		printf("l3 : ");
		qPrint(&l3);
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
		/////
		
			
		printf("current process : %d, quantum count : %d\n", curProc, quantumTimer);
		//작업 진행하는 부분 추가 (time quantum 고려) 
		int latestProc = result[procTime-1];
		if (qSize(&l1) > 0) {
			printf("if1 rear: %d\nresult[procTime-1] : %d\n", l1.data[l1.rear+1], latestProc);
			if (latestProc == l1.data[l1.rear+1])
				quantumTimer--;
			else
				quantumTimer = timeQuantum;
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) > leftServiceTimeArr[latestProc]) {
				printf("tq\n");
				for (int i = 0; i < leftServiceTimeArr[latestProc]; i++) {
					qPush(&l2, qPop(&l1));
				}
				quantumTimer = timeQuantum;
			}
		}
		else if (qSize(&l2) > 0) {
			printf("if2 rear: %d\n", l2.data[l2.rear+1]);
			if (latestProc == l2.data[l2.rear+1])
				quantumTimer--;
			else
				quantumTimer = timeQuantum;
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) > leftServiceTimeArr[latestProc]) {
				printf("tq\n");
				for (int i = 0; i < leftServiceTimeArr[latestProc]; i++) {
					qPush(&l3, qPop(&l2));
				}
				quantumTimer = timeQuantum;
			}
		}
		else
			quantumTimer = timeQuantum;
			
		/////dev
		printf("---------------------------------------------------------------------------\nquantum timer 1.\n---------------------------------------------------------------------------\n");
		printf("left service time : ");
		for (int i = 0; i < col; i++) {
			printf("%d ", leftServiceTimeArr[i]);
		}
		printf("\n");
		printf("l1 : ");
		qPrint(&l1);
		printf("l2 : ");
		qPrint(&l2);
		printf("l3 : ");
		qPrint(&l3);
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
		/////	
		
		if (qSize(&l1) > 0) {
			curProc = qPop(&l1);
		}
		else if (qSize(&l2) > 0) {
			curProc = qPop(&l2);
		}
		else {
			curProc = qPop(&l3);
		}
		result[procTime] = curProc;
		leftServiceTimeArr[curProc]--;
		
		/////dev
		printf("---------------------------------------------------------------------------\nprocess.\n---------------------------------------------------------------------------\n");
		printf("left service time : ");
		for (int i = 0; i < col; i++) {
			printf("%d ", leftServiceTimeArr[i]);
		}
		printf("\n");
		printf("l1 : ");
		qPrint(&l1);
		printf("l2 : ");
		qPrint(&l2);
		printf("l3 : ");
		qPrint(&l3);
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
		/////
		
		printf("current process : %d, quantum count : %d\n", curProc, quantumTimer);
		//작업 진행하는 부분 추가 (time quantum 고려) 
		int latestProc2 = result[procTime];
		if (qSize(&l1) > 0) {
			printf("if1 rear: %d\nresult[procTime] : %d\n", l1.data[l1.rear+1], latestProc2);
			/*if (latestProc2 == l1.data[l1.rear+1])
				quantumTimer--;
			else
				quantumTimer = timeQuantum;*/
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) > leftServiceTimeArr[latestProc2]) {
				printf("tq\n");
				for (int i = 0; i < leftServiceTimeArr[latestProc2]; i++) {
					qPush(&l2, qPop(&l1));
				}
				quantumTimer = timeQuantum;
			}
		}
		else if (qSize(&l2) > 0) {
			printf("if2 rear: %d\n", l2.data[l2.rear+1]);
			/*if (latestProc2 == l2.data[l2.rear+1])
				quantumTimer--;
			else
				quantumTimer = timeQuantum;*/
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) > leftServiceTimeArr[latestProc2]) {
				printf("tq\n");
				for (int i = 0; i < leftServiceTimeArr[latestProc2]; i++) {
					qPush(&l3, qPop(&l2));
				}
				quantumTimer = timeQuantum;
			}
		}
		else {
			quantumTimer = timeQuantum;
			if (curProc == l3.data[l3.rear+1]) {
				for (int i = 0; i < leftServiceTimeArr[curProc]-1; i++) {
					qPush(&l3, qPop(&l3));
				}
			}
		}
			
		/////dev
		printf("---------------------------------------------------------------------------\nquantum timer 2.\n---------------------------------------------------------------------------\n");
		printf("left service time : ");
		for (int i = 0; i < col; i++) {
			printf("%d ", leftServiceTimeArr[i]);
		}
		printf("\n");
		printf("l1 : ");
		qPrint(&l1);
		printf("l2 : ");
		qPrint(&l2);
		printf("l3 : ");
		qPrint(&l3);
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
		/////	
		
		procTime++;
		//break condition 1. Exit when all services are finised
		if (procTime > minArriveTime + totalServiceTime) {
			printf("service finished (condition 1)\n");
			break;
		}
		//break condition 2. Exit when procTime is appropriate or the queues are all empty
		if (procTime > maxArriveTime && qSize(&l1) == 0 && qSize(&l2) == 0 && qSize(&l3) == 0) {
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

