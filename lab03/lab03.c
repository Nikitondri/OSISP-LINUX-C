#define _GNU_SOURCE

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

FILE *resultFile;

typedef struct {
    int entries_count;
    ssize_t total_bytes;
} find_result_t;

char* execfile, *fullpath;
int max_num_proc, cur_num_proc;
char *BYTES_SEQUENCE;
char buffer[BUFSIZE];
char buf[BUFSIZE];
#define ERRORSIZE 512
char errorText[ERRORSIZE];


int open_dir(char*);
char* getfullname(char*,char*);
int getprocess(char*);
int find_bytes_in_file(const char*, const char*, find_result_t*);
void print_result(const char*, const find_result_t);
void printError(char*, const char*);


 int main(int argc, char* argv[])
 {
	execfile = basename(argv[0]);
	if (argc != ARGS_COUNT) {
		printError("Wrong number of parameters", NULL);
        return 1;
    }

    if ((max_num_proc = atoi(argv[3])) < 1) {
		printError("Proccess number must be bigger than 1", NULL);
        return 1;
    }
	if(strlen(argv[2]) >= 255){
		printError("M must be less than 255 bytes", NULL);
        return 1;
	}
	if ((resultFile = fopen(argv[4], "w")) == NULL){
		 printError(NULL, argv[4]);
         return 1;
    }
	setvbuf(resultFile, buf, _IOFBF, 10000);
 	char *dirname = argv[1];
	BYTES_SEQUENCE = argv[2];

	cur_num_proc = 1;
	open_dir(dirname);

	while(cur_num_proc != 0)
	{
		if(wait(NULL) == -1){
			if(errno == -1)
				printError(NULL, NULL);
			return 1;
		}
		cur_num_proc--;
	}
	if(fclose(resultFile) == EOF){
		printError(NULL, argv[4]);
        return 1; 
    }
	return 0;
 }

 char* getfullname(char* rootdirname, char* dirname)
 {
 	char* filepath = (char*)malloc(sizeof(char)*PATH_MAX);

 	strcpy(filepath,rootdirname);
 	strcat(filepath, "/");
 	strcat(filepath, dirname);
 	return filepath;
 } 

 int find_bytes_in_file(const char *filename, const char *bytes_sequence, find_result_t *find_result) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
		printError(NULL, filename);
        return -1;
    }
	struct stat statBuf;
	if (lstat(filename, &statBuf) == -1) {  
		printError(NULL, filename);
		return -1;
	}
    int entries_count = 0;
	int temp;
	while(1){
		if((temp = read(fd, &buffer, BUFSIZE)) == -1){
			printError(NULL, filename);
			return -1;
		};
		if(temp  ==  0){
			break;
		}
		if(memmem(buffer, BUFSIZE, bytes_sequence, strlen(bytes_sequence)) != NULL){
			++entries_count;
			break;
		}
	}

	if (close(fd) == -1) {
		printError(NULL, NULL);
		return -1;
    }	

    find_result->entries_count = entries_count;
    find_result->total_bytes = statBuf.st_size;

    return 0;
}


 void print_result(const char *filename, const find_result_t find_result) {
	fprintf(resultFile, "%d: %s: %ld\n", getpid(), filename, find_result.total_bytes);
}

 int getprocess(char* filepath)
 {
 	int result;
 	pid_t pid;
	find_result_t find_result;

 	if (cur_num_proc >= max_num_proc)
 	{
 		if ((result = wait(NULL)) > 0)
 			cur_num_proc--;
 		if (result == -1)
 		{
			printError(NULL, NULL);
 			exit(0);
 		}
 	}

 	if (cur_num_proc < max_num_proc)
 	{
 		pid = fork();
 		if ( pid < 0 ) {
			printError(NULL, NULL);
 			return 1;
 		}
 		if (pid == 0 ) {
			if (find_bytes_in_file(filepath, BYTES_SEQUENCE, &find_result) != -1) {
				if (find_result.entries_count != 0) {
					print_result(filepath, find_result);
				}
			};
 			exit(0); 			
 		}
 		if ( pid > 0 ) {
 			cur_num_proc++;
		}
 	}

 	return 0;
 }

 void printError(char* text, const char *fileName){
	if(fileName != NULL){
		snprintf(errorText, ERRORSIZE, "%d: %s: %s", getpid(), execfile, fileName);
	} else if(fileName != NULL) {
		snprintf(errorText, ERRORSIZE, "%d: %s: %s", getpid(), execfile, text);
	} else {
		snprintf(errorText, ERRORSIZE, "%d: %s", getpid(), execfile);
	}
	perror(errorText);
}

 int open_dir(char* dirname)
 {
 	DIR* dfd;
 	struct dirent *dir;
 	char* filepath;

 	if ((dfd=opendir(dirname)) == NULL)
 	{
		printError(NULL, dirname);
 		return 1;
 	}
 	errno = 0; 
 	while((dir = readdir(dfd)) != NULL)
 	{
 		filepath=getfullname(dirname, dir->d_name);
 		if (((dir->d_type == DT_DIR)) 
				&& ((strcmp(dir->d_name,".")) != 0) 
				&& ((strcmp(dir->d_name,"..")) != 0))
 			open_dir(filepath);
		else if (dir->d_type == DT_REG)
 			getprocess(filepath);
 		free(filepath);
 	}
	if(0 != errno){
		printError(NULL, dirname);
		return 1;
	}
 	if (closedir(dfd) != 0)
 	{
		printError(NULL, dirname);
 		return 1;
 	}
 	return 0;
 }