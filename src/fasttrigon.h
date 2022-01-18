#ifndef HVFASTTRIGON_H
#define HVFASTTRIGON_H


#include <math.h>
#include <stdlib.h>


float fast_cos_table[361];
float fast_sin_table[361];
float fast_tan_table[361];

void init_fast_trigon();

inline float fastsin(int angle);

inline float fastcos(int angle);

inline int fastatan(float tg);

inline float fastabsf(float n);

inline int fastabs(int n);

#endif
