#include "stdio.h"
#include "stdlib.h"
#include "dirent.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "fcntl.h"
#include "string.h"
#include "errno.h"
#include "time.h"
#include "libgen.h"
#include <unistd.h>
#include "fcntl.h"

const int BUFSIZE = 10000000;
 
typedef struct list {
  char *name;
  struct list *next;
} list;
 
int main(int argc, char **argv) {
    char *progName = basename(argv[0]);
    if (argc != 5) {
        fprintf(stderr, "%s: Wrong number of arguments\n", progName);
        return 1;
    }
    if (opendir(argv[1]) == NULL) {
        fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), argv[1]);
        return 1;
    }
    int resultFile;
    char *buf;
    buf = malloc(BUFSIZE);
    if ((resultFile = open(argv[4], O_TRUNC | O_RDWR)) == -1){
         fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), argv[4]);
         return 1;
    }
    struct dirent *file;
    struct stat statBuf;
    long int n1 = atoi(argv[2]); 
    long int n2 = atoi(argv[3]); 
    char* b1 = malloc(n2 + 1);
    char* b2 = malloc(n2 + 1);
    int i = 0; 
    char **directories;
    char *tempString;
    int start = 0;
    int end = 0;
    DIR *dir;
    list *files = NULL, *tempFiles = NULL, *currentFile = NULL;
    int f1, f2;
    directories = (char **) malloc(sizeof(char *));
    directories[0] = (char *) malloc ((strlen(argv[1])+1) * sizeof(char));
    strcpy(directories[0], argv[1]);

    while (start <= end) {
        if((dir = opendir(directories[start])) == NULL){
            fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), directories[start]);
        } else {
            errno = 0;
            while (1){ 
                if((file = readdir(dir) ) == NULL){
                    if(0 != errno){
                        fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), directories[start]);
                    }
                    break;
                }
                if ((strcmp(file -> d_name,".") != 0) && (strcmp(file -> d_name,"..") != 0)) {
                    tempString = (char *) malloc ((strlen(directories[start]) + strlen(file -> d_name) + 3) * sizeof(char));
                    strcpy(tempString, directories[start]);
                    strcat(tempString,"/"); strcat(tempString, file -> d_name); strcat(tempString,"\0");
                    if (lstat(tempString, &statBuf) == -1) {  
                        fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), tempString);       
                        free(tempString);
                        continue;
                    }
                    if (statBuf.st_mode & S_IFDIR) { //если папка
                        end++;
                        if (((directories = (char **) realloc(directories,(end + 1) * sizeof(char *))) == NULL) ||
                            ((directories[end] = (char *) malloc((strlen(tempString) + 1) * sizeof(char))) == NULL)) {
                                fprintf(stderr, "%s: %s\n", progName, strerror(errno));  
                            continue;
                        }
                        strcpy(directories[end], tempString);
                    }
                    if(!S_ISREG(statBuf.st_mode)){
                        continue;
                    }
                    if (statBuf.st_mode & S_IFREG)  //если файл
                        if (statBuf.st_size >= n1 && statBuf.st_size <= n2 && statBuf.st_size != 0) { //проверка размера
                            tempFiles = (list *) malloc(sizeof(sizeof(list *)));
                            tempFiles -> name = (char *) malloc((strlen(tempString) + 1) * sizeof(char));
                            strcpy(tempFiles -> name, tempString);
                            tempFiles -> next = files;
                            files = tempFiles;
                        }           
                    free(tempString);
                }
            }
        }
        if((closedir(dir) == -1) && 0 == errno){
            fprintf(stderr, "%s: %s, %s\n", progName, strerror(errno), directories[start]);
        }
        start++;
    }
    int isCorrect;
    size_t s1, s2;
    struct stat stbuf;
    int size;
    while (files) {
        currentFile = files;
        while (currentFile -> next) {
            tempFiles = currentFile -> next;
            if ((stat(files -> name, &stbuf) == -1) || (f1 = open(files -> name, O_RDONLY)) == -1) {
                fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), files -> name); 
                break;
            }
            size = stbuf.st_size;
            if ((stat(tempFiles -> name, &stbuf) == -1)) {
                fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), tempFiles-> name);
                currentFile -> next = tempFiles -> next;
                currentFile = currentFile -> next;
                if(close(f1)){
                    fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), files-> name);
                }
                continue;
            }
            if((f2 = open(tempFiles-> name, O_RDONLY)) == -1){
                fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), tempFiles-> name);
                currentFile -> next = tempFiles -> next;
                currentFile = currentFile -> next;
                if(close(f1)){
                    fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), files-> name);
                }
                continue;
            }
            if(size != stbuf.st_size){
                currentFile = currentFile -> next;
                if(close(f2)){
                    fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), files-> name);
                }
                if(close(f1)){
                    fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), files-> name);
                }
                continue;
            }
            if((s1 = read(f1, b1, n2)) == -1){
                fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), files-> name);
                if(close(f2)){
                    fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), files-> name);
                }
                break;
            }
            if((s2 = read(f2, b2, n2)) == -1){
                fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), tempFiles-> name);
                currentFile -> next = tempFiles -> next;
                currentFile = currentFile -> next;
                if(close(f1)){
                    fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), files-> name);
                }
                continue;
            }
            if (memcmp(b1, b2, s1)) {
                isCorrect = 0;
            } else{
                isCorrect = 1;
            }   

            if(isCorrect == 1){
                strcat(buf, files -> name); strcat(buf, " = ");
                strcat(buf, tempFiles -> name); strcat(buf, "\n");
                currentFile -> next = tempFiles -> next;
                free(tempFiles);
            } else { 
                currentFile = currentFile -> next;
            }
            if(close(f1)){
                fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), files-> name);
            }
            if(close(f2)){
                fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), tempFiles-> name);
            }
        }
        files = files -> next;
    }
    if(write(resultFile, buf, strlen(buf)) != strlen(buf)){
        fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), argv[4]);
        return 1; 
    }
    if(close(resultFile)){
        fprintf(stderr, "%s: %s: %s\n", progName, strerror(errno), argv[4]);
        return 1; 
    }
    for (i = 0; i < end; i++){
        free(directories[i]);
        directories[i] = NULL;
    }
    free(directories);
}