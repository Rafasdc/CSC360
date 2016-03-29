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
#include <cstring>

int currentFile;

int findFreeBlock(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int val;
	int v = 512;
	int i = 0;
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
			printf("%d\n",i);
			return v;
		}
		i++;
	}
}

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
	fseek(fp,DIRECTORY_FILE_SIZE_OFFSET-1,SEEK_CUR);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	high = ((high_temp<<8)) + low_temp;
	//printf("%d\n",high);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	low = ((high_temp<<8)) + low_temp;
	//printf("%d\n",low);
	retVal = ((high<<8)) + low;
	return retVal;
}

void getName (FILE *fp, char *name){
	fseek(fp,DIRECTORY_FILENAME_OFFSET+currentFile,SEEK_SET);
	fread(name,1,30,fp);
	//printf("%*s ",DIRECTORY_MAX_NAME_LENGTH,name);
}

int getStartBlock(FILE *fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int retVal;
	fseek(fp,DIRECTORY_START_BLOCK_OFFSET+currentFile,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	high = ((high_temp<<8)) + low_temp;
	//printf("%d\n",high);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	low = ((high_temp<<8)) + low_temp;
	//printf("%d\n",low);
	retVal = ((high<<8)) + low;
	return retVal;
}

int getBlockSize(FILE * fp){
	int high;
	int low;
	unsigned char high_temp;
	unsigned char low_temp;
	int retVal;
	fseek(fp,DIRECTORY_BLOCK_COUNT_OFFSET+currentFile,SEEK_SET);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	high = ((high_temp<<8)) + low_temp;
	//printf("%d\n",high);
	fread(&high_temp,1,1,fp);
	fread(&low_temp,1,1,fp);
	low = ((high_temp<<8)) + low_temp;
	//printf("%d\n",low);
	retVal = ((high<<8)) + low;
	return retVal;
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
int getFileSize(FILE* fp){
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	return size;
}
void copyFile(FILE *fp, int startblock, int size, char* name){
	fseek(fp,startblock*512,SEEK_SET);
	FILE * out;
	out = fopen(name,"w");
	char test[4];
	int i = 0;
	while (1){
		fread(&test,1,1,fp);
		fwrite(test,1,1,out);
		//printf("%s",test);
		i++;
		if (i == size){
			break;
			fclose(out);
		}
	}
}

void copyIn(FILE *fp, char* name){
	FILE * in;
	in = fopen(name, "r");
	int size = getFileSize(in);
	int freeblock = findFreeBlock(fp);

}

int main(int argc, char** argv)
{

	int filetype;
	char *name = new char[DIRECTORY_MAX_NAME_LENGTH];
	FILE *fp;

	if (argv[2] == 0){
		printf("File not found\n");
		free(name);
		return 0;
	}

	if ((fp=fopen(argv[1],"rb+")))
	{
		currentFile = getRootDirStart(fp)*512;
		printf("File not found\n");
		int freeblock = findFreeBlock(fp);
		fseek(fp,freeblock-4,SEEK_SET);
		char hex[9];
		sprintf(hex,"%08x",freeblock);
		printf("%s\n", hex);
		const char a[] = {'R','R','R','R'};
		//printf("%d\n", freeblock);
		int c = 0xDEADBEEF;
		int b = htonl(c);
		fwrite(&b,1,4,fp);

		/*
		while (1){
			fseek(fp, currentFile,SEEK_SET);
			filetype = getFileType(fp);
			if (filetype == 3){
				//printf("F ");
			} else if (filetype == 5){
				//printf("D ");
			} else {
				//add file

			}
			getName(fp,name);
			currentFile+=64;
			if (strcmp(argv[2],name) == 0){
				printf("File already exists\n");
				break;
			}
		}
		*/
	} else
		printf("File not found\n");
	free(name);
	fclose(fp);
	return 0;
}
