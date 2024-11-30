/*
 * main.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implementation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 17, 2024
 *
 */

#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include "cache_impl.h"
#include "cache.c"

int num_cache_hits = 0;
int num_cache_misses = 0;

int num_bytes = 0;
int num_access_cycles = 0;

int global_timestamp = 0;

int retrieve_data(void *addr, char data_type) {
    int value_returned = -1; /* accessed data */

    /*check data by invoking check_cache_data_hit()*/
    printf("===== addr %d type %c ======\n", *(int *)addr, data_type);
    value_returned = check_cache_data_hit(addr, data_type); 

    /* In case of the cache miss event, retrieve data from the main memory  by invoking access_memory()*/

    if(value_returned == -1){
        value_returned = access_memory(addr, data_type); 

        /* If there is no data neither in cache nor memory, return -1, 
        else return data */

        if(value_returned == -1) return -1; 
        
        /* access_memory 성공 !! */
        else return value_returned; 
    }
    /* hit 성공 !! */
    else return value_returned; 
  
}

int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr; /* byte address (located at 1st column) in "access_input.txt" */
    char access_type; /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data; /* This is the data that you want to retrieve first from cache, and then from memory */ 
    
    init_memory_content();
    init_cache_content();
    
    ifp = fopen("access_input2.txt", "r"); //ifp = 인풋파일을 '읽기모드'로 열었음 
    if (ifp == NULL) {
        printf("Can't open input file\n");
        return -1;
    }
    ofp = fopen("access_output.txt", "w"); //ofp = 아웃풋파일을 쓰기 모드로 열었음 
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }
    
    /* Fill out here by invoking retrieve_data() */

    // 인풋파일 뜯어서 array 이차원 배열에 저장 
    // array[0][1] : 첫번째 줄의 두번째 단어 == b

    char line[101]; //인풋파일 줄 나눠 넣을 배열
    char *array[100]; //쪼개진 단어 넣을 배열
    int lineCount=0; //줄 수 세기

    while(fgets(line, sizeof(line), ifp)!=NULL && lineCount < 100){
        array[lineCount] = malloc(sizeof(char)*101);
        char *token = strtok(line, " \n");
        int tokenIndex = 0;
        while(token!=NULL&&tokenIndex<2){
            if(tokenIndex==0)
                strncpy(array[lineCount], token, 101);
            else if(tokenIndex==1)
                array[lineCount][100] = token[0];
            token=strtok(NULL, " \n");
            tokenIndex++;
        }
        lineCount++;
    }

    // array[i][0]으로 access_addr 정하고
    // array[i][1]로 type읽어서 access_type 정하기
     
    char s[1000] = "";
    fprintf(ofp,"[Accessed Data]\n");

    for(int i=0;i<lineCount;i++){
        access_addr = strtoul(array[i], NULL, 10);
        access_type = array[i][100];
        accessed_data = retrieve_data(&access_addr, access_type);
        print_cache_entries();
        snprintf(s+strlen(s), sizeof(s) - strlen(s), "%d\t%c\t%#x\n",access_addr,access_type,accessed_data);
    }

    // output파일 출력
    fprintf(ofp,s); 
    fprintf(ofp,"----------------------------\n") ;

    // DEFAULT_CACHE_ASSOC에 따라서 이름바꾸기 
    if(DEFAULT_CACHE_ASSOC  == 1)
        fprintf(ofp,"[Direct mapped Cache performance]\n");
    else if(DEFAULT_CACHE_ASSOC  == 2)
        fprintf(ofp,"[2-way set associative Cache performance]\n");
    else if(DEFAULT_CACHE_ASSOC  == 4)
        fprintf(ofp,"[Fully associative Cache performance]\n");

    fprintf(ofp,"Hit ratio = %.2f  (%d/%d)\n", (float)num_cache_hits/global_timestamp, num_cache_hits,global_timestamp); 
    fprintf(ofp,"Bandwidth = %.2f  (%d/%d)\n", (float)num_bytes/num_access_cycles, num_bytes,num_access_cycles); 

    fclose(ifp);
    fclose(ofp);
    
    return 0;
}