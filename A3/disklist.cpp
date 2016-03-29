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

int currentFile;

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


int getFileType(FILE *fp){
	unsigned char type;
	//if 03 is file if 05 is directory becaus eof bits set in binary
	fread(&type,1,1,fp);
	//printf("%d\n",type);
	return type;
}
int getSize(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int retVal;
	fseek(fp,DIRECTORY_FILE_SIZE_OFFSET+currentFile,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	high = ((high_temp<<8)) + low_temp;
	//printf("%d\n",high);
	fseek(fp,DIRECTORY_FILE_SIZE_OFFSET+currentFile+2,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	low = ((high_temp<<8)) + low_temp;
	//printf("%d\n",low);
	retVal = ((high<<8)) + low;
	return retVal;
}

void getName (FILE *fp){
	char name[DIRECTORY_MAX_NAME_LENGTH];
	fseek(fp,DIRECTORY_FILENAME_OFFSET+currentFile,SEEK_SET);
	fread(&name,1,31,fp);
	printf("%*s ",DIRECTORY_MAX_NAME_LENGTH,name);
}

void getModified (FILE *fp){
	fseek(fp,DIRECTORY_MODIFY_OFFSET+currentFile,SEEK_SET);
	unsigned char high;
	unsigned char low;
	int YYYY;
	int MM;
	int DD;
	int HH;
	int mm;
	int SS;
	fread(&high,1,1,fp);
	fread(&low,1,1,fp);
	YYYY = ((high)<<8) + low;
	fread(&MM,1,1,fp);
	fread(&DD,1,1,fp);
	fread(&HH,1,1,fp);
	fread(&mm,1,1,fp);
	fread(&SS,1,1,fp);
	printf("%d/%02d/%02d %02d:%02d:%02d\n",YYYY, MM, DD, HH, mm, SS);
}

int main(int argc, char** argv)
{

	int filetype = 1;
	int size;
	FILE *fp;

	if ((fp=fopen(argv[1],"r")))
	{
		currentFile = getRootDirStart(fp)*512;
		while (1){
			fseek(fp, currentFile,SEEK_SET);
			filetype = getFileType(fp);
			if (filetype == 3){
				printf("F ");
			} else if (filetype == 5){
				printf("D ");
			} else {
				break;
			}
			size = getSize(fp);
			printf("%*d ",10,size);
			getName(fp);
			getModified(fp);
			currentFile+=64;
		}
	} else
		printf("Fail to open the image file.\n");
	fclose(fp);
	return 0;
}
