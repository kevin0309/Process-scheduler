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

int qPeek(struct Queue *q) {
	return q->data[q->rear+1];
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
	int *mlfq = calcMLFQ(testData, 5, 1, &mlfqResSize);
	printResult(testData, mlfq, 5, mlfqResSize);
	
	mlfq = calcMLFQ(testData, 5, 2, &mlfqResSize);
	printResult(testData, mlfq, 5, mlfqResSize);
	
	mlfq = calcMLFQ(testData, 5, 3, &mlfqResSize);
	printResult(testData, mlfq, 5, mlfqResSize);
}

void printDev(int col, int leftServiceTimeArr[], struct Queue *l1, struct Queue *l2, struct Queue *l3, int procTime, int result[]) {
	printf("---------------------------------------------------------------------------\nleft service time : ");
	for (int i = 0; i < col; i++)
		printf("%d ", leftServiceTimeArr[i]);
	printf("\nl1 : "); qPrint(l1);
	printf("l2 : "); qPrint(l2);
	printf("l3 : "); qPrint(l3);
	printf("result : ");
	for (int i = 0; i < procTime+1; i++)
		printf("%c ", (char)(result[i]+65));
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
	int lastArriveTime = 0; //last arrive time between processes
	int firstArriveTime = data[0][0]; //first arrive time between processes
	int totalServiceTime = 0; //total process service time from first process arrival	
	int *leftServiceTimeArr = malloc(sizeof(int) * col); //remaining service time temporary storage of processes
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
	int procTime = 0; //CPU process timer
	int curProc; //the process currently being processed by CPU
	int quantumTimer = timeQuantum; //time quantum counter
	while (1) {
		/////
		//printf("===================================================================================================\nprocess timer : %d\n===================================================================================================\n", procTime);
		/////
		
		//Add new process to Queue1(highest priority) when there is a process currently in procTime.
		//If there are multiple processes arriving at the same time, a process with a smaller array index is added to the queue. Because the process is not named in the current structure.
		for (int i = 0; i < col; i++)
			if (data[i][0] == procTime) {
				for (int j = 0; j < data[i][1]; j++)
					qPush(&l1, i);
				//When a new process is inserted, if there is a process that was previously running, drop it to the queue below.
				if (i > firstArriveTime && qSize(&l1) > data[i][1])
					while (qPeek(&l1) != i)
						qPush(&l2, qPop(&l1));
				quantumTimer = timeQuantum;
			}
		
		/////
		//printf("1. Add ");
		//printDev(col, leftServiceTimeArr, &l1, &l2, &l3, procTime, result);
		/////
		
		//Execute process in the highest priority queue. 
		if (qSize(&l1) > 0)
			curProc = qPop(&l1);
		else if (qSize(&l2) > 0)
			curProc = qPop(&l2);
		else
			curProc = qPop(&l3);
		result[procTime] = curProc; //Push to result.
		leftServiceTimeArr[curProc]--;
		
		/////
		//printf("2. Process ");
		//printDev(col, leftServiceTimeArr, &l1, &l2, &l3, procTime, result);
		/////
		
		//When there are remaining jobs and there are no other processes scheduled in scheduler, drop it to the queue below.
		int latestProc = result[procTime];
		if (qSize(&l1) > 0) {
			if (latestProc == qPeek(&l1))
				quantumTimer--;
			else
				quantumTimer = timeQuantum;
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) != leftServiceTimeArr[latestProc]) {
				for (int i = 0; i < leftServiceTimeArr[latestProc]; i++)
					qPush(&l2, qPop(&l1));
				quantumTimer = timeQuantum;
			}
		}
		else if (qSize(&l2) > 0) {
			if (latestProc == qPeek(&l2))
				quantumTimer--;
			else
				quantumTimer = timeQuantum;
			if (quantumTimer <= 0 && qSize(&l1) + qSize(&l2) + qSize(&l3) != leftServiceTimeArr[latestProc]) {
				for (int i = 0; i < leftServiceTimeArr[latestProc]; i++)
					qPush(&l3, qPop(&l2));
				quantumTimer = timeQuantum;
			}
		}
		else { 
			if (latestProc == qPeek(&l3))
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
		//printf("3. Quantum ");
		//printDev(col, leftServiceTimeArr, &l1, &l2, &l3, procTime, result);
		/////
		
		procTime++;
		//Exit when procTime is appropriate or the queues are all empty
		if (procTime > lastArriveTime && qSize(&l1) == 0 && qSize(&l2) == 0 && qSize(&l3) == 0)
			break;
	}
	free(leftServiceTimeArr);
	*resSize = totalServiceTime;
	return result;
}

