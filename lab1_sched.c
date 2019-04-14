/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32141868, 32144697
*	    Student name :
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
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
		for (int i = 0; i < q->max/2; i++)
			newArray[i] = q->data[i];
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

int calcTotalProcessTime(int data[][2],int col) {
	int result=0;
	for (int i=0; i < col; i++)
		result = result + data[i][1];
	return result;
}

int getLeftTime(int *leftTimeArr, int col) {
	int totalLeftTime = 0;
		for (int i = 0; i < col; i++)
			totalLeftTime += leftTimeArr[i];
	return totalLeftTime;
}

/*
 * you need to implement FCFS, RR, SPN, SRT, HRRN, MLFQ scheduler. 
 */

int* calcFCFS(int data[][2], int col,int *resSize) {
	int tempData[col][2];
	int tempX, tempY;
	for (int i = 0; i < col; i++) // Sort the Process Data
		for (int j = 0; j < 2; j++)
			tempData[i][j] = data[i][j];
			
	for (int j = col-1; j > 0; j--)
		for (int i = 0; i < col-1; i++)
			if (tempData[i][0] > tempData[i+1][0]) {
				tempX = tempData[i][0];
				tempY = tempData[i][1];
				tempData[i][0] = tempData[i+1][0];
				tempData[i][1] = tempData[i+1][1];
				tempData[i+1][0] = tempX;
				tempData[i+1][1] = tempY;
			}
		
	for (int i = 0; i < col; i++) //Recopy Data
		for (int j = 0; j < 2; j++)
			 data[i][j]= tempData[i][j];
			 
	int realTime = 0;
	int totalProcessTime = calcTotalProcessTime(tempData, col); // Calculate totalProcessTime
	int leftProcessTime = totalProcessTime;
	int *resultData = malloc(sizeof(int) * totalProcessTime * 10);
	int serviceData[col];
	int checkProcess[col];
	struct Queue q;
	int tempQSize = 0;
	int index = 0;
	qInit(&q, totalProcessTime + 100);
	for (int i = 0; i < col; i++) {
		serviceData[i] = tempData[i][1];
		checkProcess[i] = 0;
	}
	while(1) {
		tempQSize = qSize(&q);
		for (int i = 0; i < col; i++)
			if (realTime >= tempData[i][0] && checkProcess[i] == 0) { // It queues when a process arrives.
				for (int j = 0; j < serviceData[i]; j++) {
					qPush(&q, i);
					realTime++;
					leftProcessTime--;
				}
				checkProcess[i] = 1;
			}
		if (qSize(&q) == tempQSize){	// If no jobs have arrived, wait.
			qPush(&q, -1);
			realTime++;
			totalProcessTime++;
		}
		if (leftProcessTime == 0)	// Exit when all execution is finished.
			break;
	}
	*resSize = totalProcessTime;
	return q.data;
}

