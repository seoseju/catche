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
#include <string.h> // 내가 했어..
#include <stdlib.h> //내가 했어..
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
        num_access_cycles+=100;
        value_returned = access_memory(addr, data_type); 

        /* If there is no data neither in cache nor memory, return -1, 
        else return data */
        if(value_returned == -1){
            return -1; 
        }
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
    ofp = fopen("^^access_output.txt", "w"); //ofp = 아웃풋파일을 쓰기 모드로 열었음 
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }
    
    /* Fill out here by invoking retrieve_data() */

    //인풋 파일에서 행 수 읽기 .. 하... 그냥 "많아봤자 10행이겠지" 생각해서 max_lines = 10으로 했음..
    //ㅋㅋㅋㅋㅋㅋㅋ
    //NULL로 수정할 수 잇을까

    // 인풋파일 뜯어서 array 이차원 배열에 저장 
    // array[0][1] : 첫번째 줄의 두번째 단어 == b

    char array[10][10][10];     //[max_lines][max_word][max_length]: [최대 줄 수][최대 단어 수][단어의 최대 길이]
    char line[20];          // 한 줄을 저장할 배열 
    int lineCount =0 ;      //읽은 줄 수
    int wordCounts[10] = {0};
    (void)wordCounts;

    while(fgets(line, sizeof(line), ifp) && lineCount < 10){
    
        line[strcspn(line, "\n")] = '\0';  // 줄 끝의 \n제거

        char *word = strtok(line, " "); //공백으로 단어 분리 
        int wordIndex = 0; 
        while(word != NULL && wordIndex < 2){
           
            strncpy(array[lineCount][wordIndex], word, 9); 
            array[lineCount][wordIndex][9] = '\0'; 
            word = strtok(NULL, " "); 
            wordIndex ++;

           
        }
        wordCounts[lineCount] = wordIndex; 
        lineCount ++; 
    }

    // array[i][1] 로 type읽어서 access_type 정하고 array[i][0]으로 access_addr 정하기 

    char s[1000]; 
    
    fprintf(ofp,"[Accessed Data]\n");
    for(int i=0; i<10; i++){

        if(array[i][1][0] == '\0')
            break; 
        else {
            access_addr = (unsigned long)atoi(array[i][0]); // char 형 배열이라서 atoi()함수로 int형전환을 해줌 
            access_type = array[i][1][0]; // [i][1][0] 이 들어가야 단어의 첫번째 char을 반환한대.. array를 이차원으로 했더니, overflow가 발생할까봐 안된다네..?
            accessed_data = retrieve_data(&access_addr, access_type);
            print_cache_entries();
            snprintf(s+strlen(s) , sizeof(s)-strlen(s),"%d\t%c\t%#x\n",access_addr,access_type,accessed_data); 
        
            printf("잘됐어요 : \n%s",s); 
            //디버깅용 프린트
            printf("%#x\n", accessed_data);
            

            
        }
    }

    // output파일 출력
    fprintf(ofp,s); 
    fprintf(ofp,"----------------------------\n") ;

    // cache_set_size에 따라서 이름 바꾸게 했어 
    if(DEFAULT_CACHE_ASSOC  == 1)
        fprintf(ofp,"[Direct mapped Cache performance]\n");
    else if(DEFAULT_CACHE_ASSOC  == 2)
        fprintf(ofp,"[2-way set associative Cache performance]\n");
    else if(DEFAULT_CACHE_ASSOC  == 4)
        fprintf(ofp,"[Fully associative Cache performance]\n");


    
    fprintf(ofp,"cache hit = %d  \n", num_cache_hits); 
    fprintf(ofp,"cache hit = %d\n", num_access_cycles); 
    fprintf(ofp,"Hit ratio = %.2f  (%d/%d)\n", (float)num_cache_hits/global_timestamp, num_cache_hits,global_timestamp); 
    fprintf(ofp,"Bandwidth = %.2f  (%d/%d)\n", (float)num_bytes/num_access_cycles, num_bytes,num_access_cycles); 


    printf("캐시 히트:%d",num_cache_hits); 
    
    fclose(ifp);
    fclose(ofp);
    
    print_cache_entries();
    //system("pause");
    return 0;
}