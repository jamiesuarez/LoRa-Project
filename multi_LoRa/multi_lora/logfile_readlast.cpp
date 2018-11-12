#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int channel;
unsigned int lastMessageIndex;
unsigned int currentIndex=0;
FILE* fd_log=NULL;
FILE* fd_index=NULL;

char channelString[2]="";
char  logfile[255]="/tmp/channel";
char  indexfile[255]="/tmp/channel";

char  line[255] = "";

int main(int argc, char **argv){

if (argc != 2) {
	fprintf(stderr, "usage: %s <channel number>\n", argv[0]);
	exit(EXIT_FAILURE);
}

sscanf(argv[1], "%d", & channel);

sprintf(channelString, "%d",channel);
strcat(logfile, channelString);
strcat(logfile, ".log"); 

strcat(indexfile, channelString);
strcat(indexfile, ".index"); 

//Open the log file & index file
if ((fd_log = fopen(logfile, "r")) < 0) {
	perror(logfile);
	exit(EXIT_FAILURE);
}

if ((fd_index = fopen(indexfile, "r")) < 0) {
	perror(indexfile);
	exit(EXIT_FAILURE);
}


fscanf(fd_index, "%09d", &lastMessageIndex);

fgets(line, 255, fd_log);

sscanf(line, "%09d", &currentIndex);


while(currentIndex != lastMessageIndex){
	fgets(line, 255, fd_log);
	sscanf(line, "%09d", &currentIndex);
}


fprintf(stdout, "%s", line); // line already contains \n
//fprintf(stdout, "%09d\n", currentIndex);

while(fgets(line, 255, fd_log) != NULL){

	fprintf(stdout, "%s", line);
}


fclose(fd_log);
fclose(fd_index);
} // end main
