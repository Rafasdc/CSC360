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

int getRootDirStart(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int retVal;
	fseek(fp,ROOTDIRSTART_OFFSET,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	high = ((high_temp<<8)) + low_temp;
	fseek(fp,ROOTDIRSTART_OFFSET+2,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	low = ((high_temp<<8)) + low_temp;
	retVal = ((high<<8)) + low;
	return retVal;
};

int getRootDirBlocks(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int retVal;
	fseek(fp,ROOTDIRBLOCKS_OFFSET,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	high = ((high_temp<<8)) + low_temp;
	fseek(fp,ROOTDIRBLOCKS_OFFSET+2,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	low = ((high_temp<<8)) + low_temp;
	retVal = ((high<<8)) + low;
	return retVal;
};

void getFile(FILE *fp){
	unsigned char type;
	//if 03 is file if 05 is directory becaus eof bits set in binary
	fread(&type,1,1,fp);
	printf("%d\n",type);
}

int main(int argc, char** argv)
{

	FILE *fp;
	if ((fp=fopen(argv[1],"r")))
	{
		fseek(fp, getRootDirStart(fp)*512,SEEK_SET);
		getFile(fp);

	} else
		printf("Fail to open the image file.\n");
	fclose(fp);
	return 0;
}
