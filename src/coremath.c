#include "coremath.h"



int hv_angle_between(hvpoint *p1, hvpoint *p2) {

    if(p1->x == p2->x && p1->y > p2->y) return 90;
    if(p1->x == p2->x && p1->y < p2->y) return 270;
    if(p1->y == p2->y && p1->x < p2->x) return 0;
    if(p1->y == p2->y && p1->x > p2->x) return 180;

    int x,y, ang;
    x = p2->x - p1->x;
    y = p1->y - p2->y;

    if(x==0) return 90;
    //ang = (int)   (atan((float)y/x) * 180 / PI);
    ang = (int) atan2(y,x);
    //ang = (fastatan ((float)y/x)) ;
    return hv_normalize_angle(ang);
}

float hv_points_distance(const hvpoint* const from, const hvpoint* const to) {
    float catet1 = (float)fastabs(from->x - to->x);
    float catet2 = (float)fastabs(from->y - to->y);

    float sumcuad = (catet1*catet1) + (catet2*catet2);
    float ret = (float)sqrt(sumcuad);

    return ret;
}

int hv_colinear_angle(int ang) {
    ang = ang+180;

    return hv_normalize_angle(ang);
}

inline int hv_normalize_angle(int ang) {
    while(ang<0) ang+=360;
    while(ang>=360) ang-=360;

    return ang;
}

inline float hv_bezier_error(const hvpoint* const from, const hvpoint* const to, int x1,int y1,int x2,int y2) {
    int steps = hv2_points_between(from,to);
    if(steps<2) return 9999.9;

    float stepbezier = 1.0 / (steps+1.0);
    float auxerror=0;

    hvpoint* auxp=from;
    int i, j;
    for(i=1; i<=steps; i++) {
        float t, resx, resy;

        t = i * stepbezier ;
        hv_compute_cubic_bezier(from, to, x1, y1, x2, y2, t, &resx, &resy);

        //calculate error
        auxp = auxp->next_s;
        auxerror += abs(auxp->x - (int)resx) + abs(auxp->y - (int)resy);

    }

    return auxerror / steps;

}


inline void hv_compute_cubic_bezier(const hvpoint* from, const hvpoint* to, int x1, int y1, int x2, int y2, float t, float*resx, float*resy) {

    float ax, bx, cx, ay, by, cy, tSquared, tCubed;

    //cubic bezier curve :)
    cx = 3.0 * (x1 - from->x);
    bx = 3.0 * (x2 - x1) - cx;
    ax = (float)to->x - (float)from->x - cx - bx;
    cy = 3.0 * (y1 - from->y);
    by = 3.0 * (y2 - y1) - cy;
    ay = (float)to->y - (float)from->y - cy - by;

    tSquared = t * t;
    tCubed = tSquared * t;

    *resx = (ax * tCubed) + (bx * tSquared) + (cx * t) + (float)from->x;
    *resy = (ay * tCubed) + (by * tSquared) + (cy * t) + (float)from->y;

}


int hv_is_straight_segment( hvpoint* p1, hvpoint* p2) {

    float dist = hv_points_distance(p1,p2);
    float straight_tolerance = 1.0 + (dist * 0.025);

    if(p1->x==p2->x || p1->y==p2->y) return 1;  //orthogonal

    int steps = hv2_points_between(p1,p2);

    if(steps==0) return 1;  // two consecutive nodes became straight connected

    float stepbezier = 1.0 / (steps+1.0);
    float auxerror=0;

    hvpoint* auxp=p1;
    int i, j;
    for(i=1; i<=steps; i++) {
        float resx, resy;

        //linear bezier  :)
        resx = (1.0 - (i*stepbezier)) * p1->x + (i*stepbezier) * p2->x;
        resy = (1.0 - (i*stepbezier)) * p1->y + (i*stepbezier) * p2->y;

        auxp = auxp->next_s;
        auxerror += abs(auxp->x - (int)resx) + abs(auxp->y - (int)resy);
    }

    if ( (auxerror / steps) <= straight_tolerance) return 1;
    return 0;
}
