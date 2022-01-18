#ifndef HVPOINTLIST_H
#define HVPOINTLIST_H

#include "globals.h"





hvpoint * hv1_new_point(unsigned int x, unsigned int y, hvpoint* next);

hvpoint * hv1_duplicate_point(hvpoint* orig);

void hv1_add_point(hvpoint * p1);

int hv1_point_exists(int x, int y);


void hv1_print_list();

void hv1_delete_point(hvpoint * p1);


#endif
