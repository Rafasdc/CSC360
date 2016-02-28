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
#include <vector>
extern "C"{
#include "train.h"
}

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
int east_crossed = 0;
//TrainInfo *list[256];
std::vector<TrainInfo *>list;



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



	train->arrival = arrivalPos;
	list.push_back(train);
	ArriveBridge (train);
	CrossBridge  (train);
	LeaveBridge  (train); 
	//printf("train %d just finished\n", train->trainId);


	/* I decided that the parameter structure would be malloc'd
	 * in the main thread, but the individual threads are responsible66f
	 * for freeing the memory.
	 *
	 * This way I didn't have to keep an array of parameter pointers
	 * in the main thread.
	 */
	//printf("before free train\n");
	free (train);
	return NULL;
}

/*
 * You will need to add code to this function to ensure that
 * the trains cross the bridge in the correct order.
 */
pthread_mutex_t bridge = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t turns = PTHREAD_COND_INITIALIZER;


void ArriveBridge ( TrainInfo *train )
{

	printf ("Train %2d arrives going %s\n", train->trainId,
			(train->direction == DIRECTION_WEST ? "West" : "East"));


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
	if (train->direction == DIRECTION_EAST){
		east_waiting--;
		if(west_waiting >0){
			east_crossed++;
		}
	} else if (train->direction == DIRECTION_WEST /*|| east_crossed == 2*/) {
		west_waiting--;
		//east_crossed = 0;
	}
	//printf("East crossed %d\n", east_crossed);
	//printf("There are %d east trains waiting\n", east_waiting);
	//printf("in leave bridge \n");
	int found = 0;
	TrainInfo* firstFound;
	TrainInfo* secondFound;
	if (train->direction == DIRECTION_WEST && east_waiting > 0){
		for(int i = turn+1; i < list.size(); i++){
			if (list[i]->direction == DIRECTION_EAST){
				if (found == 0){
					found++;
					firstFound = list[i];
					list.erase(list.begin()+i);
					list.insert(list.begin()+turn+1, firstFound);
				} else if (found == 1){
					found++;
					secondFound = list[i];
					list.erase(list.begin()+i);
					list.insert(list.begin()+turn+2,secondFound);
					break;
				}
			}
		}
		if (found == 1){

		} else if (found == 2){

		}
	} else if (east_waiting > 0 && east_crossed < 2){
		TrainInfo* temp;
		for(int i = turn+1; i < list.size(); i++){
			if(list[i]->direction == DIRECTION_EAST){
				temp = list[i];
				list.erase(list.begin()+i);
				list.insert(list.begin()+turn+1, temp);
				break;
			}
		}

	} else if (west_waiting > 0 && east_crossed == 2){
		east_crossed = 0;
		TrainInfo* temp;
		for(int i = turn+1; i < list.size(); i++){
			if(list[i]->direction == DIRECTION_WEST){
				temp = list[i];
				list.erase(list.begin()+i);
				list.insert(list.begin()+turn+1, temp);
				break;
			}
		}
	}



//	printf("train %d just before unlocking mutex\n", train->trainId);
	turn++;

	//printf("train %d broadcasted\n", train->trainId);

	pthread_cond_broadcast(&turns);
	pthread_mutex_unlock(&bridge);

	//printf("train %d unlocked mutex\n", train->trainId);


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
		//printf("waiting on %d\n", i);
	}
	
	//printf("before free tids\n");
	free(tids);
	return 0;
}

