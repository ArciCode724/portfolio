#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct circelString{
    char* text;
    int len;
};

struct transfromString{
    char* text;
    int len;
    int row;
};

void printCircle(struct circelString base, int ofset){
    for(int i=ofset; i < base.len; i++)
        printf("%c", base.text[i]);
    for(int i=0; i < ofset; i++)
        printf("%c", base.text[i]);
    printf("\n");
}

// 1 a > b; 0 a = b; -1 a < b
int compOfset(struct circelString base, int ofsetA, int ofsetB){
    for(int i=0; i<=base.len; i++){
        if(ofsetA >= base.len)
            ofsetA -= base.len;
        if(ofsetB >= base.len)
            ofsetB -= base.len;

        if(base.text[ofsetA] > base.text[ofsetB])
            return 1;
        else if(base.text[ofsetA] < base.text[ofsetB])
            return -1;

        ofsetA++;
        ofsetB++;
    }
    return 0;
}

int quicSortSplit(int list[], struct circelString base, int start, int end){
    int temp;
    int pivit = list[start];
    int left = start+1;
    for(int i = start+1; i<end; i++){
        if(compOfset(base, pivit, list[i]) == 1){
            temp = list[left];
            list[left] = list[i];
            list[i] = temp;
            left++;
        }
    }
    list[start] = list[left-1];
    list[left-1] = pivit;
    return left;
}

void quicSort(int list[], struct circelString base, int start, int end){
    if(end - start <= 1)
        return;
    int split = quicSortSplit(list, base, start, end);
    quicSort(list, base, start, split);
    quicSort(list, base, split, end);
}

int main(){
    struct circelString naszString;
    naszString.text = "babcbaaba";
    naszString.len = strlen(naszString.text);
    int list[naszString.len];
    for(int i = 0; i<naszString.len; i++){
        list[i] = i;
    }
    quicSort(list, naszString, 0, naszString.len);


    struct transfromString transfrom;
    transfrom.len = naszString.len;
    transfrom.text = malloc(naszString.len+1);
    transfrom.text[naszString.len] = '\0';
    for (int i = 0; i < naszString.len; i++){
        if(list[i] == 0){
            transfrom.text[i] = naszString.text[naszString.len-1];
            continue;
        }
        transfrom.text[i] =  naszString.text[list[i]-1];
    }
    for (int i = 0; i < naszString.len; i++){
        if(list[i]== 0){
            transfrom.row = i;
            break;
        }
    }
    printf("%s : %d\n", transfrom.text, transfrom.row);
    

    char sorted[transfrom.len+1];
    sorted[transfrom.len] = '\0';
    for(int i=0; i<transfrom.len; i++)
        sorted[i] = transfrom.text[i];
    
    for(int i=1; i<transfrom.len; i++){
        for (int j = 1; j < transfrom.len-i+1; j++){
            if(sorted[j-1] > sorted[j]){
                char tamp = sorted[j-1];
                sorted[j-1] = sorted[j];
                sorted[j] = tamp;
            }
        }
    }

    printf("%s\n", sorted);
    int conection[transfrom.len];
    for(int i=0; i<transfrom.len; i++){
        // printf("%c\n ", sorted[i]);
        int next;
        for(next=0; next<transfrom.len; next++){
            // printf("%c ", transfrom.text[next]);
            if(sorted[i] != transfrom.text[next]){
                continue;
            }
            int aledyExist = 0;
            for(int j = i-1; j >=0; j--){
                if(conection[j] == next){
                    aledyExist = 1;
                    break;
                }
            }
            if(aledyExist){
                continue;
            }
            else{
                // printf("%c %d; ", transfrom.text[next], next);
                break;
            }
        }
        // printf("\n");
        conection[i] = next;
        // printf("%d ", conection[i]);
    }

    int curentIndex = transfrom.row;
    do{
        printf("%c", sorted[curentIndex]);
        curentIndex = conection[curentIndex];
    } while (curentIndex != transfrom.row);
    

    return 0;
}