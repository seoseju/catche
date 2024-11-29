#include <stdio.h>

#include <string.h> // 내가 했어..
#include <stdlib.h> //내가 했어..


int main(void){
FILE *ifp = NULL, *ofp = NULL;

ifp = fopen("access_input.txt", "r"); //ifp = 인풋파일을 '읽기모드'로 열었음 
    if (ifp == NULL) {
        printf("Can't open input file\n");
        return -1;
    }
    ofp = fopen("^^OutputTest.txt", "w"); //ofp = 아웃풋파일을 쓰기 모드로 열었음 
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }

char s[1000];
snprintf(s+strlen(s) , sizeof(s)-strlen(s),"heihei"); 
            //fputs(s,ofp); 
            fprintf(ofp,s); 
fprintf(ofp," hei"); 
fclose(ifp); 
fclose(ofp); 
    return 0; 
}