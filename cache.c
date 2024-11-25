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
    int blockAddr = (*(int *)addr) / 8; 
    int cache_index = blockAddr % CACHE_SET_SIZE ; 
    int tag= blockAddr / CACHE_SET_SIZE; 
    int byte_offset = (*(int *)addr) % 8; 
    int word_index = blockAddr * 8 / CACHE_SET_SIZE; 

    for(int i =0; i<DEFAULT_CACHE_ASSOC; i++){
        cache_entry_t *cache = &cache_array[cache_index][i]; 

        if(cache->valid == 1 && cache->tag == tag){
            cache->timestamp ++;
            global_timestamp ++; 
            num_cache_hits ++;  
            //return cache->data[0];

            if(type=='b'){
                return cache->data[byte_offset]; 
            }
            else if(type == 'h'){

                return cache ->data[byte_offset]| (cache ->data[byte_offset +1] <<8); 
            }
            else if(type == 'w'){
                return (cache ->data[byte_offset+3]<< 24) |(cache ->data[byte_offset+2] << 16) |(cache ->data[byte_offset+1] << 8) |(cache ->data[byte_offset]) ; 
            }
        }
        
    }
    num_cache_misses++; 
        return -1; 
    
}
// This function is to find the entry index in set for copying to cache
int find_entry_index_in_set(int cache_index) {
    int entry_index;
    
    // set 어쩌고에서는 cache_index=집합 번호
    /* Check if there exists any empty cache space by checking 'valid' */
    for(int i=0;i<DEFAULT_CACHE_ASSOC;i++)
        if(cache_array[cache_index][i].valid==0)
            return entry_index = i;

    /* If the set has only 1 entry, return index 0 */
    /* Otherwise, search over all entries
    to find the least recently used entry by checking 'timestamp' */
    if(CACHE_SET_SIZE == 1) entry_index = 0;
    else{
        int min_timestamp = cache_array[cache_index][0].timestamp;
        for(int i=1;i<DEFAULT_CACHE_ASSOC;i++) {
            if(min_timestamp > cache_array[cache_index][0].timestamp){
                min_timestamp = cache_array[cache_index][0].timestamp;
                entry_index = i;
            }
        }
    }

    /* return the cache index for copying from memory*/
    return entry_index; 
}

int access_memory(void *addr, char type) {
    
    /* get the entry index by invoking find_entry_index_in_set()
        for copying to the cache */

    /* add this main memory access cycle to global access cycle */

    /* Fetch the data from the main memory and copy them to the cache */
    /* void *addr: addr is byte address, whereas your main memory address is word address due to 'int memory_array[]' */

    /* You need to invoke find_entry_index_in_set() for copying to the cache */




    /* Return the accessed data with a suitable type (b, h, or w) */    
    // return -1 for unknown type
    return 0;
}
