#ifndef HVCORE1_H
#define HVCORE1_H


#include "globals.h"
#include "pointlist.h"

#include "fasttrigon.h"

#include <math.h>


void hv1_start_analysis(const hvpoint, int);

hvpoint* hv1_discover_next_point(hvpoint *);
hvpoint* hv1_find_nearby(int fx, int fy);

void hv1_delete_similar_points();
void hv1_delete_erased_points();

void hv1_draw_points();

#endif
