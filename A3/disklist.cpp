/*
 *  Rafael Solorzano
 *  V00838235
 *  CSC360 Assignment 3
 *
 */

#include "Constants.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>



int main(int argc, char** argv)
{

	FILE *fp;
	if ((fp=fopen(argv[1],"r")))
	{


	} else
		printf("Fail to open the image file.\n");
	free(name);
	fclose(fp);
	return 0;
}
