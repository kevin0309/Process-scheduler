/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32141868, 32144697
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
						printf(" 2. Choose how much of the queue MLFQ will use. (1~)\n");
						printf("Enter your command : ");
						int queueSize;
						scanf("%d", &queueSize); getchar();
						if (queueSize > 0) {
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
							printf("Amount of queue must be greater than 0! Please try again.\n");
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
