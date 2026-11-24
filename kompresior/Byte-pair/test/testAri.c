#include<stdio.h>
#include"..\code\ariCoding.c"

int main(){
    FILE* f = fopen("d.txt", "wb");
    
    ariCtxEncode* ectx = ari_encodingContex(f);
    token list[] = {0,1,2,3,4,5,6,7,8,9};
    for(int i = 0; i < sizeof(list)/sizeof(*list); i++){
        ari_encodeToken(ectx, list[i]);
        // printf("\n");
    }
    ari_flushEncode(ectx);
    free(ectx);
    fclose(f);
    // printf("\n");

    f = fopen("d.txt", "rb");
    ariCtxDecode* dctx = ari_decodingContex(f);
    for (int i = 0; i < sizeof(list)/sizeof(*list); i++){
        printf("%3d", ari_decodeToken(dctx));
    }
    printf("\n");
    free(dctx);
    fclose(f);
}