/*
 * cache.c
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
#include "cache_impl.h"

extern int num_cache_hits;
extern int num_cache_misses;

extern int num_bytes;
extern int num_access_cycles;

extern int global_timestamp;

cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];
int memory_array[DEFAULT_MEMORY_SIZE_WORD];


/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_memory_content() {
    unsigned char sample_upward[16] = {0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef};
    unsigned char sample_downward[16] = {0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010};
    int index, i=0, j=1, gap = 1;
    
    for (index=0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {
        memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);
        if (++i >= 16)
            i = 0;
        if (++j >= 16)
            j = 0;
        
        if (i == 0 && j == i+gap)
            j = i + (++gap);
            
        printf("mem[%d] = %#x\n", index, memory_array[index]);
    }
}   

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_cache_content() {
    int i, j;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        for (j=0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            pEntry->valid = 0;
            pEntry->tag = -1;
            pEntry->timestamp = 0;
        }
    }
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
/* This function is a utility function to print all the cache entries. It will be useful for your debugging */
void print_cache_entries() {
    int i, j, k;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        printf("[Set %d] ", i);
        for (j=0; j <DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            printf("V: %d Tag: %#x Time: %d Data: ", pEntry->valid, pEntry->tag, pEntry->timestamp);
            for (k=0; k<DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {
                printf("%#x(%d) ", pEntry->data[k], k);
            }
            printf("\t");
        }
        printf("\n");
    }
}

int check_cache_data_hit(void *addr, char type) {

    /* Fill out here */
    //여기 constant 변수명으로 수정함
    int blockAddr = (*(int *)addr) / DEFAULT_CACHE_BLOCK_SIZE_BYTE; 
    int cache_index = blockAddr % CACHE_SET_SIZE; 
    int tag= blockAddr / CACHE_SET_SIZE; 
    int byte_offset = (*(int *)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE; 
    //int word_index = (*(int *)addr) / CACHE_SET_SIZE; 

    printf("CACHE >> block_addr = %d, byte_offset = %d, cache_index = %d, tag = %d\n",
                   blockAddr, byte_offset, cache_index, tag);

    num_access_cycles++; // 내가추가햇삼 >> 고마어 ㅎㅎㅎㅎ

    
    for(int i =0; i<DEFAULT_CACHE_ASSOC; i++){
         cache_entry_t *cache = &cache_array[cache_index][i]; 

        if(cache->valid == 1 && cache->tag == tag){
            printf("=> Hit!\n");

            cache->timestamp = global_timestamp++;
            num_cache_hits ++;
            cache->tag =  tag;
            printf("캐시 태그는: %d", cache->tag);  

            if(type=='b'){
                return cache->data[byte_offset]; 
            }
            else if(type == 'h'){

                return cache ->data[byte_offset]| (cache ->data[byte_offset +1] <<8); 
            }
            else if(type == 'w'){
                return cache ->data[byte_offset] |(cache ->data[byte_offset+1] << 8) |(cache ->data[byte_offset+2] << 16) |(cache ->data[byte_offset+3] << 24) ;
            }
        }
        
    }
    num_cache_misses++;
    printf("=> Miss!\n");
    
    return -1; 
    
}
// This function is to find the entry index in set for copying to cache
int find_entry_index_in_set(int cache_index) {
    int entry_index=0;
    
    // set 어쩌고에서는 cache_index=집합 번호
    /* Check if there exists any empty cache space by checking 'valid' */
    for(int i=0;i<DEFAULT_CACHE_ASSOC;i++){
        if(cache_array[cache_index][i].valid==0)
            return entry_index = i;

    }
        
    /* If the set has only 1 entry, return index 0 */
    /* Otherwise, search over all entries
    to find the least recently used entry by checking 'timestamp' */
    if(CACHE_SET_SIZE == 1) entry_index = 0;
    else{
        int min_timestamp = cache_array[cache_index][0].timestamp;
        for(int i=1;i<DEFAULT_CACHE_ASSOC;i++) {
            if(min_timestamp > cache_array[cache_index][i].timestamp){
                min_timestamp = cache_array[cache_index][i].timestamp;
                entry_index = i;
            }
        }
    }

    /* return the cache index for copying from memory*/
    return entry_index; 
}

int access_memory(void *addr, char type) {
    int blockAddr = (*(int *)addr) / DEFAULT_CACHE_BLOCK_SIZE_BYTE; 
    int cache_index = blockAddr % CACHE_SET_SIZE;
    int word_index = (*(int *)addr) / WORD_SIZE_BYTE;
    //이거는 memory에서 바로 리턴할 경우...
    //이름을 word_offset으로 하는 게 맞을지 모르겟네 byte_offset이 맞나? >> %니까 byte_offset 일거같은디 
    int byte_offset = (*(int *)addr) % WORD_SIZE_BYTE;
    //int byte_offset = (*(int *)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE; 

    /* get the entry index by invoking find_entry_index_in_set()
        for copying to the cache */
    int entry_index = find_entry_index_in_set(cache_index);

    int tag = blockAddr / CACHE_SET_SIZE; 
    printf("MEMORY >> word index = %d\n", entry_index);

    /* add this main memory access cycle to global access cycle */
    //num_access_cycles+=100;

    /* Fetch the data from the main memory and copy them to the cache */
    /* void *addr: addr is byte address, whereas your main memory address is word address due to 'int memory_array[]' */
    int i=0;
    int arrayIndex = word_index;
    while(i<DEFAULT_CACHE_BLOCK_SIZE_BYTE){
        for(int j=0;j<WORD_SIZE_BYTE;j++){
            cache_array[cache_index][entry_index].data[i]
                = (memory_array[arrayIndex] >> (i * 8)) & 0xFF;
            i++;
        }
        arrayIndex++;
    }
    //valid랑 tag 내가 넣었어 -> 고마워^.ㅜ
    cache_array[cache_index][entry_index].valid  = 1; 
    cache_array[cache_index][entry_index].tag  = tag;
    cache_array[cache_index][entry_index].timestamp = global_timestamp++; 
    printf("미스난 태그는: %d",cache_array[cache_index][entry_index].tag); 
            printf("global time: %d", global_timestamp);
            printf("cache의 time: %d", cache_array[cache_index][entry_index].timestamp );

    cache_entry_t *cache= &cache_array[cache_index][entry_index];
    /* Return the accessed data with a suitable type (b, h, or w) */
    if(type=='b'){
        return cache->data[byte_offset]; 
    }
    else if(type == 'h'){
        return cache ->data[byte_offset]| (cache ->data[byte_offset +1] <<8); 
    }
    else if(type == 'w'){
        return cache ->data[byte_offset] |(cache ->data[byte_offset+1] << 8) |(cache ->data[byte_offset+2] << 16) |(cache ->data[byte_offset+3] << 24) ;
    }
    // return -1 for unknown type
}
