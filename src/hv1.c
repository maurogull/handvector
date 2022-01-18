
#include "hv1.h"

/* module hv1, extracts points from raw drawing on canvas */

void hv1_start_analysis(const hvpoint p1, int arenewones) {

    if(arenewones) {
        hvpoint * pri=hv1_new_point(p1.x, p1.y, NULL);
        hv1_add_point(pri);

        hvpoint* prox = pri;
        int counting=0;
        while(prox = hv1_discover_next_point(prox)) {
            hv1_add_point(prox);
            counting++;
            if(counting>400) break; //please dont hang while discovering points!
        }

    }

    hv1_delete_erased_points(pointlist);
    hv1_delete_similar_points(pointlist);

    //hv1_print_list();
}



hvpoint* hv1_discover_next_point(hvpoint * origin) {

    //TODO: buscar más de un punto posible con etse método, y retornar el promedio "angular"

    int discoveranglestep=15;
    int testx, testy;

    int ii;
    for(ii=0; ii<=345; ii+=discoveranglestep) {
        testx = (int) (origin->x + (int)(fastcos(ii) * DISCOVER_RADIUS) );
        testy = (int) (origin->y - (int)(fastsin(ii) * DISCOVER_RADIUS) );

        if(testx > MAXW || testy > MAXH || testx < 0 || testy < 0) continue;

        //gdk_draw_point(canvas,context_drawing, testx, testy);

        hvpoint * testpoint = hv1_find_nearby(testx,testy);
        if(testpoint!=NULL) {
            if(!hv1_point_exists(testx,testy)) return testpoint;
        }

    }

    return NULL;

}

hvpoint* hv1_find_nearby(int fx, int fy) {

    int j,k;
    for(j=-1; j<=1; j++) {
        for(k=-1; k<=1; k++) {
            if(fx+j > MAXW || fy+j > MAXH || fx+j < 0 || fy+j < 0) continue;
            if(pixels[fx+j][fy+k]==1) return hv1_new_point(fx+j, fy+k, NULL);
        }
    }
    return NULL;
}




void hv1_delete_similar_points() {

    hvpoint * aux = pointlist, *aux2, *auxd;
    int found=0;
    int tolerance=8;

    while(pointlist && aux) {

        aux2=aux->next;
        found=0;
        while(aux2) {
            if((abs(aux2->x - aux->x) < tolerance ) && (abs(aux2->y - aux->y) < tolerance )) {
                auxd = aux;
                aux=aux->next;
                hv1_delete_point(auxd);
                found=1;
                break;
            }
            else {
                aux2=aux2->next;
            }
        }

        if(!found) aux=aux->next;
    }

}


void hv1_delete_erased_points() {

    hvpoint * aux = pointlist, *aux2;

    while(pointlist && aux ) {

        if(pixels[aux->x][aux->y]==0) {

            aux2 = aux;
            aux=aux->next;
            hv1_delete_point(aux2);

        }
        else {
            aux=aux->next;
        }

    }
}





void hv1_draw_points() {

    hvpoint* aux=pointlist;
    while(aux) {
        gdk_draw_arc(canvas,context_points, FALSE, aux->x-3, aux->y-3, 6, 6, 0, 23040);
        aux = aux->next;
    }

    g_print("%d points in pointlist\n", pointcount);
}

