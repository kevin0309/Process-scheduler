/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32141868, 32144697
*	    Student name : 박유현, 최광진 
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

char getUserCommand() {
	char res;
	printf("Enter your command : ");
	scanf("%c", &res); getchar();
	return res;
}

int **getProcessData(int *size) {
	while (1) {
		printf("Choose how many process data to input.\nEnter your command : ");
		int dataSize;
		scanf("%d", &dataSize);
		if (dataSize > 0) {
			*size = dataSize;
			int **result = malloc(sizeof(int*) * dataSize);
			for (int i = 0; i < dataSize; i++) {
				result[i] = malloc(sizeof(int) * 2);
				printf("Process %c arrival : ", (char)(i+65));
				scanf("%d", &result[i][0]); getchar();
				printf("Process %c duration : ", (char)(i+65));
				scanf("%d", &result[i][1]); getchar();
				printf("\n");
			}
			return result;
		}
		else
			printf("Amount of process data must be greater than 0! Please try again.\n");
	}
	return NULL;
}

int getLeftTime(int *leftTimeArr, int col) {
	int totalLeftTime = 0;
		for (int i = 0; i < col; i++)
			totalLeftTime += leftTimeArr[i];
	return totalLeftTime;
}

/*
 * you need to implement scheduler simlator test code.
 */
 
