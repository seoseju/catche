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
//#include "cache.c"

int num_cache_hits = 0;
int num_cache_misses = 0;

int num_bytes = 0;
int num_access_cycles = 0;

int global_timestamp = 0;

int retrieve_data(void *addr, char data_type) {
    int value_returned = -1; /* accessed data */                        // initialize "value_returned" to -1 to store accessed data 

    /*check data by invoking check_cache_data_hit()*/
    printf("===== addr %d type %c ======\n", *(int *)addr, data_type);  // debugging code to check if addr and type parameters are correct 
    value_returned = check_cache_data_hit(addr, data_type);             // check cache data hit: if HIT, returned value will have accessed data
                                                                        //                       if MISS, returned value will have '-1'

    /* In case of the cache miss event, retrieve data from the main memory  by invoking access_memory()*/
    if(value_returned == -1){                                           // MISS case  
        value_returned = access_memory(addr, data_type);                // invoke access_memory() to read the target data from main memory 

        if(value_returned == -1) return -1;                             // If there is no data neither in cache nor memory, return -1, else return data
        
        /* access_memory() success!! */
        else return value_returned;                                     // if memory access succeeds, return data from memory
    }
    /* hit!! */
    else return value_returned;                                         // HIT case: return data accessed from cache
  
}

int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr; /* byte address (located at 1st column) in "access_input.txt" */
    char access_type; /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data; /* This is the data that you want to retrieve first from cache, and then from memory */ 
    
    init_memory_content();
    init_cache_content();
    
    ifp = fopen("access_input2.txt", "r");
    if (ifp == NULL) {
        printf("Can't open input file\n");
        return -1;
    }
    ofp = fopen("access_output.txt", "w");
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }
    
    /* Fill out here by invoking retrieve_data() */

    char line[101];                                                     // array to store one line from input file
    char *array[100];                                                   // array to store tokens from input lines
    int lineCount=0;                                                    // the number of lines

    while(fgets(line, sizeof(line), ifp)!=NULL && lineCount < 100){     // loop while line string from input file is not NULL and line count is under MAX_LINE_COUNT
        array[lineCount] = malloc(sizeof(char)*101);                    // dynamic memory allocation to "array"
        char *token = strtok(line, " \n");                              // tokenize one input line by " " and "\n"
        int tokenIndex = 0;                                             // initialize tokenIndex to differentiate address and type string
        while(token!=NULL&&tokenIndex<2){                               // loop while token is not NULL and tokenIndex is under 2
            if(tokenIndex==0)                                           // case that the token means byte address
                strncpy(array[lineCount], token, 101);                  // store address to "array"
            else if(tokenIndex==1)                                      // case that the token means type
                array[lineCount][100] = token[0];                       // store type character to the last entry of "array"
            token=strtok(NULL, " \n");                                  // empty the token list
            tokenIndex++;                                               // increment tokenIndex by one
        }
        lineCount++;                                                    // increment lineCount by one to read the next line
    }

    // Now, array[i] is pointer of the target address string
    // array[i][1] value is the type character
    // to retreive data from the cache and memory and print the data into "output file"
    char s[1000] = "";                                  // char type list to store character to be printed into the output file
    fprintf(ofp,"[Accessed Data]\n");                   // print "[Accessed Data]\n" into output file

    for(int i=0;i<lineCount;i++){                       // loop NUM_LINES times to retrieve i-th data
        access_addr = strtoul(array[i], NULL, 10);      // load access address from "array[i]"
        access_type = array[i][100];                    // load access type from "array[i][100]"
        accessed_data = retrieve_data(&access_addr, access_type);   // store retrieved data from cache or memory into accessed data variable
        print_cache_entries();                          // debugging code to check if the correct data is stored into cache
        snprintf(s+strlen(s), sizeof(s) - strlen(s), "%d\t%c\t%#x\n",access_addr,access_type,accessed_data);    // store access information into "s" array
    }

    // print every input information and accessed data into the output file
    fprintf(ofp,"%s",s);
    fprintf(ofp,"----------------------------\n") ;

    // print information about cache access performance
    if(DEFAULT_CACHE_ASSOC  == 1)                                       // DEFAULT_CACHE_ASSOC = 1 (direct mapped cache)
        fprintf(ofp,"[Direct mapped Cache performance]\n");             // print "[Direct mapped Cache performance]"
    else if(DEFAULT_CACHE_ASSOC  == 2)                                  // DEFAULT_CACHE_ASSOC = 2 (2-way set associative cache)
        fprintf(ofp,"[2-way set associative Cache performance]\n");     // print "[2-way set associative Cache performance]"
    else if(DEFAULT_CACHE_ASSOC  == 4)                                  // DEFAULT_CACHE_ASSOC = 4 (fully associative cache)
        fprintf(ofp,"[Fully associative Cache performance]\n");         // print "[Fully associative Cache performance]"

    fprintf(ofp,"Hit ratio = %.2f  (%d/%d)\n", (float)num_cache_hits/global_timestamp, num_cache_hits,global_timestamp);   // print hit ratio (the number of cache hit cases)/(the number of cache access)
    fprintf(ofp,"Bandwidth = %.2f  (%d/%d)\n", (float)num_bytes/num_access_cycles, num_bytes,num_access_cycles);           // print bandwidth (the number of accessed bytes)/(the access cycles)

    fclose(ifp); 
    fclose(ofp);
    
    return 0;
}