void printResult(int inputData[][2], int resData[], int col, int resSize) {
	//Analyze result data.
	int firstProcessedTime[col];
	int lastProcessedTime[col];
	int responseTime[col];
	float avgResponseTime = 0;
	int turnaroundTime[col];
	float avgTurnaroundTime = 0;
	
	for (int i = 0; i < col; i++) {
		firstProcessedTime[i] = 2 * resSize;
		lastProcessedTime[i] = 0;
	}
	for (int i = 0; i < resSize; i++) {
		if (firstProcessedTime[resData[i]] > i)
			firstProcessedTime[resData[i]] = i;
		if (lastProcessedTime[resData[i]] < i)
			lastProcessedTime[resData[i]] = i;
	}
	for (int i = 0; i < col; i++) {
		responseTime[i] = firstProcessedTime[i] - inputData[i][0];
		avgResponseTime += responseTime[i];
		turnaroundTime[i] = lastProcessedTime[i] - inputData[i][0] + 1;
		avgTurnaroundTime += turnaroundTime[i];
	}
	avgResponseTime /= col;
	avgTurnaroundTime /= col;
	
	//Print result.
	printf("\n # 'Process-scheduler simulator' RESULT REPORT\n\n       ");
	for (int i = 0; i < resSize; i++) {
		int arrCnt = 0;
		int tempArr[col];
		for (int j = 0; j < col; j++)
			if (inputData[j][0] == i)
				tempArr[arrCnt++] = j;
		if (arrCnt == 1)
			printf("%2c ", (char)(tempArr[0]+65));
		else if (arrCnt > 1)
			printf("%c..", (char)(tempArr[0]+65));
		else
			printf("   ");
	}
	printf("\n");
	printf("       ");
	for (int i = 0; i < resSize; i++) {
		int arrCnt = 0;
		for (int j = 0; j < col; j++)
			if (inputData[j][0] == i)
				arrCnt++;
		if (arrCnt > 0)
			printf(" | ");
		else
			printf("   ");
	}
	printf("\n");
	printf("       ");
	for (int i = 0; i < resSize; i++) {
		int arrCnt = 0;
		for (int j = 0; j < col; j++)
			if (inputData[j][0] == i)
				arrCnt++;
		if (arrCnt > 0)
			printf(" V ");
		else
			printf("   ");
	}
	printf("\n");
	printf("-------");
	for (int i = 0; i < resSize; i++)
		printf("---", i);
	printf("\n");
	printf("Time | ");
	for (int i = 0; i < resSize; i++)
		printf("%2d ", i);
	printf("\n-------");
	for (int i = 0; i < resSize; i++)
		printf("---", i);
	printf("\n");
	for (int i = 0; i < col; i++) {
		printf("   %c | ", (char)(i+65));
		for (int j = 0; j < resSize; j++)
			if (resData[j] == i)
				printf("%2c ", 'O');
			else
				printf("%2c ", '-');
		printf("\n");
	}
	printf("(SUM)| ");
	for (int i = 0; i < resSize; i++)
		printf("%2c ", (char)(resData[i]+65));
	printf("\n");
	printf("-------");
	for (int i = 0; i < resSize; i++)
		printf("---", i);
	printf("\n\n");
	printf("------------------------");
	for (int i = 0; i < col; i++)
		printf("-----", i);
	printf("\n");
	printf(" (Analyzation)  |");
	for (int i = 0; i < col; i++)
		printf("%4c ", (char)(i+65));
	printf("    AVG \n");
	printf("------------------------");
	for (int i = 0; i < col; i++)
		printf("-----", i);
	printf("\n");
	printf("  Response time |");
	for (int i = 0; i < col; i++)
		printf("%4d ", responseTime[i]);
	printf(" %6.2f", avgResponseTime);
	printf("\n");
	printf("Turnaround time |");
	for (int i = 0; i < col; i++)
		printf("%4d ", turnaroundTime[i]);
	printf(" %6.2f", avgTurnaroundTime);
	printf("\n");
	printf("------------------------");
	for (int i = 0; i < col; i++)
		printf("-----", i);
	printf("\n");
}
