#include "fasttrigon.h"


void init_fast_trigon() {
    int i;
    for(i=0;i<360;i++) {
        fast_sin_table[i] = sin(i * 3.14159265 / 180.0);
        fast_cos_table[i] = cos(i * 3.14159265 / 180.0);
        fast_tan_table[i] = tan(i * 3.14159265 / 180.0);

    }
}

inline float fastsin(int angle) {
    while(angle>=360) angle-=360;
    while(angle<0) angle+=360;
    return fast_sin_table[angle];
}

inline float fastcos(int angle) {
    while(angle>=360) angle-=360;
    while(angle<0) angle+=360;
    return fast_cos_table[angle];
}


inline int fastatan(float tg) {
    float error=99999, aux;
    int i,ret=-1;

    for(i=0;i<360;i++) {
        //if(i==0 || i==180) continue;

        aux = fast_tan_table[i] ;
        if(aux==tg) return i;

        if(fastabsf(aux-tg) < error) {
            error=fastabsf(aux-tg);
            ret=i;
        }
    }

    if(ret==-1) return (int)atan(tg);
    return ret;
}

inline float fastabsf(float n) {
    if(n<0) return (n * -1);
    else return n;
}

inline int fastabs(int n) {
    if(n<0) return (n * -1);
    else return n;
}
