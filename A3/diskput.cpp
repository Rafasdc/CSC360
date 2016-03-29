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
#include <time.h>
#include <netinet/in.h>

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
			//printf("%d\n",i);
			return v;
		}
		i++;
	}
}

int findFreeBlockPos(FILE *fp){
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
			return i;
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

void getName (FILE *fp, char *name){
	fseek(fp,DIRECTORY_FILENAME_OFFSET+currentFile,SEEK_SET);
	fread(name,1,30,fp);
	//printf("%*s ",DIRECTORY_MAX_NAME_LENGTH,name);
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

int findRootDirFree(FILE *fp){
	currentFile = getRootDirStart(fp)*512;
	int filetype;
	while (1){
		fseek(fp, currentFile,SEEK_SET);
		filetype = getFileType(fp);
		if (filetype == 0){
			return currentFile;
		}
		currentFile += 64;
	}
}

void writeDirectoy(FILE *fp,int startingblock, int numblocks, int filesize,char *name){
	int freeDir = findRootDirFree(fp);
	int bi;
	//setfiletype
	fseek(fp,freeDir,SEEK_SET);
	int ft =3;
	fwrite(&ft,1,1,fp);
	//set starting block
	fseek(fp,freeDir+DIRECTORY_START_BLOCK_OFFSET,SEEK_SET);
	bi = htonl(startingblock);
	fwrite(&bi,1,4,fp);
	//set number of blocks
	fseek(fp,freeDir+DIRECTORY_BLOCK_COUNT_OFFSET,SEEK_SET);
	bi = htonl(numblocks);
	fwrite(&bi,1,4,fp);
	//set file size
	fseek(fp,freeDir+DIRECTORY_FILE_SIZE_OFFSET,SEEK_SET);
	bi = htonl(filesize);
	fwrite(&bi,1,4,fp);
	//set create time
	fseek(fp,freeDir+DIRECTORY_CREATE_OFFSET,SEEK_SET);
	//set modify time
	fseek(fp,freeDir+DIRECTORY_MODIFY_OFFSET,SEEK_SET);
	time_t rawtime;
	struct tm *info;
	time (&rawtime);
	info = localtime(&rawtime);
	int year = 0xE007; //the year is fixed but can be acquiered with tm_year+1900
	int month = info->tm_mon+1;
	int day = info->tm_mday;
	int hour = info->tm_hour;
	int min = info->tm_min;
	int sec = info->tm_sec;
	bi = year;
	fwrite(&bi,1,2,fp);
	bi = month;
	fwrite(&bi,1,1,fp);
	bi = day;
	fwrite(&bi,1,1,fp);
	bi = hour;
	fwrite(&bi,1,1,fp);
	bi = min;
	fwrite(&bi,1,1,fp);
	bi = sec;
	fwrite(&bi,1,1,fp);
	//set filename
	fseek(fp,freeDir+DIRECTORY_FILENAME_OFFSET,SEEK_SET);
	int zero = 0;
	fwrite(&zero,1,29,fp);
	fseek(fp,freeDir+DIRECTORY_FILENAME_OFFSET,SEEK_SET);
	fputs(name,fp);
	//set to 0x00000000000FF
	fseek(fp,freeDir+DIRECTORY_FILENAME_OFFSET+30,SEEK_SET);
	int unused = 0x00FF;
	bi = htonl(unused);
	fwrite(&bi,1,2,fp);
}

void writeFile(FILE *fp, FILE *in, int startingblock,int filesize){
	char test[4];
	int i = 0;
	fseek(fp,startingblock*512,SEEK_SET);
	while (1){
		fread(&test,1,1,in);
		fwrite(test,1,1,fp);
		//printf("%s",test);
		i++;
		if (i == filesize){
			break;
			fclose(in);
		}
	}
}

void copyIn(FILE *fp, char* name){
	FILE * in;
	if ((in = fopen(name, "r"))){
		int freeblock = findFreeBlock(fp);
		//printf("%d\n",freeblock);
		int freeblockpos = findFreeBlockPos(fp);
		int filesize = getFileSize(in);
		//printf("%d\n",filesize);
		if (filesize <= 512){
			fseek(fp,freeblock-4,SEEK_SET);
			int hex = 0xffffffff;
			fwrite(&hex,1,4,fp);
			//write directory
			writeDirectoy(fp,freeblockpos,1,filesize,name);
			//write file
			writeFile(fp,in,freeblockpos, filesize);
		} else {
			fseek(fp,freeblock-4,SEEK_SET);
			int numblocks = (filesize/512);
			if (filesize%512 != 0){
				numblocks++;
			}
			//printf("numblocks is %d\n",numblocks);
			int j = 0;
			int fbp = freeblockpos+1;
			while (j != numblocks-1){
				int bi = htonl(fbp);
				fwrite(&bi,1,4,fp);
				fbp++;
				j++;
			}
			int hex = 0xffffffff;
			fwrite(&hex,1,4,fp);
			//write directory
			writeDirectoy(fp,freeblockpos,numblocks,filesize,name);
			//write file
			writeFile(fp,in,freeblockpos, filesize);
		}
	} else {
		printf("File not found\n");
	}

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
		while (1){
			fseek(fp, currentFile,SEEK_SET);
			filetype = getFileType(fp);
			if (filetype == 3){
				//printf("F ");
			} else if (filetype == 5){
				//printf("D ");
			} else {
				//add file
				copyIn(fp,argv[2]);
				break;
			}
			getName(fp,name);
			currentFile+=64;
			if (strcmp(argv[2],name) == 0){
				printf("File already exists\n");
				break;
			}
		}

	} else
		printf("File not found\n");
	free(name);
	fclose(fp);
	return 0;
}
