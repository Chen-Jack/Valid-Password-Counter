#include<stdio.h>


int main(int args, char* var[]){

    char word[6];
    for(int i=0; i<5; i++){word[i] = '0';}
    word[5] = '\0';

    printf("%s\n",word);

    return 0;
}

