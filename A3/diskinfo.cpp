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

#define IDENT_SIZE 8

void getName(FILE *fp, char *name){
	fseek(fp,SEEK_SET,IDENT_OFFSET);
	fread(name,1,IDENT_SIZE,fp);
}

int getBlockSize(FILE *fp)
{
	unsigned char high;
	unsigned char low;
	int retVal;
	fseek(fp,BLOCKSIZE_OFFSET,SEEK_SET);
	fread(&high,1,1,fp);
	fread(&low,1,1,fp);
	retVal = ((high)<<8) + low;
	return retVal;
};

int getBlockCount(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int retVal;
	fseek(fp,BLOCKCOUNT_OFFSET,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	//printf("%d\n",high_temp);
	//printf("%d\n",low_temp);
	high = ((high_temp<<8)) + low_temp;
	//printf("%d\n",high);
	fseek(fp,BLOCKCOUNT_OFFSET+2,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	//printf("%d\n",high_temp);
	//printf("%d\n",low_temp);
	low = ((high_temp<<8)) + low_temp;
	//printf("%d\n",low);
	retVal = ((high<<8)) + low;
	return retVal;
};

int getFATstart(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int retVal;
	fseek(fp,FATSTART_OFFSET,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	high = ((high_temp<<8)) + low_temp;
	fseek(fp,FATSTART_OFFSET+2,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	low = ((high_temp<<8)) + low_temp;
	retVal = ((high<<8)) + low;
	return retVal;
};

int getFATblocks(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int retVal;
	fseek(fp,FATBLOCKS_OFFSET,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	high = ((high_temp<<8)) + low_temp;
	fseek(fp,FATBLOCKS_OFFSET+2,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	low = ((high_temp<<8)) + low_temp;
	retVal = ((high<<8)) + low;
	return retVal;
};

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

int getFreeBlocks(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int val;
	int retVal = 0;
	int v = DEFAULT_BLOCK_SIZE;
	while (1){
		fseek(fp,v,SEEK_SET);
		v+= 4;
		fread(&high_temp,1,1,fp);
		fread(&low_temp,1,1,fp);
		high = ((high_temp<<8)) + low_temp;
		fread(&high_temp,1,1,fp);
		fread(&low_temp,1,1,fp);
		low = ((high_temp<<8)) + low_temp;
		val = ((high<<8)) + low;
		if (val == FAT_FREE){
			retVal++;
		}
		if (v == (getFATblocks(fp)+1)*512){ //may need +1
			break;
		}
	}

	return retVal;
}

int getReservedBlocks(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int val;
	int retVal = 0;
	int v = 512;
	while (1){
		fseek(fp,v,SEEK_SET);
		v+= 4;
		fread(&high_temp,1,1,fp);
		fread(&low_temp,1,1,fp);
		high = ((high_temp<<8)) + low_temp;
		fread(&high_temp,1,1,fp);
		fread(&low_temp,1,1,fp);
		low = ((high_temp<<8)) + low_temp;
		val = ((high<<8)) + low;
		if (val == FAT_RESERVED){
			retVal++;
		}
		if (v == (1+getFATblocks(fp))*512){ //may need +1
			break;
		}
	}

	return retVal;
}

int getAllocatedBlocks(FILE *fp){
	int  bc = getBlockCount(fp);
	int  rb = getReservedBlocks(fp);
	int  fb = getFreeBlocks(fp);
	return bc - rb - fb;
}

int main(int argc, char** argv)
{
	FILE *fp;
	char *name = new char[IDENT_SIZE];
	int size;
	int blockcount;
	int FATstarts;
	int FATblocks;
	int rootdirstart;
	int rootdirblocks;
	int freeblocks;
	int reservedblocks;
	int allocatedblocks;
	if ((fp=fopen(argv[1],"r")))
	{
		printf("Super block information: \n");
		//getName(fp,name);
		//printf("File system identifier: %s\n", name);
		size = getBlockSize(fp);
		printf("Block size: %d\n", size);
		blockcount = getBlockCount(fp);
		printf("Block count: %d\n", blockcount);
		FATstarts = getFATstart(fp);
		printf("FAT starts: %d\n", FATstarts);
		FATblocks = getFATblocks(fp);
		printf("FAT blocks: %d\n", FATblocks);
		rootdirstart = getRootDirStart(fp);
		printf("Root directory start: %d\n", rootdirstart);
		rootdirblocks = getRootDirBlocks(fp);
		printf("Root directory blocks: %d\n", rootdirblocks);
		printf("\nFAT information: \n");
		freeblocks = getFreeBlocks(fp);
		printf("Free Blocks: %d\n", freeblocks);
		reservedblocks = getReservedBlocks(fp);
		printf("Reserved Blocks: %d\n", reservedblocks);
		allocatedblocks = getAllocatedBlocks(fp);
		printf("Allocated Blocks: %d\n", allocatedblocks);

	} else
		printf("Fail to open the image file.\n");
	free(name);
	fclose(fp);
	return 0;
}
