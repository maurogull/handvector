#include "hv2.h"


/* module hv2, discover paths from hv1 point list */

void hv2_start_analysis() {

    if(pointlist==NULL) return;

    hv2_clear_strokes();

    hv2_discover_strokes();

    hv2_delete_poor_strokes();

    //hv1_print_list();

    //hv2_draw_strokes();
    //gtk_image_set_from_pixmap(mainimage,canvas,NULL);

}

void hv2_delete_poor_strokes() {

    if(strokelist==NULL) return;

    hvstroke* auxs=strokelist, *auxs2;
    hvpoint * auxp;
    int points;

    while(auxs) {

        auxs2 = auxs;
        auxs=auxs->next;

        if(hv2_stroke_point_count(auxs2) < 3) {
            hv2_delete_stroke(auxs2);
        }

    }

}


void hv2_discover_strokes() {

    while(hv2_strokefree_point_count() > 0) {
        hvstroke * auxs=hv2_new_stroke();
        hv2_add_stroke(auxs);

        hvpoint* first = hv2_first_strokefree_point(), *firstdup;
        hvpoint* nearest, *testing=first, *last;

        hv2_add_point_to_stroke(first,auxs, BEGINNING);
        last=first;

        while(nearest = hv2_nearest_alone_point(testing)) {
            nearest->inangle = hv_angle_between(nearest, last);

            last->outangle = hv_colinear_angle(nearest->inangle);

            hv2_add_point_to_stroke(nearest, auxs, ENDING);

            testing=nearest;
            last=nearest;
        }
        if( last!=first && hv2_are_close_points(first, last)  ){  //is closed path
            auxs->isclosed = 1;
            g_print("closed path\n");
            firstdup=hv1_duplicate_point(first);  //dummy closing point
            hv2_add_point_to_stroke(firstdup, auxs, ENDING);
        }


        //TODO: buscar en dirección opuesta al encontrado prviamente. si hay otro pero
        //no es opuesto, marcar como isnode=1 (seria una inflexión y asimétrica)
        if(!auxs->isclosed) {
            testing=first; //2nd try, from the same origin
            last=first;
            while(nearest = hv2_nearest_alone_point(testing)) {
                nearest->outangle = hv_angle_between(nearest, last);
                last->inangle = hv_colinear_angle(nearest->outangle);

                hv2_add_point_to_stroke(nearest, auxs, BEGINNING);
                testing=nearest;
                last=nearest;
            }

        }

    }

}

int hv2_are_close_points(hvpoint* first, hvpoint* second) {

    int cateto1=abs(first->x - second->x);
    int cateto2=abs(first->y - second->y);
    if(cateto1 <= (DISCOVER_RADIUS*1.6) && cateto2 <= (DISCOVER_RADIUS*1.6)) {
        return 1;
    }

    return 0;
}

hvpoint* hv2_nearest_alone_point(hvpoint* first) {
    if(pointlist==NULL) return NULL;

    hvpoint* aux=pointlist->next;
    hvpoint* best=pointlist;
    float radius, bestradius=9999;

    while(aux) {
        if(aux->stroke==NULL) {
            int cateto1=abs(aux->x - first->x);
            int cateto2=abs(aux->y - first->y);
            if(cateto1 > (DISCOVER_RADIUS*1.6) || cateto2 > (DISCOVER_RADIUS*1.6)) {
                aux=aux->next;
                continue;
            }

            radius = hv_points_distance(aux,first);
            if(radius < bestradius) {
                bestradius=radius;
                best=aux;
            }
        }

        aux=aux->next;

    }
    if(bestradius==9999) return NULL;
    return best;
}


hvpoint* hv2_previous_point(hvpoint* p) {
    if(p->stroke==NULL) return NULL;

    hvstroke * s=p->stroke;
    hvpoint * aux;

    if(s->points==p) { //first in stroke
        if(s->isclosed) { //is closed, return last (omit dummy last)
            aux = s->points->next_s;
            while(aux->next_s != p) aux=aux->next_s;
            return aux;
        }
        else return NULL;
    }

    aux = s->points->next_s;
    while(aux->next_s != p) aux=aux->next_s;
    return aux;

}

hvpoint * hv2_middle_point(const hvpoint * const p1, const hvpoint * const p2) {

    int middle = (hv2_points_between(p1,p2)+1) / 2;

    if(middle<3) return NULL;

    hvpoint * auxforward = p1;
    while(middle--) auxforward=auxforward->next_s;

    return auxforward;

}

int hv2_points_between(const hvpoint* const from, const hvpoint* const to) {

    hvpoint* auxp = from->next_s;
    int cont=0;
    while(auxp) {
        if(auxp==to) { return cont; }

        cont++;
        auxp=auxp->next_s;
    }

    return 0;

}



void hv2_draw_strokes() {

    int cont=0;
    hvstroke* auxs = strokelist;
    hvpoint* auxp=pointlist;

    while(auxs) {
        auxp=auxs->points;
        while(auxp) {

            if (auxp->isnode)
                gdk_draw_arc(canvas,context_path[cont], TRUE, auxp->x-4, auxp->y-4, 8, 8, 0, 23040);
            else
                gdk_draw_arc(canvas,context_path[cont], FALSE, auxp->x-2, auxp->y-2, 4, 4, 0, 23040);

            auxp = auxp->next_s;

        }
        auxs=auxs->next;
        cont++;
        if(cont>4) cont=0;
    }

    g_print("%d strokes in strokelist\n", strokecount);
}


