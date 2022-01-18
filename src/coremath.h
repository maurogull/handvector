#ifndef HVCOREMATH_H
#define HVCOREMATH_H


#include <math.h>
#include <stdlib.h>
#include "fasttrigon.h"

#include "hv2.h"
#include "hv3.h"

int hv_angle_between(hvpoint *p1, hvpoint *p2);
int hv_colinear_angle(int ang);
inline int hv_normalize_angle(int ang);

float hv_points_distance(const hvpoint* const from, const hvpoint* const to);

int hv_is_straight_segment( hvpoint* p1, hvpoint* p2);



inline float hv_bezier_error(const hvpoint* const from, const hvpoint* const to, int x1,int y1,int x2,int y2) ;

inline void hv_compute_cubic_bezier(const hvpoint* from, const hvpoint* to, int x1, int y1, int x2, int y2, float t, float*resx, float*resy);



#endif
