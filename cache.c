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
                                                                        //검사할 데이터의 캐시 자리 정보
    int blockAddr = (*(int *)addr) / DEFAULT_CACHE_BLOCK_SIZE_BYTE;     //block address = byte address / cache block size 
    int cache_index = blockAddr % CACHE_SET_SIZE;                       //cache index = block address % cache set size (캐시 세트 개수) 
    int tag= blockAddr / CACHE_SET_SIZE;                                //tag = block address / cache set size (캐시 세트 개수)
    int byte_offset = (*(int *)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE;   //byte offset = byte address % cache block size 

    num_access_cycles++;                                                //cache hit 검사하러 오면서 access cycle 개수 + 1 

    printf("CACHE >> block_addr = %d, byte_offset = %d, cache_index = %d, tag = %d\n",      //캐시 정보가 잘 나오는지 확인용 코드 
                   blockAddr, byte_offset, cache_index, tag);

    
    for(int i =0; i<DEFAULT_CACHE_ASSOC; i++){                          //반복문을 associativity 번 돌면서 cache hit 검사 
         cache_entry_t *cache = &cache_array[cache_index][i];           //cache 포인터로 cache_array[cache_index][i]를 가리킴 

        if(cache->valid == 1 && cache->tag == tag){                     //cache_array의 valid 값이 1 이고 검사할 데이터의 tag와 같은지 확인 >> hit 
            printf("=> Hit!\n");                                        //hit 확인용 코드  
                                                                    
            cache->timestamp = global_timestamp++;                      //히트 시 cache_array의 timestamp를 global_timestamp로 맞추고 globalstamp + 1 증가 
            num_cache_hits ++;                                          //hit 횟수 + 1    
            cache->tag =  tag;                                          //cache_array의 tag를 hit한 데이터의 tag로 바꿔줌 

            if(type=='b'){                                              //hit한 데이터가 "byte" type일 경우 
                num_bytes +=1;                                          //accessed byte + 1 
                return cache->data[byte_offset];                        //cache_array의 data값의 byte_offset 인덱스에 접근하여 한 바이트만 반환한다 
            }
            else if(type == 'h'){                                       //hit한 데이터가 "halfword" type일 경우 
                num_bytes+=2;                                           //accessed byte + 2 
                return (unsigned char)cache ->data[byte_offset]         //cache_array의 data값의 byte_offset 인덱스에 접근하여 한 바이트를 가져오고
                        | cache ->data[byte_offset +1] <<8;             //byte_offset + 1 인덱스에 접근하여 한 바이트를 더 갖고오고 8비트(1바이트) 왼쪽으로 이동시킨 후 반환한다. 
            }
            else if(type == 'w'){                                       //hit한 데이터가 "word" type일 경우 
                num_bytes+=4;                                           //accessed byte + 4
                return (unsigned char)cache ->data[byte_offset]         //cache_array의 data값의 byte_offset 인덱스에 접근하여 한 바이트를 가져온다.
                        |((unsigned char)cache ->data[byte_offset+1]) << 8  //(byte_offset + 1) 인덱스에 접근하여 8비트(1바이트) 왼쪽으로 이동시킨 후 1바이트를 가져온다.
                        |((unsigned char)cache ->data[byte_offset+2]) << 16 //(byte_offset + 2) 인덱스에 접근하여 16비트(2바이트) 왼쪽으로 이동시킨 후 1바이트를 가져온다.
                        |cache ->data[byte_offset+3] << 24 ;                //(byte_offset + 3) 인덱스에 접근하여 24비트(4바이트) 왼쪽으로 이동시킨 후 1바이트를 가져온다. >> 모두 합쳐 반환한다.
            }
        }
        
    }
    num_cache_misses++;                                                 //miss 시 캐시 miss 횟수 + 1
    printf("=> Miss!\n");                                               //miss 확인용 코드 
    
    return -1; 
    
}

// This function is to find the entry index in set for copying to cache
int find_entry_index_in_set(int cache_index) {
    int entry_index=0;
    
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
    int byte_offset = (*(int *)addr) % WORD_SIZE_BYTE;
    
    /* get the entry index by invoking find_entry_index_in_set()
        for copying to the cache */
    int entry_index = find_entry_index_in_set(cache_index);

    int tag = blockAddr / CACHE_SET_SIZE; 

    printf("MEMORY >> word index = %d\n", entry_index);
   
    /* add this main memory access cycle to global access cycle */
    num_access_cycles+=100;

    /* Fetch the data from the main memory and copy them to the cache */
    /* void *addr: addr is byte address, whereas your main memory address is word address due to 'int memory_array[]' */
    int i=0;
    int arrayIndex = word_index;
    while(i<DEFAULT_CACHE_BLOCK_SIZE_BYTE){
        for(int j=0;j<WORD_SIZE_BYTE;j++){
            cache_array[cache_index][entry_index].data[i]
                = (memory_array[arrayIndex] >> (j * 8)) & 0xFF;
            i++;
        }
        arrayIndex++;
    }

    cache_array[cache_index][entry_index].valid  = 1;           //cache_array의 valid 값을 1로 바꾼다
    cache_array[cache_index][entry_index].tag  = tag;           //cache_array의 tag값을 데이터의 tag값으로 바꾼다 
    cache_array[cache_index][entry_index].timestamp = global_timestamp++;   //cache_array의 timestamp를 global_timestamp로 바꾸고 global_timestamp를 1 증가한다
   
    cache_entry_t *cache= &cache_array[cache_index][entry_index];

    /* Return the accessed data with a suitable type (b, h, or w) */
    if(type=='b'){
        num_bytes+=1;                       //byte 타입이므로 액세스한 바이트를 1 증가시킨다
        return cache->data[byte_offset]; 
    }
    else if(type == 'h'){
        num_bytes+=2;                      //halfword 타입이므로 액세스한 바이트를 2 증가시킨다
        return (unsigned char)cache ->data[byte_offset]
                | (cache ->data[byte_offset +1]) <<8; 
    }
    else if(type == 'w'){   
        num_bytes+=4;                      //word 타입이므로 액세스한 바이트를 4 증가시킨다 
        return (unsigned char)cache ->data[byte_offset]
                |((unsigned char)cache ->data[byte_offset+1]) << 8
                |((unsigned char)cache ->data[byte_offset+2]) << 16
                |(cache ->data[byte_offset+3]) << 24;
    } else return -1; //return -1 for unknown type
}
