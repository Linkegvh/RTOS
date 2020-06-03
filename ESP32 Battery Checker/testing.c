#include <stdio.h>

void printing( int * a, int b);

int main(){
    int Num [6];
    int i;
    for (i = 0; i < 6; i ++){
        Num[i] = i;
    }
    printing(Num, 6);
}

void printing( int * a, int b){
    int i = 0;
    while (i < b){
        printf("%d ", a[i]);
        i++;
    }
}