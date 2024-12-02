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

// This function is to check whether cache data hit
int check_cache_data_hit(void *addr, char type) {
    // Information of the target cache block
    int blockAddr = (*(int *)addr) / DEFAULT_CACHE_BLOCK_SIZE_BYTE;     // block address = byte address / cache block size 
    int cache_index = blockAddr % CACHE_SET_SIZE;                       // cache index = block address % cache set size (the index of cache entry matched with the block addr) 
    int tag= blockAddr / CACHE_SET_SIZE;                                // tag = block address / cache set size (the index of cache set matched with the block addr)
    int byte_offset = (*(int *)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE;   // byte offset = byte address % cache block size 

    num_access_cycles+=CACHE_ACCESS_CYCLE;                              // increment the number of access cycles by one
    
    // debugging code to check if every above cache information is correct
    printf("CACHE >> block_addr = %d, byte_offset = %d, cache_index = %d, tag = %d\n",      
                   blockAddr, byte_offset, cache_index, tag);

    
    for(int i =0; i<DEFAULT_CACHE_ASSOC; i++){                          // loop CACHE_ASSOC times to check cache hit 
        cache_entry_t *cache = &cache_array[cache_index][i];            // initialize "cache" pointer variable to access "cache_array[cache_index][i]"

        if(cache->valid == 1 && cache->tag == tag){                     // if HIT: if valid is '1' and the stored tag is same to one of the target data
            printf("=> Hit!\n");                                        // debuggin code to check HIT  
                                                                    
            cache->timestamp = ++global_timestamp;                      // update timestamp of the cache block and increment global timestamp by one
            num_cache_hits ++;                                          // increment the number of cache hit by one    
            cache->tag =  tag;                                          // set the tag of cache block to one of hit data

            if(type=='b'){                                              // if hit data type is "byte" 
                num_bytes +=1;                                          // byte type: increment the number of accessed bytes by one
                return cache->data[byte_offset];                        // return one byte data from byte_offset index of the stored cache
            }
            else if(type == 'h'){                                       // if hit data type is "halfword"  
                num_bytes+=2;                                           // halfword type: increment the number of accessed bytes by two
                return (unsigned char)cache ->data[byte_offset]         // the first byte from byte_offset index of the stored cache
                        | cache ->data[byte_offset +1] <<8;             // the second byte from (byte_offset+1) index and left-shift by 8 >> combine the return value by OR operation
            }
            else if(type == 'w'){                                       // if hit data type is "word" 
                num_bytes+=4;                                           // word type: increment the number of accessed bytes by four
                return (unsigned char)cache ->data[byte_offset]         // return the first byte from byte_offset index of the stored cache
                        |((unsigned char)cache ->data[byte_offset+1]) << 8  // the second byte from (byte_offset+1) index and left-shift by 8
                        |((unsigned char)cache ->data[byte_offset+2]) << 16 // the third byte from (byte_offset+2) index and left-shift by 16
                        |cache ->data[byte_offset+3] << 24 ;                // the fourth byte from (byte_offset+3) index and left-shift by 24 >> combine the return value by OR operation
            }
        }
        
    }

    // MISS case
    num_cache_misses++;                                                 // increment the number of cache missis by one
    printf("=> Miss!\n");                                               // debugging code to check MISS
    
    return -1;                                                          // return -1 for cache miss case
    
}

// This function is to find the entry index in set for copying to cache
int find_entry_index_in_set(int cache_index) {
    int entry_index=0;                                                  // initialize the return value "entry_index" to zero
    
    /* Check if there exists any empty cache space by checking 'valid' */
    for(int i=0;i<DEFAULT_CACHE_ASSOC;i++){                             // loop DEFAULT_CACHE_ASSOC times to check if any block in the set has invalid space
        if(cache_array[cache_index][i].valid==0)                        // if any cache block is invalid
            return entry_index = i;                                     // return that entry index to put the memory data into that invalid block
    }

    // if any block in the set has other data
    if(CACHE_SET_SIZE == 1) entry_index = 0;                            // If the set has only 1 entry, return index 0
    else{            //Otherwise, search over all entries to find the least recently used entry by checking 'timestamp'
        int min_timestamp = cache_array[cache_index][0].timestamp;      // initailize "min_timestamp" to one of the first cache block
        for(int i=1;i<DEFAULT_CACHE_ASSOC;i++) {                        // loop DEFAULT_CACHE_ASSOC to find the minimal timestamp
            if(min_timestamp > cache_array[cache_index][i].timestamp){  // if there is smaller timestamp than "min_timestamp"
                min_timestamp = cache_array[cache_index][i].timestamp;  // update the "min_timestamp"
                entry_index = i;                                        // update "entry_index" to the index of the block with the minimal timestamp
            }
        }
    }

    /* return the cache index for copying from memory*/
    return entry_index;                                                 // return the "entry_index"
}

// This function is to access memory data and store it to cache
int access_memory(void *addr, char type) {                              // take address and type as parameters
    // Information of the target memory address
    int blockAddr = (*(int *)addr) / DEFAULT_CACHE_BLOCK_SIZE_BYTE;     // block address = byte address / cache block size
    int cache_index = blockAddr % CACHE_SET_SIZE;                       // cache index = block address % cache set size (the index of cache entry matched with the block addr) 
    int tag = blockAddr / CACHE_SET_SIZE;                               // tag = block address / cache set size (the index of cache set matched with the block addr)
    int word_index = ((*(int *)addr) / DEFAULT_CACHE_BLOCK_SIZE_BYTE)*2;               // word index = (byte address / num of cache block size)*2 (the index of cache set matched with the block addr)
    int byte_offset = (*(int *)addr) % DEFAULT_CACHE_BLOCK_SIZE_BYTE;                  // byte offset = byte address % num of word byte (the access start address in each memory entry)
    
    // get the entry index by invoking find_entry_index_in_set() for copying to the cache
    int entry_index = find_entry_index_in_set(cache_index);

    printf("MEMORY >> word index = %d\n", entry_index);
   
    num_access_cycles+=MEMORY_ACCESS_CYCLE;

    int i=0;
    int arrayIndex = word_index;                                        // initialize "arrayIndex" value to "word_index" to store two entries in the memory
    while(i<DEFAULT_CACHE_BLOCK_SIZE_BYTE){                             // loop block_size times to store data as each byte
        for(int j=0;j<WORD_SIZE_BYTE;j++){                              // loop word_size times to load data from each memory entry (word size)
            cache_array[cache_index][entry_index].data[i]               // store ONE BYTE data from memory entry to cache entry
                = (memory_array[arrayIndex] >> (j * 8)) & 0xFF;
            i++;                                                        // increment the number of times accessing one byte
        }
        arrayIndex++;                                                   // increment "arrayIndex" to move to the next array entry
    }

    cache_array[cache_index][entry_index].valid  = 1;                       // set the valid of cache_array to '1'
    cache_array[cache_index][entry_index].tag  = tag;                       // update the tag of the cache_array to one of the given address
    cache_array[cache_index][entry_index].timestamp = ++global_timestamp;   // update timestamp of the cache block and increment global timestamp by one
   
    cache_entry_t *cache= &cache_array[cache_index][entry_index];       // declare cache_entry_t pointer as cache

    /* Return the accessed data with a suitable type (b, h, or w) */
    if(type=='b'){
        num_bytes+=1;                                   // byte type: increment the number of accessed bytes by one
        return cache->data[byte_offset];                // return one byte data from newly updated cache
    }
    else if(type == 'h'){
        num_bytes+=2;                                   // halfword type: increment the number of accessed bytes by two
        return (unsigned char)cache ->data[byte_offset] // return two byte data from newly updated cache
                | (cache ->data[byte_offset +1]) <<8;   // using shift operation and bit mask
    }
    else if(type == 'w'){   
        num_bytes+=4;                                   // word type: increment the number of accessed bytes by four
        return (unsigned char)cache ->data[byte_offset] // return four byte data from newly updated cache
                |((unsigned char)cache ->data[byte_offset+1]) << 8
                |((unsigned char)cache ->data[byte_offset+2]) << 16
                |(cache ->data[byte_offset+3]) << 24;
    } else return -1;                                   //return -1 for unknown type
}
