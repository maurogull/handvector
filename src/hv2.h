#ifndef HVCORE2_H
#define HVCORE2_H


#include "globals.h"
#include "coremath.h"
#include "fasttrigon.h"
#include "strokelist.h"
#include <math.h>
#include <stdlib.h>



void hv2_start_analysis();

void hv2_discover_strokes();

hvpoint* hv2_nearest_alone_point(hvpoint* first);

int hv2_are_close_points(hvpoint* first, hvpoint* second);

hvpoint* hv2_previous_point(hvpoint* p) ;

int hv2_points_between(const hvpoint* const from, const hvpoint* const to);

hvpoint * hv2_middle_point(const hvpoint * const p1, const hvpoint * const p2) ;

void hv2_draw_strokes();

void hv2_delete_poor_strokes();

#endif