int* calcRR(int data[][2], int col, int timeQuantum,int *resSize) {
	int tempData[col][2];
	int tempX, tempY;
	for (int i = 0; i < col; i++)  // Sort the process data.
		for (int j = 0; j < 2; j++)
			tempData[i][j] = data[i][j];
			
	for (int j = col-1; j > 0; j--)
		for (int i = 0; i < col-1; i++)
			if (tempData[i][0] > tempData[i+1][0]) {
				tempX = tempData[i][0];
				tempY = tempData[i][1];
				tempData[i][0] = tempData[i+1][0];
				tempData[i][1] = tempData[i+1][1];
				tempData[i+1][0] = tempX;
				tempData[i+1][1] = tempY;
			}

	for (int i = 0; i < col; i++)
		for (int j = 0; j < 2; j++)
			 data[i][j]= tempData[i][j];

	int totalProcessTime = calcTotalProcessTime(tempData, col); // Calculate totalProcessTime
	int *resultData = malloc(sizeof(int) * totalProcessTime * 10);
	int realTime = 0;
	int temp = -1;
	int emptyCounter = 0;
	int leftServiceTime = 0;
	struct Queue q;
	qInit(&q, totalProcessTime);
	int serviceData[col];
	int checkProcess[col];
	for (int i = 0; i < col; i++) { // Initialiazaion
		serviceData[i] = tempData[i][1];
		checkProcess[i] = 0;
	}
	for (int i = 0; i < col; i++)
		leftServiceTime += serviceData[i];
	while (1) {	//Loop
		for (int i = 0; i < col; i++)
			if (realTime >= tempData[i][0] && checkProcess[i] == 0) {   // It queues when a process arrives.
				for (int j = 0; j < serviceData[i]; j++)
					qPush(&q, i);
				if (temp == qPeek(&q))	// If a new job arrives and there is a previous process running, the previous process is sent back.
					for (int k = 0; k < serviceData[temp]; k++)
						qPush(&q, qPop(&q));
				checkProcess[i] = 1;
			}
		if (temp == qPeek(&q)) // Processes that were performed without any new arrivals are also backward.
			for (int w = 0; w < serviceData[temp]; w++)
				qPush(&q, qPop(&q));
				
		for (int i = 0; i < timeQuantum; i++) { // Executes as much as the time quantum.
			if (qSize(&q) == 0) // Ends the loop if the queue is empty.
				break;
			leftServiceTime--;
			temp = qPop(&q);								
			resultData[realTime] = temp;							
			serviceData[temp]--;
			realTime++;		// If the process is successful, it will take time.			
			if(temp != qPeek(&q)) // When the process has finished running, it exits the loop.
				break;
		}
		if (qSize(&q) == 0) { // The part that handles spaces between processes.
			emptyCounter++;
			if(emptyCounter == 3) { 
				resultData[realTime] =- 1;
				realTime++;
				totalProcessTime++;
				emptyCounter = 0;
			}
		}
		else
			emptyCounter = 0;
		if(leftServiceTime == 0) // Exit when all execution is finished.
			break;
	}
	*resSize = totalProcessTime;	
	return resultData;
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
	int totalServiceTime = 0; //minimum total process service time from first process arrival	
	int *leftServiceTimeArr = malloc(sizeof(int) * col); //remaining service time temporary storage for each process
	for (int i = 0; i < col; i++) {
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
	int curProc; //the process currently being executed by CPU
	int quantumTimer = timeQuantum; //time quantum counter
	while (getLeftTime(leftServiceTimeArr, col) > 0) {
		//Add new process to highest priority queue when there is a process currently in procTime.
		//If there are multiple processes arriving at the same time, a process with a smaller array index is added to the queue first. 
		// Because the process is not named in the current simulator.
		int pushCnt = -1;
		for (int i = 0; i < col; i++)
			if (data[i][0] == procTime) {
				for (int j = 0; j < data[i][1]; j++)
					qPush(&queueList[0], i);
				if (pushCnt == -1)
					pushCnt = i;
			}
		
		//When a new process is scheduled, if there is a process that was previously running, drop it to the queue below.
		if (pushCnt != -1)
			if (qSize(&queueList[0]) > data[pushCnt][1]) {
				int latestProc = result[procTime-1];
				while (qPeek(&queueList[0]) == latestProc && pushCnt != latestProc)
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
		if (curProc > -1) { //Continue roof when there are no processes to execute.
			result[procTime] = curProc; //Push to result.
			leftServiceTimeArr[curProc]--;
			
			//When there are remaining jobs and there are another processes scheduled in scheduler, drop it to the queue below.
			int totalQueueSize = 0;
			for (int i = 0; i < queueSize; i++)
				totalQueueSize += qSize(queueList+i);
			for (int i = 0; i < queueSize; i++)
				if (qSize(&queueList[i]) > 0) {
					if (curProc == qPeek(&queueList[i]))
						quantumTimer--;
					else
						quantumTimer = timeQuantum;
					if (quantumTimer <= 0 && totalQueueSize != leftServiceTimeArr[curProc]) {
						for (int j = 0; j < leftServiceTimeArr[curProc]; j++)
							if (i != queueSize - 1)
								qPush(&queueList[i+1], qPop(&queueList[i]));
							else //Push to itself because there is no queue below.
								qPush(&queueList[i], qPop(&queueList[i]));
						quantumTimer = timeQuantum;
					}
					break;
				}
		}
		procTime++;
	}
	*resSize = procTime;
	return result;
}

int* calcLottery(int data[][2], int col, int *resSize) {
	if (col < 1)
		return NULL;
	//Analyze input data.
	int totalServiceTime = 0; //minimum total process service time from first process arrival	
	int *leftServiceTimeArr = malloc(sizeof(int) * col); //remaining service time temporary storage for each process
	int *ticketArr = malloc(sizeof(int) * col); //remaining service time temporary storage for each process
	for (int i = 0; i < col; i++) {
		totalServiceTime += data[i][1];
		leftServiceTimeArr[i] = data[i][1];
		ticketArr[i] = 0;
	}
	
	//Set empty result data.
	//It is difficult to predict the gap between processes now, so allocate enough space.
	int *result = malloc(sizeof(int) * totalServiceTime * 10);
	for (int i = 0; i < totalServiceTime * 10; i++)
		result[i] = -1;
	
	//Main process roof start. Exit when all the processes are finished.
	int procTime = 0; //CPU process timer
	int curProc; //the process currently being executed by CPU
	while (getLeftTime(leftServiceTimeArr, col) > 0) {
		//Add new process when there is a process currently in procTime.
		int pushCnt = -1;
		for (int i = 0; i < col; i++)
			if (data[i][0] == procTime) {
				for (int j = 0; j < data[i][1]; j++)
					ticketArr[i] = data[i][1];
				if (pushCnt == -1)
					pushCnt = i;
			}
			
		//Executes a process that randomly picks one of the tickets.
		int ticketAmount = 0;
		for (int i = 0; i < col; i++)
			ticketAmount += ticketArr[i];
		if (ticketAmount > 0) { //Continue roof when there are no tickets to pick.
			int r = rand() % ticketAmount;
			for (int i = 0; i < col; i++)
				if (r >= ticketArr[i])
					r -= ticketArr[i];
				else {
					curProc = i;
					break;
				}
			result[procTime] = curProc; //Push to result.
			leftServiceTimeArr[curProc]--;
			
			for (int i = 0; i < col; i++)
				if (leftServiceTimeArr[i] == 0)
					ticketArr[i] = 0;
		}
		procTime++;
	}
	*resSize = procTime;
	return result;
}
