#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <syscall.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ARGS_COUNT 5
#define BUFSIZE 4096 * 4096




 int main(int argc, char* argv[])
 {
	printf("hello world");
	return 0;
 }