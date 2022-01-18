
#include "pointlist.h"



void hv1_add_point(hvpoint * p1) {

    if (pointlist==NULL) {
        pointlist = p1;
        p1->next=NULL;
        pointcount=1;
        return;
    }

    hvpoint * aux = pointlist;
    while(aux->next != NULL) aux=aux->next;
    aux->next = p1;
    p1->next=NULL;

    pointcount++;

}

void hv1_delete_point(hvpoint * p1) {
    if(!p1 || !pointlist) return;

    if(pointlist==p1) { //first one
        pointlist=p1->next;
        g_free(p1);
        pointcount--;
        return;
    }

    hvpoint * aux = pointlist->next;
    hvpoint * previous = pointlist;

    while(aux) {
        if(aux == p1) {

            previous->next = aux->next;
            g_free(aux);

            pointcount--;
            return;
        }
        previous = aux;
        aux = aux->next;
    }
}


hvpoint * hv1_new_point(unsigned int x, unsigned int y, hvpoint* next) {
    hvpoint* pnew=g_malloc(sizeof(hvpoint));
    pnew->x = x;
    pnew->y = y;
    pnew->next = next;
    pnew->next_s = NULL;
    pnew->stroke = NULL;
    pnew->isnode = 0;
    pnew->inangle = -1;
    pnew->outangle = -1;
    pnew->isasymmetric = 0;

    return pnew;
}

hvpoint * hv1_duplicate_point(hvpoint* orig) {
    hvpoint* pnew=g_malloc(sizeof(hvpoint));
    pnew->x = orig->x;
    pnew->y = orig->y;
    pnew->next = NULL;
    pnew->next_s = NULL;
    pnew->stroke = orig->stroke;
    pnew->isnode = orig->isnode;
    pnew->inangle = orig->inangle;
    pnew->outangle = orig->outangle;
    pnew->isasymmetric = orig->isasymmetric;

    return pnew;
}

int hv1_point_exists(int x, int y) {
    hvpoint * aux = pointlist;
    while(aux) {

        int j,k;
        for(j=-3; j<=3; j++) {
            for(k=-3; k<=3; k++) {
                if(x+j > MAXW || y+j > MAXH || x+j < 0 || y+j < 0) continue;
                if(aux->x==x+j && aux->y==y+k) return 1;
            }
        }

        aux=aux->next;
    }
    return 0;
}





void hv1_print_list() {

    hvpoint* aux=pointlist;
    while(aux) {
        g_print("%d@%d inangle%d outangle%d", aux->x, aux->y, aux->inangle, aux->outangle);

        if(aux->next != NULL) g_print(" next: %d@%d", aux->next->x, aux->next->y);
        if(aux->next_s != NULL) g_print(" next_s: %d@%d", aux->next_s->x, aux->next_s->y);

        g_print("\n");

        aux=aux->next;
    }
    g_print("%d points \n", pointcount);
}
