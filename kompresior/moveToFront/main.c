#include<stdio.h>
#include<math.h>

void entropy(int* list, int n){
    float entropy = 0;
    int suma = 0;
    for(int i=0;i<n;i++)
        suma+=list[i];
    
    for(int i=0;i<n;i++){
        if(list[i] == 0)
            continue;
        float p = (float)list[i]/(float)suma;
        entropy-= p * log2(p);
    }
    printf("%f\n", entropy);
}

int main(int argc, char** argv){
    unsigned char stack[256];
    int apearnceIn[256];
    int apearnceOut[256];
    for(int i = 0; i<256; i++){
        stack[i] = i;
        apearnceIn[i] = 0;
        apearnceOut[i] = 0;
    }

    FILE* f = fopen(argv[1], "rb");
    struct{
        double readed;
        double size;
        int show;
    } fileReaded;
    fseek(f, 0, SEEK_END);
    fileReaded.size = (double)ftell(f);
    fclose(f);

    fileReaded.readed = 0;
    fileReaded.show = 0;
    for(long i= (long)fileReaded.size; i>1; i/=10)
        fileReaded.show++;

    f = fopen(argv[1], "rb");
    FILE* out = fopen((argc>2) ? argv[2] : "out.txt", "wb");
    unsigned char c;
    int index;
    while (fscanf(f, "%c", &c) != EOF){
        fileReaded.readed++;
        printf("\r%1.*lf", fileReaded.show, fileReaded.readed/fileReaded.size);

        apearnceIn[c]++;
        for(index = 0; stack[index] != c; index++);
        fprintf(out, "%c", index);
        apearnceOut[index]++;
        // printf("%2x ", index);
        for(; index>0; index--){
            unsigned char temp = stack[index];
            stack[index] = stack[index-1];
            stack[index-1] = temp;
        }
    }
    printf("\n");
    entropy(apearnceIn, 256);
    entropy(apearnceOut, 256);

    return 0;
}