int main(int argc, char *argv[]){
	while(1) {
		int roofEndFlag = 1;
		printf("\n # Process-scheduler simulator\n\n");
		printf(" 1. FCFS\n 2. RR\n 3. MLFQ\n 4. Lottery\n 5. Exit\n");
		char command = getUserCommand();
		printf("\n");
		if (command == '1'){
			while (roofEndFlag) {
				printf("\n # FCFS\n\n   Option setup\n-----------------\n");
				printf(" 1. Choose which input data to use.\n");
				printf("   1. Type directly.\n   2. Use sample process data.\n");
				char command2 = getUserCommand();
				if (command2 == '1') {
					printf("\n");
					int procSize;
					int **tempData = getProcessData(&procSize);
					int testData[procSize][2];
					for (int i = 0; i < procSize; i++) {
						testData[i][0] = tempData[i][0];
						testData[i][1] = tempData[i][1];
					}
					int fcfsResSize;
					int *fcfs = calcFCFS(testData, procSize, &fcfsResSize);
					printResult(testData, fcfs, procSize, fcfsResSize);
					printf("Return to the main menu.\n\n\n");
					roofEndFlag = 0;
				}
				else if (command2 == '2') {
					printf("\n # FCFS\n\n   Option setup\n-----------------\n");
					printf(" 2. Choose sample data.\n\n");
					printf(" 1.       A  B  C  D  E\n");
					printf("arrival   0  2  4  6  8\n");
					printf("duration  3  6  4  5  2\n\n");
					printf(" 2.       A  B  C\n");
					printf("arrival   0  1  2\n");
					printf("duration 15  1  1\n\n");
					char command3 = getUserCommand();
					if (command3 == '1') {
						int testData[5][2] = {{0, 3}, {2, 6}, {4, 4}, {6, 5}, {8, 2}};
						int fcfsResSize;
						int *fcfs = calcFCFS(testData, 5, &fcfsResSize);
						printResult(testData, fcfs, 5, fcfsResSize);
						printf("Return to the main menu.\n\n\n");
						roofEndFlag = 0;
					}
					else if (command3 == '2') {
						int testData[3][2] = {{0, 15}, {1, 1}, {2, 1}};
						int fcfsResSize;
						int *fcfs = calcFCFS(testData, 3, &fcfsResSize);
						printResult(testData, fcfs, 3, fcfsResSize);
						printf("Return to the main menu.\n\n\n");
						roofEndFlag = 0;
					}
					else
						printf("Invalid command! Please try again.\n");
				}
				else
					printf("Invalid command! Please try again.\n");
			}
		}
		else if (command == '2') {
			while (roofEndFlag) {
				printf("\n # RR\n\n   Option setup\n-----------------\n");
				printf("->1. time quantum\n");
				printf("  2. input process data\n");
				printf("-----------------\n");
				printf(" 1. Choose how much time quantum to use. (1~)\n");
				printf("Enter your command : ");
				int timeQuantum;
				scanf("%d", &timeQuantum); getchar();
				if (timeQuantum > 0) {
					while (roofEndFlag) {
						printf("\n # RR\n\n   Option setup\n-----------------\n");
						printf("  1. time quantum\n");
						printf("->2. input process data\n");
						printf("-----------------\n");
						printf(" 2.1 Choose which input data to use.\n");
						printf("   1. Type directly.\n   2. Use sample process data.\n");
						int command2 = getUserCommand();
						if (command2 == '1') {
							printf("\n");
							int procSize;
							int **tempData = getProcessData(&procSize);
							int testData[procSize][2];
							for (int i = 0; i < procSize; i++) {
								testData[i][0] = tempData[i][0];
								testData[i][1] = tempData[i][1];
							}
							int rrResSize;
							int *rr = calcRR(testData, procSize, timeQuantum, &rrResSize);
							printResult(testData, rr, procSize, rrResSize);
							printf("Return to the main menu.\n\n\n");
							roofEndFlag = 0;
						}
						else if (command2 == '2') {
							while (roofEndFlag) {
								printf("\n # RR\n\n   Option setup\n-----------------\n");
								printf("  1. time quantum\n");
								printf("->2. input process data\n");
								printf("-----------------\n");
								printf(" 2.2 Choose sample data.\n\n");
								printf(" 1.       A  B  C  D  E\n");
								printf("arrival   0  2  4  6  8\n");
								printf("duration  3  6  4  5  2\n\n");
								printf(" 2.       A  B  C  D  E  F  G  H\n");
								printf("arrival   0  2  4  6  8 10 12 14\n");
								printf("duration  4  4  4  4  4  4  4  4\n\n");
								char command3 = getUserCommand();
								if (command3 == '1') {
									int testData[5][2] = {{0, 3}, {2, 6}, {4, 4}, {6, 5}, {8, 2}};
									int rrResSize;
									int *rr = calcRR(testData, 5, timeQuantum, &rrResSize);
									printResult(testData, rr, 5, rrResSize);
									printf("Return to the main menu.\n\n\n");
									roofEndFlag = 0;
								}
								else if (command3 == '2') {
									int testData[8][2] = {{0, 4}, {2, 4}, {4, 4}, {6, 4}, {8, 4}, {10, 4}, {12, 4}, {14, 4}};
									int rrResSize;
									int *rr = calcRR(testData, 8, timeQuantum, &rrResSize);
									printResult(testData, rr, 8, rrResSize);
									printf("Return to the main menu.\n\n\n");
									roofEndFlag = 0;
								}
								else
									printf("Invalid command! Please try again.\n");
							}
						}
						else
							printf("Invalid command! Please try again.\n");
					}
				}
				else
					printf("Time quantum must be greater than 0! Please try again.\n");
				break;
			}
		}
		else if (command == '3') {
			while (roofEndFlag) {
				printf("\n # MLFQ\n\n   Option setup\n-----------------\n");
				printf("->1. time quantum\n");
				printf("  2. queue size\n");
				printf("  3. input process data\n");
				printf("-----------------\n");
				printf(" 1. Choose how much time quantum to use. (1~)\n");
				printf("Enter your command : ");
				int timeQuantum;
				scanf("%d", &timeQuantum); getchar();
				if (timeQuantum > 0) {
					while (roofEndFlag) {
						printf("\n # MLFQ\n\n   Option setup\n-----------------\n");
						printf("  1. time quantum\n");
						printf("->2. queue size\n");
						printf("  3. input process data\n");
						printf("-----------------\n");
						printf(" 2. Choose how much of the queue MLFQ will use. (2~)\n");
						printf("Enter your command : ");
						int queueSize;
						scanf("%d", &queueSize); getchar();
						if (queueSize > 1) {
							while (roofEndFlag) {
								printf("\n # MLFQ\n\n   Option setup\n-----------------\n");
								printf("  1. time quantum\n");
								printf("  2. queue size\n");
								printf("->3. input process data\n");
								printf("-----------------\n");
								printf(" 3.1 Choose which input data to use.\n");
								printf("   1. Type directly.\n   2. Use sample process data.\n");
								int command2 = getUserCommand();
								if (command2 == '1') {
									printf("\n");
									int procSize;
									int **tempData = getProcessData(&procSize);
									int testData[procSize][2];
									for (int i = 0; i < procSize; i++) {
										testData[i][0] = tempData[i][0];
										testData[i][1] = tempData[i][1];
									}
									int mlfqResSize;
									int *mlfq = calcMLFQ(testData, procSize, timeQuantum, queueSize, &mlfqResSize);
									printResult(testData, mlfq, procSize, mlfqResSize);
									printf("Return to the main menu.\n\n\n");
									roofEndFlag = 0;
								}
								else if (command2 == '2') {
									while (roofEndFlag) {
										printf("\n # MLFQ\n\n   Option setup\n-----------------\n");
										printf("  1. time quantum\n");
										printf("  2. queue size\n");
										printf("->3. input process data\n");
										printf("-----------------\n");
										printf(" 3.2 Choose sample data.\n\n");
										printf(" 1.       A  B  C  D  E\n");
										printf("arrival   0  2  4  6  8\n");
										printf("duration  3  6  4  5  2\n\n");
										printf(" 2.       A  B  C  D  E  F  G  H\n");
										printf("arrival   4  2  4  6  8 29 32 33\n");
										printf("duration  8  6  4  5  2  8  1  6\n\n");
										char command3 = getUserCommand();
										if (command3 == '1') {
											int testData[5][2] = {{0, 3}, {2, 6}, {4, 4}, {6, 5}, {8, 2}};
											int mlfqResSize;
											int *mlfq = calcMLFQ(testData, 5, timeQuantum, queueSize, &mlfqResSize);
											printResult(testData, mlfq, 5, mlfqResSize);
											printf("Return to the main menu.\n\n\n");
											roofEndFlag = 0;
										}
										else if (command3 == '2') {
											int testData[8][2] = {{4, 8}, {2, 6}, {4, 4}, {6, 5}, {8, 2}, {29, 8}, {32, 1}, {33, 6}};
											int mlfqResSize;
											int *mlfq = calcMLFQ(testData, 8, timeQuantum, queueSize, &mlfqResSize);
											printResult(testData, mlfq, 8, mlfqResSize);
											printf("Return to the main menu.\n\n\n");
											roofEndFlag = 0;
										}
										else
											printf("Invalid command! Please try again.\n");
									}
								}
								else
									printf("Invalid command! Please try again.\n");
							}
						}
						else
							printf("Amount of queue must be greater than 1! Please try again.\n");
					}
				}
				else
					printf("Time quantum must be greater than 0! Please try again.\n");
				break;
			}
		}
		else if (command == '4') {
			while (roofEndFlag) {
				printf("\n # Lottery\n\n   Option setup\n-----------------\n");
				printf(" 1. Choose which input data to use.\n");
				printf("   1. Type directly.\n   2. Use sample process data.\n");
				int command2 = getUserCommand();
				if (command2 == '1') {
					printf("\n");
					int procSize;
					int **tempData = getProcessData(&procSize);
					int testData[procSize][2];
					for (int i = 0; i < procSize; i++) {
						testData[i][0] = tempData[i][0];
						testData[i][1] = tempData[i][1];
					}
					int lotteryResSize;
					int *lottery = calcLottery(testData, procSize, &lotteryResSize);
					printResult(testData, lottery, procSize, lotteryResSize);
					printf("Return to the main menu.\n\n\n");
					roofEndFlag = 0;
				}
				else if (command2 == '2') {
					printf("\n # Lottery\n\n   Option setup\n-----------------\n");
					printf(" 2. Choose sample data.\n\n");
					printf(" 1.       A  B  C  D  E\n");
					printf("arrival   0  2  4  6  8\n");
					printf("duration  3  6  4  5  2\n\n");
					printf(" 2.       A  B  C  D  E  F\n");
					printf("arrival   0  0  0  0  0  0\n");
					printf("duration  5  5  5  5  5  5\n\n");
					char command3 = getUserCommand();
					if (command3 == '1') {
						int testData[5][2] = {{0, 3}, {2, 6}, {4, 4}, {6, 5}, {8, 2}};
						int lotteryResSize;
						int *lottery = calcLottery(testData, 5, &lotteryResSize);
						printResult(testData, lottery, 5, lotteryResSize);
						printf("Return to the main menu.\n\n\n");
						roofEndFlag = 0;
					}
					else if (command3 == '2') {
						int testData[6][2] = {{0, 5}, {0, 5}, {0, 5}, {0, 5}, {0, 5}, {0, 5}};
						int lotteryResSize;
						int *lottery = calcLottery(testData, 6, &lotteryResSize);
						printResult(testData, lottery, 6, lotteryResSize);
						printf("Return to the main menu.\n\n\n");
						roofEndFlag = 0;
					}
					else
						printf("Invalid command! Please try again.\n");
				}
				else
					printf("Invalid command! Please try again.\n");
			}
		}
		else if (command == '5')
			break;
		else
			printf("Invalid command! Please try again.\n");
		roofEndFlag = 1;
	}
}

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
		if (pushCnt != -1) {
			if (qSize(&queueList[0]) > data[pushCnt][1])
				while (qPeek(&queueList[0]) != pushCnt)
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
	printf("\n       ");
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
	printf("\n       ");
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
	printf("\n-------");
	for (int i = 0; i < resSize; i++)
		printf("---");
	printf("\nTime | ");
	for (int i = 0; i < resSize; i++)
		printf("%2d ", i);
	printf("\n-------");
	for (int i = 0; i < resSize; i++)
		printf("---");
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
		if (resData[i] == -1)
			printf(" - ");
		else
			printf("%2c ", (char)(resData[i]+65));
	printf("\n-------");
	for (int i = 0; i < resSize; i++)
		printf("---");
	printf("\n\n------------------------");
	for (int i = 0; i < col; i++)
		printf("-----");
	printf("\n (Analyzation)  |");
	for (int i = 0; i < col; i++)
		printf("%4c ", (char)(i+65));
	printf("    AVG \n------------------------");
	for (int i = 0; i < col; i++)
		printf("-----");
	printf("\n");
	printf("  Response time |");
	for (int i = 0; i < col; i++)
		printf("%4d ", responseTime[i]);
	printf(" %6.2f", avgResponseTime);
	printf("\nTurnaround time |");
	for (int i = 0; i < col; i++)
		printf("%4d ", turnaroundTime[i]);
	printf(" %6.2f", avgTurnaroundTime);
	printf("\n------------------------");
	for (int i = 0; i < col; i++)
		printf("-----");
	printf("\n");
}
