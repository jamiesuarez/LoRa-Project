#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){

FILE * fd_log;

if ((fd_log = fopen("./channel10.log", "a+")) < 0) {
	perror("log file:");
	exit(1);
}

fprintf(fd_log, "toto\n");

fclose(fd_log);

}
