#include<stdio.h>


int main(){
    char* word = "bcnana";
    int n;
    for(n=0; *(word+n); n++);
    char type[n+1];
    type[n] = '\0';
    type[n-1] = 'S';
    for(int i=n-2; i>=0; i--){
        if(word[i] == word[i+1]){
            type[i] = type[i+1];
            continue;
        }
        if(word[i] < word[i+1]){
            type[i] = 'S';
        }else{
            type[i] = 'L';
        }
    }
    printf("%s : %d\n",word, n);
    printf("%s", type);
    return 0;
}