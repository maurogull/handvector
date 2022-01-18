
#include "strokelist.h"


void hv2_clear_strokes() {

    hvpoint * aux = pointlist;
    while(aux) {
        aux->stroke=NULL;
        aux->next_s=NULL;
        aux->isnode=0;
        aux->isasymmetric=0;
        aux=aux->next;
    }

    hvstroke* aux2;
    while(strokelist) {

        if(strokelist->isclosed==1) {
            //TODO: delete last dummy point, not in point list. MEMORY LEAKKKKKKKKKKKKKKKKK
        }

        aux2=strokelist;
        strokelist=strokelist->next;
        g_free(aux2);
    }

    strokecount=0;

}


void hv2_add_stroke(hvstroke * s1) {

    if (strokelist==NULL) {
        strokelist = s1;
        s1->next=NULL;
        strokecount=1;
        return;
    }

    hvstroke * aux = strokelist;
    while(aux->next != NULL) aux=aux->next;
    aux->next = s1;
    s1->next=NULL;

    strokecount++;

}


hvstroke* hv2_new_stroke() {
    hvstroke* news=g_malloc(sizeof(hvstroke));
    news->next=NULL;
    news->points=NULL;
    news->isclosed=0;
    return news;
}

void hv2_add_point_to_stroke(hvpoint* newp, hvstroke* stroke, char order) {

    hvstroke* auxs = strokelist;

    while(auxs) {
        if(auxs==stroke) {

            hvpoint* auxp=auxs->points;

            newp->stroke = auxs; //reference from point to stroke

            if(auxp==NULL) { //first point of this stroke
                auxs->points=newp;
                return;
            }
            else {

                if(order==BEGINNING) {
                    newp->next_s = auxs->points;
                    auxs->points = newp;
                    return;

                }
                else { //ENDING
                    while(auxp->next_s != NULL) auxp=auxp->next_s;
                    auxp->next_s = newp;
                    newp->next_s = NULL;
                    return;
                }
            }

        }
        auxs=auxs->next;
    }

}


int hv2_strokefree_point_count() {
    if(pointlist==NULL) return 0;

    int ret=0;

    hvpoint * aux = pointlist;
    while(aux) {
        if(aux->stroke==NULL) ret++;
        aux=aux->next;
    }

    return ret;

}


hvpoint* hv2_first_strokefree_point() {
    if(pointlist==NULL) return NULL;

    hvpoint * aux = pointlist;
    while(aux) {
        if(aux->stroke==NULL) return aux;
        aux=aux->next;
    }

    return NULL;

}


void hv2_delete_stroke(hvstroke* s) {
    if(!s || !strokelist) return;

    if(strokelist==s) { //first one
        strokelist=s->next;
        g_free(s);
        return;
    }

    hvstroke * aux = strokelist->next;
    hvstroke * previous = strokelist;

    while(aux) {
        if(aux == s) {

            previous->next = aux->next;
            g_free(aux);

            strokecount--;
            return;
        }
        previous = aux;
        aux = aux->next;
    }
}


int hv2_stroke_point_count(hvstroke* s1) {
    if(strokelist==NULL) return 0;

    hvstroke * auxs=strokelist;

    while(auxs) {
        if(auxs==s1) {
            int points=0;
            hvpoint * auxp=auxs->points;

            while(auxp) { points++; auxp=auxp->next_s; }
            return points;
        }

        auxs=auxs->next;
    }

    return 0;
}
