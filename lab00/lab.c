#include <stdio.h>
#include <stdlib.h> 
#include <locale.h>
#include <string.h>


int findNumDay (char word[20]){
    char day[3];
    day[0] = word[0];
    day[1] = word[1];
    int dayNum = atoi(day);

    char month[3];
    month[0] = word[3];
    month[1] = word[4];
    int monthNum = atoi(month);

    char year[5];
    year[0] = word[6];
    year[1] = word[7];
    year[2] = word[8];
    year[3] = word[9];
    int yearNum = atoi(year);

    return dayNum + monthNum * 30 + yearNum * 365;
}

int main(int argc, char *argv[]){
    unsigned char *words[argc - 2];
    for(int i = 0; i < argc - 2; i++){
        words[i] = (unsigned char *) argv[i];
    }
    setlocale(LC_ALL, "Russian");
    long long resultSum = 0;
    int num = atoi(argv[argc - 1]);
    for(int i = 1; i < 4; i++){
        for(int j = 0; j < strlen(argv[i]); j += 2){
            long long code = words[i][j];
            code *= 256;
            code += words[i][j + 1];
            printf("%llx \n", code);
            resultSum += code * code;
        }
    }
    printf("Sum of squares of codes: %llu\n", resultSum);
    int numDay = findNumDay(argv[argc - 2]);
    printf("NumDay: %d\n", numDay);
    resultSum *= numDay;
    printf("Product: %llu\n", resultSum);
    char strNum[50];
    sprintf(strNum, "%llu\n", resultSum);
    char strResultNum[4];
    int j = 0;
    int len = strlen(strNum) - 1;
    printf("Length: %d\n", len);
    for(int i = len/2 - 1; i <= len/2 + 1; i++){
        strResultNum[j] = strNum[i];
        j++;
    }
    printf("Average numbers: %s\n", strResultNum);
    int resultNum = atoi(strResultNum);
    int result = (resultNum % num) + 1;
    printf("Number of options: %d\n", num);
    printf("\nFinal result: %d\n", result);
    return 0;
}