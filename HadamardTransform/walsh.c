// https://en.wikipedia.org/wiki/Walsh_function
#include"./printTable.c"

void walshOrder(int* out, int level){
    int listSize = 1<<level;
    for(int k = 0; k < listSize; k++){
        int lastState = 0;
        out[k] = 0;
        for(int i = 1; i < listSize; i++){
            // int state = i & k;
            // for(int half = sizeof(int)/2*8; half > 0; half /= 2) state ^= state>>half;
            int state = 0;
            for(int x = i&k; x; x>>=1){ state ^= x;}
            state &= 1;

            if(state != lastState) out[k]++;
            lastState = state;
        }
    }
}

int main(){
    int data[8];
    walshOrder(data, 3);
    print1d(data, 8);
}