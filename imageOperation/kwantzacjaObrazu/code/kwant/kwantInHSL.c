#include"../../header/color.h"
#include<stdio.h>
#include<math.h>

// sorce https://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/

static int max(float l[3]){
    int retVal = 0;
    if(l[retVal] < l[1]){
        retVal = 1;
    }
    if(l[retVal] < l[2]){
        retVal = 2;
    }
    return retVal;
}

static int min(float l[3]){
    int retVal = 0;
    if(l[retVal] > l[1]){
        retVal = 1;
    }
    if(l[retVal] > l[2]){
        retVal = 2;
    }
    return retVal;
}

static void rgb2hsl(float rgb[3], float hsl[3]){
    int maxI = max(rgb);
    int minI = min(rgb);

    float plus = rgb[maxI] + rgb[minI];
    float minus = rgb[maxI] - rgb[minI];

    hsl[2] = plus/2;
    if(minus != 0){
        hsl[1] = minus / ((hsl[2] <= 0.5) ? plus : (2.0 - plus));
    }else{
        hsl[1] = 0;
    }
    if(minus != 0){
        if(maxI == 0){
            hsl[0] = (rgb[1] - rgb[2])/minus;
        }else if(maxI == 1){
            hsl[0] = 2.0 + (rgb[2] - rgb[0])/minus;
        }else{
            hsl[0] = 4.0 + (rgb[0] - rgb[1])/minus;
        }
        hsl[0] *= 60;
        if(hsl[0] < 0){
            hsl[0] += 360;
        }
    }else{
        hsl[0] = 0;
    }
}

static void hsl2rgb(float hsl[3], float rgb[3]){
    if(hsl[1] == 0){
        rgb[0] = hsl[2];
        rgb[1] = hsl[2];
        rgb[2] = hsl[2];
        return;
    }

    float temp1;
    if(hsl[2] < 0.5){
        temp1 = hsl[2] *(1.0 + hsl[1]);
    }else{
        temp1 = hsl[2] + hsl[1] - hsl[2] * hsl[1];
    }

    float temp2 = 2 * hsl[2] - temp1;

    hsl[0] /= 360;

    float tempRGB[3] = {hsl[0] + 1.0/3.0, hsl[0], hsl[0] - 1.0/3.0};
    // printf("%f %f %f\n", tempRGB[0], tempRGB[1], tempRGB[2]);
    for (int i = 0; i < 3; i++){
        tempRGB[i] += (tempRGB[i] < 0) ? 1 : 0;
        tempRGB[i] -= (tempRGB[i] > 1) ? 1 : 0;
    }
    // printf("%f %f %f\n", tempRGB[0], tempRGB[1], tempRGB[2]);
    
    for (int i = 0; i < 3; i++){
        if(6*tempRGB[i] < 1){
            rgb[i] = temp2 + (temp1 - temp2) * 6 * tempRGB[i];
        }else if(2*tempRGB[i] < 1){
            rgb[i] = temp1;
        }else if(3*tempRGB[i] < 2){
            rgb[i] = temp2 + (temp1 - temp2) * (2.0/3.0 - tempRGB[i]) * 6;
        }else{
            rgb[i] = temp2;
        }
    }
}

void kwantLight(color8_3* data, int dataNum, int grupNum){
    for (int i = 0; i < dataNum; i++){
        // RGB -> HSL
        float rgb[3];
        for (int j = 0; j < 3; j++){
            rgb[i] = data[i].c[j]/255.0;
        }

        float hsl[3];
        rgb2hsl(rgb, hsl);

        // printf("%f ", hsl[2]);
        hsl[2] = roundf( hsl[2] * (grupNum - 1) ) / (grupNum - 1);
        // printf("%f\n", hsl[2]);

        hsl2rgb(hsl, rgb);

        // printf("%f %f %f\n\n", rgb[0]*255, rgb[1]*255,rgb[2]*255);
        for (int j = 0; j < 3; j++){
            data[i].c[j] = rgb[0]*255;
        }
    }
}