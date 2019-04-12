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

int calcTotalProcessTime(int data[][2],int col) {
	int result=0;
	for (int i=0; i < col; i++) {
		result = result + data[i][1];
	}
	return result;
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
	int testData[5][2] = {{0,3},{2,6},{6,5},{8,2},{4,4}};
	int testData2[3][2]={{0,6},{3,5},{7,2}};
	calcFCFS(testData,5);
	//calcRR(testData, 5, 1);
	
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


int* calcFCFS(int data[][2], int col) {
	
	int tempData[col][2];
	int tempX, tempY;
	for (int i = 0; i < col; i++) {
		for (int j = 0; j < 2; j++) {
			tempData[i][j] = data[i][j];
		}
	}
	for(int j=col-1;j>0;j--){
		for (int i = 0; i < col-1; i++) {
			if (tempData[i][0] > tempData[i + 1][0]) {
				tempX = tempData[i][0];
				tempY = tempData[i][1];
				tempData[i][0] = tempData[i + 1][0];
				tempData[i][1] = tempData[i + 1][1];
				tempData[i + 1][0] = tempX;
				tempData[i + 1][1] = tempY;
			}
		}
	}
	for (int i = 0; i < col; i++) {
		printf("{ %d,%d } \n", tempData[i][0], tempData[i][1]);
	}
	
	int totalProcessTime=calcTotalProcessTime(tempData,col);
	int resultData[totalProcessTime];
	int i = 0;
	struct Queue q;
	qInit(&q, totalProcessTime);
	for (int i = 0; i < col; i++) {
		for (int j = 0; j < tempData[i][1]; j++) {
			qPush(&q, i);
		}
	}
	while (1)
	{
		if (qSize(&q) == 0)
			break;
		resultData[i]=qPop(&q);
		i++;
	}
	for(int i=0;i<totalProcessTime;i++){
			printf("%d. ", resultData[i]);
	}	
	return resultData;
}

int* calcRR(int data[][2], int col, int timeQuantum) {
	
	//값이 온 순서대로 정렬 
	int tempData[col][2];
	int tempX, tempY;
	for (int i = 0; i < col; i++) {
		for (int j = 0; j < 2; j++) {
			tempData[i][j] = data[i][j];
		}
	}
	for(int j=col-1;j>0;j--){
		for (int i = 0; i < col-1; i++) {
			if (tempData[i][0] > tempData[i + 1][0]) {
				tempX = tempData[i][0];
				tempY = tempData[i][1];
				tempData[i][0] = tempData[i + 1][0];
				tempData[i][1] = tempData[i + 1][1];
				tempData[i + 1][0] = tempX;
				tempData[i + 1][1] = tempY;
			}
		}
	}
	for (int i = 0; i < col; i++) {
		printf("{ %d,%d } \n", tempData[i][0], tempData[i][1]);
	}
	

	int totalProcessTime=calcTotalProcessTime(tempData,col);
	printf("총 실행시간  %d\n",totalProcessTime); 
	int resultData[totalProcessTime];
	int realTime=0;
	int temp=-1;
	int leftServiceTime=0;
	struct Queue q;
	qInit(&q, totalProcessTime);
	int serviceData[col];
	int checkProcess[col];
	for(int i=0;i<col;i++){
		serviceData[i]=tempData[i][1];
		checkProcess[i]=0;
	}
	for(int i=0;i<col;i++){
			leftServiceTime+=serviceData[i];
	}//실행정보
	while(1){	
		printf("현재시간 %d 초 \n",realTime); 
		printf("남은시간 %d\n",leftServiceTime);
		for(int i=0;i<col;i++){
			if(realTime>=tempData[i][0] && checkProcess[i]==0){//도달한 프로세스면 시간만큼 넣어주세요. 도달했는데 이미 넣었던 친구들은 넣지마세요. 
				for(int j=0; j<serviceData[i];j++){
						qPush(&q,i); printf("큐에 %d 넣는중 \n",i );
				}
				if(temp==q.data[q.rear+1]){//작업이 남아있는 
					for(int k=0; k<serviceData[temp];k++){
						qPush(&q,qPop(&q));
					}
				}
				checkProcess[i]=1;
			}
		}
		if(temp==q.data[q.rear+1]){//작업이 남아있는 
				for(int w=0; w<serviceData[temp];w++){
				qPush(&q,qPop(&q));
			}
		}
		printf("--------------------------------------------------------------------\n");
		qPrint(&q); 
		for(int i=0; i<timeQuantum;i++){// 주어진 시간만큼 실행
			if(qSize(&q)==0)
				break;
			leftServiceTime--;
			temp=qPop(&q);										printf("%d 팝\n",temp); 
			resultData[realTime]=temp;							
			serviceData[temp]--;
			realTime++;								printf("%d 번 프로세스가 남은 시간 : %d\n 다음 작업 : %d\n",temp,serviceData[temp],q.data[q.rear+1]);
			if(temp!=q.data[q.rear+1] ){//다음 원소가 내가 아니면 끝난거니 빠져나오세요.							
				break;
			}
		}
		if(leftServiceTime==0)
			break;
	}
	for(int i=0;i<totalProcessTime;i++){
			printf("%d. ", resultData[i]);
	}	
	return resultData;
}

	//	else if(){
		//		if(temp==q.data[q.rear+1]){//작업이 남아있는 
		//			for(int w=0; w<serviceData[temp];w++){
		//				qPush(&q,qPop(&q));
		//			}
		//		}	 
		//	}





































