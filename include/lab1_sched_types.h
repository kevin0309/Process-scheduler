/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 
*	    Student name : 
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H


/*
 * You need to Declare functions in  here
 */

int* calcFCFS(int data[][2], int col, int *resSize);
int* calcRR(int data[][2], int col, int timeQuantum, int *resSize);
int* calcMLFQ(int data[][2], int col, int timeQuantum, int *resSize);
int* calcLottery(int data[][2], int col, int *resSize);

void printResult(int inputData[][2], int resData[], int col, int resSize);


#endif /* LAB1_HEADER_H*/



