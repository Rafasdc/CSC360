/*
 * assign2.c
 *
 * Name: Rafael Solorzano
 * Student Number: V00838235
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <string.h>
#include "train.h"

/*
 * If you uncomment the following line, some debugging
 * output will be produced.
 *
 * Be sure to comment this line out again before you submit 
 */

/* #define DEBUG	1 */

void ArriveBridge (TrainInfo *train);
void CrossBridge (TrainInfo *train);
void LeaveBridge (TrainInfo *train);
int arrivalPos = 0;
int turn = 0;
int east_waiting = 0;
int west_waiting = 0;
TrainInfo *list[256];



/*
 * This function is started for each thread created by the
 * main thread.  Each thread is given a TrainInfo structure
 * that specifies information about the train the individual 
 * thread is supposed to simulate.
 */
void * Train ( void *arguments )
{
	TrainInfo	*train = (TrainInfo *)arguments;

	/* Sleep to simulate different arrival times */
	usleep (train->length*SLEEP_MULTIPLE);



	ArriveBridge (train);
	CrossBridge  (train);
	LeaveBridge  (train); 


	/* I decided that the parameter structure would be malloc'd
	 * in the main thread, but the individual threads are responsible66f
	 * for freeing the memory.
	 *
	 * This way I didn't have to keep an array of parameter pointers
	 * in the main thread.
	 */
	free (train);
	return NULL;
}

/*
 * You will need to add code to this function to ensure that
 * the trains cross the bridge in the correct order.
 */
pthread_mutex_t bridge;
pthread_cond_t turns;


void ArriveBridge ( TrainInfo *train )
{
	printf ("Train %2d arrives going %s\n", train->trainId, 
			(train->direction == DIRECTION_WEST ? "West" : "East"));

	train->arrival = arrivalPos;
	list[arrivalPos] = train;
	arrivalPos++;

	if (train->direction == DIRECTION_WEST){
		west_waiting++;
	} else {
		east_waiting++;
	}


	pthread_mutex_lock(&bridge);

	while(train != list[turn]){
		pthread_cond_wait(&turns, &bridge);
	}
}

/*
 * Simulate crossing the bridge.  You shouldn't have to change this
 * function.
 */
void CrossBridge ( TrainInfo *train )
{
	printf ("Train %2d is ON the bridge (%s)\n", train->trainId,
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	fflush(stdout);
	
	/* 
	 * This sleep statement simulates the time it takes to 
	 * cross the bridge.  Longer trains take more time.
	 */
	usleep (train->length*SLEEP_MULTIPLE);

	printf ("Train %2d is OFF the bridge(%s)\n", train->trainId, 
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	fflush(stdout);
}

/*
 * Add code here to make the bridge available to waiting
 * trains...
 */
void LeaveBridge ( TrainInfo *train )
{
	/*
	int found = 0;
	TrainInfo* first_found;
	int first_found_pos;
	TrainInfo* second_found;
	int second_found_pos;
	if (train->direction == DIRECTION_WEST){
		west_waiting--;
		//find two or 1 east and shift array positions
		if (list[turn+1] != NULL){
			//TrainInfo* next = list[turn+1];

			int i = turn+1;
			while( i < arrivalPos && found < 2){
				printf("in big while\n");
				if (list[i]->direction == DIRECTION_EAST && found == 0){
					found++;
					first_found = list[i];
					first_found_pos = i;
					int a = i;
					printf("found one\n");
					while(a < arrivalPos){
						printf("Train %2d going east is waiting at position %d\n", list[a]->trainId, a);
						list[a] = list[a+1];
						a++;
						printf("whiles executed %d\n", a);
						printf("shift complete\n");
					}
					printf("Train %2d going east is waiting at position %d\n", list[i]->trainId, i);
					printf("shift complete\n");
					//i--;

				} else if (list[i]->direction == DIRECTION_EAST && found == 1){
					printf("in second if\n");
					found++;
					second_found = list[i];
					second_found_pos = i;
					int b = i;
					while(b < arrivalPos){
						list[b] = list[b+1];
						b++;
					}
					//i--;
				}
				i++;
			}
		}
	} else {
		east_waiting--;
	}
	printf("completed big while");
	if (found == 1){
		printf("in found 1\n");
		TrainInfo *list1[256];
		int a = 0;
		while(a < turn){
			list1[a] = list[a];
			a++;
		}
		list1[turn+1] = first_found;
		a = turn+2;
		while(a < arrivalPos){
			list1[a] = list[a-1];
			a++;
		}
		memcpy(list,list1,256);
	} else if (found == 2){
		TrainInfo *list2[256];
		int a = 0;
		while(a < turn){
			list2[a] = list[a];
			a++;
		}
		list2[turn+1] = first_found;
		list2[turn+2] = second_found;
		a = turn+3;
		while(a < arrivalPos){
			list2[a] = list[a-1];
			a++;
		}
		memcpy(list,list2,256);
	}
	*/
	turn++;
	pthread_mutex_unlock(&bridge);
	pthread_cond_broadcast(&turns);
}

int main ( int argc, char *argv[] )
{


	int		trainCount = 0;
	char 		*filename = NULL;
	pthread_t	*tids;
	int		i;

		
	/* Parse the arguments */
	if ( argc < 2 )
	{
		printf ("Usage: part1 n {filename}\n\t\tn is number of trains\n");
		printf ("\t\tfilename is input file to use (optional)\n");
		exit(0);
	}
	
	if ( argc >= 2 )
	{
		trainCount = atoi(argv[1]);
	}
	if ( argc == 3 )
	{
		filename = argv[2];
	}	
	
	initTrain(filename);
	
	/*
	 * Since the number of trains to simulate is specified on the command
	 * line, we need to malloc space to store the thread ids of each train
	 * thread.
	 */
	tids = (pthread_t *) malloc(sizeof(pthread_t)*trainCount);
	
	/*
	 * Create all the train threads pass them the information about
	 * length and direction as a TrainInfo structure
	 */
	for (i=0;i<trainCount;i++)
	{
		TrainInfo *info = createTrain();
		
		printf ("Train %2d headed %s length is %d\n", info->trainId,
			(info->direction == DIRECTION_WEST ? "West" : "East"),
			info->length );

		if ( pthread_create (&tids[i],0, Train, (void *)info) != 0 )
		{
			printf ("Failed creation of Train.\n");
			exit(0);
		}
	}

	/*
	 * This code waits for all train threads to terminate
	 */
	for (i=0;i<trainCount;i++)
	{
		pthread_join (tids[i], NULL);
	}
	
	free(tids);
	return 0;
}

