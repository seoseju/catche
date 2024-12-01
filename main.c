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
    int value_returned = -1; /* accessed data */                        //반환받은 접근한 데이터를 저장할 value_returned 변수 초기화 

    /*check data by invoking check_cache_data_hit()*/
    printf("===== addr %d type %c ======\n", *(int *)addr, data_type);  //메인에서 받은 데이터 주소와 타입 확인용 코드 
    value_returned = check_cache_data_hit(addr, data_type);             //hit 검사를 위해 check_cache_data_hit() 호출하고 반환값을 value_returned에 저장한다

    /* In case of the cache miss event, retrieve data from the main memory  by invoking access_memory()*/

    if(value_returned == -1){                                           //miss로 인해 -1을 반환받은 경우  
        value_returned = access_memory(addr, data_type);                //메인 메모리에서 데이터 읽어오기 위해 access_memory()호출 

        /* If there is no data neither in cache nor memory, return -1, 
        else return data */

        if(value_returned == -1) return -1;                             //메모리에도 데이터가 없는 경우 -1을 반환한다
        
        /* access_memory 성공 !! */
        else return value_returned;                                     //메모리 접근 성공 시 메모리에서 찾은 데이터를 반환한다
    }
    /* hit 성공 !! */
    else return value_returned;                                         //hit 시 캐시 데이터를 반환한다
  
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
    //인풋파일에서 읽어온 byte address와 type, retrieve_data()에서 반환받은 accessed_data를 출력한다
    char s[1000] = "";
    fprintf(ofp,"[Accessed Data]\n");                   //아웃풋파일에 "[Accessed Data]" 출력

    for(int i=0;i<lineCount;i++){                       //줄 수 만큼 반복문을 돈다
        access_addr = strtoul(array[i], NULL, 10);      
        access_type = array[i][100];
        accessed_data = retrieve_data(&access_addr, access_type);   //retrieve_data()의 반환값을 accessed_data에 저장한다
        print_cache_entries();
        snprintf(s+strlen(s), sizeof(s) - strlen(s), "%lu\t%c\t%#x\n",access_addr,access_type,accessed_data);
    }

    // output파일 출력
    fprintf(ofp,"%s",s);                                //ofp에 s 출력
    fprintf(ofp,"----------------------------\n") ;     //oft에 "----------------------------"출력

 
    if(DEFAULT_CACHE_ASSOC  == 1)                                       //DEFAULT_CACHE_ASSOC = 1 (direct mapped cache)
        fprintf(ofp,"[Direct mapped Cache performance]\n");             //"[Direct mapped Cache performance]" 출력
    else if(DEFAULT_CACHE_ASSOC  == 2)                                  //DEFAULT_CACHE_ASSOC = 2 (2-way set associative cache)
        fprintf(ofp,"[2-way set associative Cache performance]\n");     //"[2-way set associative Cache performance]" 출력
    else if(DEFAULT_CACHE_ASSOC  == 4)                                  //DEFAULT_CACHE_ASSOC = 4 (fully associative cache)
        fprintf(ofp,"[Fully associative Cache performance]\n");         //"[Fully associative Cache performance]" 출력

    fprintf(ofp,"Hit ratio = %.2f  (%d/%d)\n", (float)num_cache_hits/global_timestamp, num_cache_hits,global_timestamp);   //hit ratio 출력 코드 (캐시 히트 횟수)/(캐시 접근 횟수)로 구한다 
    fprintf(ofp,"Bandwidth = %.2f  (%d/%d)\n", (float)num_bytes/num_access_cycles, num_bytes,num_access_cycles);           //bandwidth 출력 코드 (접근한 바이트 수)/(액세스 사이클 횟수)로 구한다

    fclose(ifp);        //인풋파일 닫기 
    fclose(ofp);        //아웃풋파일 닫기 
    
    return 0;
}