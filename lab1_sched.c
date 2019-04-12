/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32141868
*	    Student name : ¹ÚÀ¯Çö 
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
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
 * you need to implement FCFS, RR, SPN, SRT, HRRN, MLFQ scheduler. 
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
}

int qPop(struct Queue *q) {
	if (q->front == q->rear) {
		printf("Warning! Empty queue exception! return -1.\n");
		return -1;
	}
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
	int testData[5][2] = {{0, 3}, {2, 6}, {4, 4}, {6, 5}, {8, 2}};
	int mlfqResSize;
	int *mlfq = calcMLFQ(testData, 5, 1, 3, &mlfqResSize);
	printResult(testData, mlfq, 5, mlfqResSize);
	
	mlfq = calcMLFQ(testData, 5, 2, 3, &mlfqResSize);
	printResult(testData, mlfq, 5, mlfqResSize);
	
	mlfq = calcMLFQ(testData, 5, 3, 3, &mlfqResSize);
	printResult(testData, mlfq, 5, mlfqResSize);
	
	mlfq = calcMLFQ(testData, 5, 1, 2, &mlfqResSize);
	printResult(testData, mlfq, 5, mlfqResSize);
	
	mlfq = calcMLFQ(testData, 5, 1, 4, &mlfqResSize);
	printResult(testData, mlfq, 5, mlfqResSize);
	
	return 0;
}

int getLeftTime(int *leftTimeArr, int col) {
	int totalLeftTime = 0;
		for (int i = 0; i < col; i++)
			totalLeftTime += leftTimeArr[i];
	return totalLeftTime;
}

int* calcMLFQ(int data[][2], int col, int timeQuantum, int queueSize, int *resSize) {
	if (col < 1 || queueSize < 2)
		return NULL;
	//Initialize queues for MLFQ.
	struct Queue *queueList = malloc(sizeof(struct Queue) * queueSize);
	for (int i = 0; i < queueSize; i++) {
		struct Queue temp;
		qInit(&temp, 100);
		queueList[i] = temp;
	}
	
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
	//It is difficult to predict the gap between processes now, so allocate enough space.
	int *result = malloc(sizeof(int) * totalServiceTime * 10);
	for (int i = 0; i < totalServiceTime * 10; i++)
		result[i] = -1;
	
	//Main process roof start. Exit when all the processes are finished.
	int procTime = 0; //CPU process timer
	int curProc = -1; //the process currently being executed by CPU
	int quantumTimer = timeQuantum; //time quantum counter
	while (getLeftTime(leftServiceTimeArr, col) > 0) {
		//Add new process to highest priority queue when there is a process currently in procTime.
		//If there are multiple processes arriving at the same time, a process with a smaller array index is added to the queue first. 
		// Because the process is not named in the current simulator.
		for (int i = 0; i < col; i++)
			if (data[i][0] == procTime) {
				for (int j = 0; j < data[i][1]; j++)
					qPush(&queueList[0], i);
				//When a new process is scheduled, if there is a process that was previously running, drop it to the queue below.
				if (i > firstArriveTime && qSize(&queueList[0]) > data[i][1])
					while (qPeek(&queueList[0]) != i)
						qPush(&queueList[1], qPop(&queueList[0]));
				quantumTimer = timeQuantum;
			}
		
		//Execute process in the highest priority queue. 
		for (int i = 0; i < queueSize; i++)
			if (qSize(&queueList[i]) > 0) {
				curProc = qPop(&queueList[i]);
				break;
			}
			else
				curProc = -1;
		if (curProc == -1) { //Continue when there are no processes to execute.
			procTime++;
			continue;
		}
		result[procTime] = curProc; //Push to result.
		leftServiceTimeArr[curProc]--;
		
		//When there are remaining jobs and there are another processes scheduled in scheduler, drop it to the queue below.
		int latestProc = result[procTime];
		int totalQueueSize = 0;
		for (int i = 0; i < queueSize; i++)
			totalQueueSize += qSize(queueList+i);
		for (int i = 0; i < queueSize; i++)
			if (qSize(&queueList[i]) > 0) {
				if (latestProc == qPeek(&queueList[i]))
					quantumTimer--;
				else
					quantumTimer = timeQuantum;
				if (quantumTimer <= 0 && totalQueueSize != leftServiceTimeArr[latestProc]) {
					if (i != queueSize - 1)
						for (int j = 0; j < leftServiceTimeArr[latestProc]; j++)
							qPush(&queueList[i+1], qPop(&queueList[i]));
					else
						for (int j = 0; j < leftServiceTimeArr[latestProc]; j++)
							qPush(&queueList[i], qPop(&queueList[i])); //Push to itself because there is no queue below.
					quantumTimer = timeQuantum;
				}
				break;
			}
			
		procTime++;
	}
	free(leftServiceTimeArr);
	*resSize = procTime;
	return result;
}

void printResult(int inputData[][2], int resData[], int col, int resSize) {
	//Analyze result data. (response time, turnaround time)
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
