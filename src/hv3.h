#ifndef HVCORE3_H
#define HVCORE3_H


#include "globals.h"
#include "strokelist.h"
#include "hv2.h"
#include "coremath.h"
#include "fasttrigon.h"

#include <math.h>
#include <stdlib.h>
#include <cairo.h>

void hv3_start_analysis();

void hv3_discover_inflections();

void hv3_set_first_last_nodes();

void hv3_delete_sibling_nodes();

void hv3_clear_all_beziers();

int inline hv3_points_in_same_side( hvpoint* p1, hvpoint* p2);

void hv3_discover_beziers() ;

void hv3_discover_beziers_last_chance();

void hv3_split_difficult_ones();

void hv3_find_diagonal_hangers(hvpoint * p1,hvpoint * mid,hvpoint * p2) ;

void hv3_bezier_best_fit_ortho(const hvpoint* const p1, const hvpoint* const p2,int* best_prev_x, int* best_prev_y,int* best_auxp_x, int* best_auxp_y, float* besterror);



hvpoint* hv3_previous_node(hvpoint* p);
hvpoint* hv3_next_node(hvpoint* p);

void hv3_draw_nodes(cairo_t * cr);

#endif

