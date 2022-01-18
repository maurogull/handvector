#include "hv3.h"

/* module hv3, discover nodes in hv2 stroke list & calculate beziers :) */


void hv3_start_analysis() {

    if(pointlist==NULL || strokelist==NULL) return;

    hv3_discover_inflections();

    hv3_set_first_last_nodes();
    hv3_delete_sibling_nodes();

    hv3_clear_all_beziers();
    hv3_discover_beziers();
    //hv3_split_difficult_ones();
    hv3_discover_beziers_last_chance();

}

void hv3_discover_inflections() {

    hvstroke* s = strokelist;

    while(s) {

        if(hv2_stroke_point_count(s) > 3) {

            hvpoint * pos1 = s->points;
            hvpoint * pos2 = s->points->next_s;
            hvpoint * pos, *prev;

            char vert=0, horiz=0, vertprev=0, horizprev=0;

            if(pos1->x == pos2->x) { horizprev=0;}
            if(pos2->x > pos1->x) { horizprev=1;}
            if(pos2->x < pos1->x) { horizprev=-1;}
            if(pos1->y == pos2->y) { vertprev=0;}
            if(pos2->y > pos1->y) { vertprev=-1;}
            if(pos2->y < pos1->y) { vertprev=1;}

            pos = pos2->next_s;
            prev = pos2;
            while(pos) {
                if(prev->x == pos->x) { horiz=0;}
                if(pos->x > prev->x) { horiz=1;}
                if(pos->x < prev->x) { horiz=-1;}

                if(prev->y == pos->y) { vert=0;}
                if(pos->y > prev->y) { vert=-1;}
                if(pos->y < prev->y) { vert=1;}

                if(vertprev!=vert || horizprev!=horiz) { prev->isnode=1; }

                if(pos->inangle!=-1 && pos->outangle!=-1) {
                    int angledif=fastabs(fastabs(pos->inangle - pos->outangle) - 180); //cuanto se aleja de 180°
                    if(angledif >= 30) pos->isnode=1;
                    if(angledif >= 45) pos->isasymmetric=1;
                }

                vertprev=vert;
                horizprev=horiz;

                prev=pos;
                pos=pos->next_s;

            }

        }

        s=s->next;
    }

}

void hv3_set_first_last_nodes() {
    hvstroke* s = strokelist;

    while(s) {
        if(hv2_stroke_point_count(s) > 3) {
            s->points->isnode = 1;
            if(!s->isclosed) s->points->isasymmetric = 1;


            hvpoint* p = s->points;
            while(p->next_s) p=p->next_s;

            p->isnode=1;
            if(!s->isclosed) p->isasymmetric=1;
        }
        s=s->next;
    }
}

void hv3_delete_sibling_nodes() {

    hvstroke* s = strokelist;

    while(s) {
        if(hv2_stroke_point_count(s) > 3) {

            hvpoint* p = s->points;
            while(p && p->next_s && p->next_s->next_s) {

                if(p->isnode==1 && p->next_s->isnode==1 && p->next_s->next_s->isnode==1) {
                    if(p->next_s->isasymmetric==0) {
                        p->next_s->isnode=0;
                        p=p->next_s; //avanzará dos al combinar con el siguiente next_s, its ok
                    }
                }

                p=p->next_s;
            }
        }
        s=s->next;
    }
}

void hv3_clear_all_beziers() {
    hvpoint * auxp = pointlist;
    while(auxp) {
        auxp->outhanger_x= -10; auxp->outhanger_y= -10;
        auxp->inhanger_x = -10; auxp->inhanger_y = -10;
        auxp=auxp->next;
    }
}


int inline hv3_points_in_same_side( hvpoint* p1, hvpoint* p2) {
    return 1;
    //TODO : no está funcionando bien...
    int steps = hv2_points_between(p1,p2);
    float stepbezier = 1.0 / (steps+1.0);
    int countside=0;

    hvpoint* auxp=p1;
    int i;
    for(i=1; i<=steps; i++) {
        float resx, resy;

        //linear bezier  :)
        resx = (1.0 - (i*stepbezier)) * p1->x + (i*stepbezier) * p2->x;
        resy = (1.0 - (i*stepbezier)) * p1->y + (i*stepbezier) * p2->y;

        auxp = auxp->next_s;
        if ( auxp->y > (int)resy ) countside++;
    }

    if ( countside == steps || countside==0 ) return 1;
    return 0;
}


void hv3_discover_beziers() {

    float bezier_tolerance = 10.0;

    hvstroke* auxs = strokelist;
    hvpoint * auxp = pointlist;
    while(auxs) {
        hvpoint *p1, *p2, *prev;

        prev = auxs->points;
        auxp = prev->next_s;
        while(auxp) {

            if(auxp->next_s==NULL && auxs->isclosed==1) { //is last dummy node? copy hangers from first
                auxp->outhanger_x=auxs->points->outhanger_x;
                auxp->outhanger_y=auxs->points->outhanger_y;
                auxp->inhanger_x=auxs->points->inhanger_x;
                auxp->inhanger_y=auxs->points->inhanger_y;
            }

            if(auxp->isnode==1 && auxp->inhanger_x < -1 && prev->outhanger_y < -1 ) {

                if(hv_is_straight_segment(prev, auxp)) { //straight segment
                    auxp->inhanger_x= -1; auxp->inhanger_y= -1;
                    prev->outhanger_x = -1; prev->outhanger_y = -1;
                }
                else if(hv3_points_in_same_side(prev, auxp))  { //non-straight segment. simple cubic bezier may be?

                    float dist = hv_points_distance(prev,auxp);
                    bezier_tolerance = 2.0 + (dist * 0.02);

                    int best_prev_x, best_prev_y,best_auxp_x, best_auxp_y;
                    float testerror;
                    hv3_bezier_best_fit_ortho(prev, auxp, &best_prev_x, &best_prev_y,&best_auxp_x, &best_auxp_y, &testerror);

                    if(testerror > bezier_tolerance || (best_prev_x==prev->x && best_auxp_x==auxp->x) || (best_prev_y==prev->y && best_auxp_y==auxp->y) ) {
                        //should try with asymmetric if possible , or split curve
                    }
                    else {
                        prev->outhanger_x= best_prev_x; prev->outhanger_y= best_prev_y;
                        auxp->inhanger_x = best_auxp_x; auxp->inhanger_y = best_auxp_y;
                    }

                } else {
                    //should split curve
                }

                prev=auxp;
            }

            auxp=auxp->next_s;

        }

        auxs=auxs->next;
    }

}

void hv3_populate_possible_hangers(int angle, int mindist, int maxdist, int steps, int* xvector, int* yvector) {
    int i, step = (maxdist-mindist) / steps;

    for(i=0; i<steps; i++) {
        xvector[i] = (int)(fastcos(angle) * (mindist + step*i));
        yvector[i] = (int)(fastsin(angle) * (mindist + step*i));
    }
}


void hv3_bezier_best_fit_ortho(const hvpoint* const p1, const hvpoint* const p2,int* best_prev_x, int* best_prev_y,int* best_auxp_x, int* best_auxp_y, float* besterror) {

    int best_x1=0, best_y1=0, best_x2=0, best_y2=0 ;
    float temperror2,temperror=9999.9;



    int try1x[6]; int try1y[6]; int try2x[6]; int try2y[6];
    int steps=6, i, j, found=0;


    if(p1->x < p2->x && p1->y > p2->y) { //ok
        // _/  p1 a la izquierda abajo de p2
        hv3_populate_possible_hangers(0,10,60,steps,try1x,try1y);
        hv3_populate_possible_hangers(270,10,60,steps,try2x,try2y);
        for(i=0;i<steps;i++) {
            for(j=0;j<steps;j++) {
                temperror2 = hv_bezier_error(p1,p2,p1->x+try1x[i],p1->y-try1y[i],p2->x+try2x[j],p2->y-try2y[j]);
                if(temperror2<temperror) {
                    temperror=temperror2; best_x1=p1->x+try1x[i]; best_y1=p1->y-try1y[i]; best_x2=p2->x+try2x[j]; best_y2=p2->y-try2y[j];
                }
            }
        }

        // /-
        hv3_populate_possible_hangers(90,10,60,steps,try1x,try1y);
        hv3_populate_possible_hangers(180,10,60,steps,try2x,try2y);
        for(i=0;i<steps;i++) {
            for(j=0;j<steps;j++) {
                temperror2 = hv_bezier_error(p1,p2,p1->x+try1x[i],p1->y-try1y[i],p2->x+try2x[j],p2->y-try2y[j]);
                if(temperror2<temperror) {
                    temperror=temperror2; best_x1=p1->x+try1x[i]; best_y1=p1->y-try1y[i]; best_x2=p2->x+try2x[j]; best_y2=p2->y-try2y[j];
                }
            }
        }
    }

    if(p1->x < p2->x && p1->y < p2->y) { //ok
    // -\           .p1 a la izquierda arriba de p2
        hv3_populate_possible_hangers(0,10,60,steps,try1x,try1y);
        hv3_populate_possible_hangers(90,10,60,steps,try2x,try2y);
        for(i=0;i<steps;i++) {
            for(j=0;j<steps;j++) {
                temperror2 = hv_bezier_error(p1,p2,p1->x+try1x[i],p1->y-try1y[i],p2->x+try2x[j],p2->y-try2y[j]);
                if(temperror2<temperror) {
                    temperror=temperror2; best_x1=p1->x+try1x[i]; best_y1=p1->y-try1y[i]; best_x2=p2->x+try2x[j]; best_y2=p2->y-try2y[j];
                }
            }
        }

        // \_
        hv3_populate_possible_hangers(270,10,60,steps,try1x,try1y);
        hv3_populate_possible_hangers(180,10,60,steps,try2x,try2y);
        for(i=0;i<steps;i++) {
            for(j=0;j<steps;j++) {
                temperror2 = hv_bezier_error(p1,p2,p1->x+try1x[i],p1->y-try1y[i],p2->x+try2x[j],p2->y-try2y[j]);
                if(temperror2<temperror) {
                    temperror=temperror2; best_x1=p1->x+try1x[i]; best_y1=p1->y-try1y[i]; best_x2=p2->x+try2x[j]; best_y2=p2->y-try2y[j];
                }
            }
        }
    }


    if(p1->x > p2->x && p1->y < p2->y) { //ok
        // p1 a la derecha arriba p2
        hv3_populate_possible_hangers(270,10,60,steps,try1x,try1y);
        hv3_populate_possible_hangers(0,10,60,steps,try2x,try2y);
        for(i=0;i<steps;i++) {
            for(j=0;j<steps;j++) {
                temperror2 = hv_bezier_error(p1,p2,p1->x+try1x[i],p1->y-try1y[i],p2->x+try2x[j],p2->y-try2y[j]);
                if(temperror2<temperror) {
                    temperror=temperror2; best_x1=p1->x+try1x[i]; best_y1=p1->y-try1y[i]; best_x2=p2->x+try2x[j]; best_y2=p2->y-try2y[j];
                }
            }
        }

        // /-
        hv3_populate_possible_hangers(180,10,60,steps,try1x,try1y);
        hv3_populate_possible_hangers(90,10,60,steps,try2x,try2y);
        for(i=0;i<steps;i++) {
            for(j=0;j<steps;j++) {
                temperror2 = hv_bezier_error(p1,p2,p1->x+try1x[i],p1->y-try1y[i],p2->x+try2x[j],p2->y-try2y[j]);
                if(temperror2<temperror) {
                    temperror=temperror2; best_x1=p1->x+try1x[i]; best_y1=p1->y-try1y[i]; best_x2=p2->x+try2x[j]; best_y2=p2->y-try2y[j];
                }
            }
        }
    }

    if(p1->x > p2->x && p1->y > p2->y) { //ok
    // -\           .  p1 a la derecha abajo de p2
        hv3_populate_possible_hangers(90,10,60,steps,try1x,try1y);
        hv3_populate_possible_hangers(0,10,60,steps,try2x,try2y);
        for(i=0;i<steps;i++) {
            for(j=0;j<steps;j++) {
                temperror2 = hv_bezier_error(p1,p2,p1->x+try1x[i],p1->y-try1y[i],p2->x+try2x[j],p2->y-try2y[j]);
                if(temperror2<temperror) {
                    temperror=temperror2; best_x1=p1->x+try1x[i]; best_y1=p1->y-try1y[i]; best_x2=p2->x+try2x[j]; best_y2=p2->y-try2y[j];
                }
            }
        }

        // \_
        hv3_populate_possible_hangers(180,10,60,steps,try1x,try1y);
        hv3_populate_possible_hangers(270,10,60,steps,try2x,try2y);
        for(i=0;i<steps;i++) {
            for(j=0;j<steps;j++) {
                temperror2 = hv_bezier_error(p1,p2,p1->x+try1x[i],p1->y-try1y[i],p2->x+try2x[j],p2->y-try2y[j]);
                if(temperror2<temperror) {
                    temperror=temperror2; best_x1=p1->x+try1x[i]; best_y1=p1->y-try1y[i]; best_x2=p2->x+try2x[j]; best_y2=p2->y-try2y[j];
                }
            }
        }
    }



    *best_prev_x = best_x1;
    *best_prev_y = best_y1;

    *best_auxp_x = best_x2;
    *best_auxp_y = best_y2;

    *besterror = temperror;

}





void hv3_split_difficult_ones() {

    hvstroke* auxs = strokelist;
    hvpoint * auxp = pointlist, *prev, *auxforward;
    while(auxs) {

        prev = auxs->points;
        auxp = prev->next_s;

        while(auxp) {
            if(auxp->isnode==1) {

                if(auxp->inhanger_x < -1 && prev->outhanger_y < -1) {
                    hvpoint* middle = hv2_middle_point(prev,auxp), *middle2;
                    if (middle) {
                        middle->isnode=1;

                        char fixed1st=0, fixed2nd=0;
                        if(hv_is_straight_segment(prev, middle)) { //straight 1st segment?
                            middle->inhanger_x= -1; middle->inhanger_y= -1;
                            prev->outhanger_x = -1; prev->outhanger_y = -1;
                            fixed1st++;
                        }
                        if(hv_is_straight_segment(middle, auxp)) { //straight 2nd segment?
                            middle->outhanger_x = -1; middle->outhanger_y = -1;
                            auxp->inhanger_x= -1; auxp->inhanger_y= -1;
                            fixed2nd++;
                        }

                        if(fixed1st && !fixed2nd && (middle2 = hv2_middle_point(middle,auxp))) {
                            middle2->isnode=1;
                            hv3_find_diagonal_hangers(middle,middle2,auxp);
                        }
                        else if (!fixed1st && !fixed2nd && (middle2 = hv2_middle_point(prev,middle)) ) {
                            middle2->isnode=1;
                            hv3_find_diagonal_hangers(prev,middle2,middle);
                        }
                        else if (!fixed1st && !fixed2nd)  { //is a curve-curve

                            hv3_find_diagonal_hangers(prev,middle,auxp);

                        }
                    }
                }

                prev=auxp;
            }

            auxp=auxp->next_s;
        }

        auxs=auxs->next;
    }

}



void hv3_find_diagonal_hangers(hvpoint * p1,hvpoint * mid,hvpoint * p2) {

    int angle, dist;
    int testmidout_x, testmidout_y, testmidin_x, testmidin_y;
    int colinear1_test, colinear2_test;
    int tempp1_x, tempp1_y, tempp2_x, tempp2_y;
    float tolerance = 8.0, erroracum, besterror=9999.9;

    int best_p1_outhanger_x,best_p1_outhanger_y,best_mid_inhanger_x, best_mid_inhanger_y,best_mid_outhanger_x,best_mid_outhanger_y, best_p2_inhanger_x, best_p2_inhanger_y;

    int p1free_colinear_x[30];
    int p1free_colinear_y[30];
    int p2free_colinear_x[30];
    int p2free_colinear_y[30];
    int i;

    for(i=0; i<30;i++) {    p1free_colinear_x[i]=0; p1free_colinear_y[i]=0;
                            p2free_colinear_x[i]=0; p2free_colinear_y[i]=0; }

    hvpoint * auxprev, *auxnext, auxpoint;
    int auxangle;

    //first, deduce p1 and p2 missing hangers. take their existent hangers (if any) and make colinear.
    //TODO: que pasa si es primer o ultimo nodo? angulo seria libre!
    if (p1->inhanger_x == p1->x) { for(i=0; i<30;i++) { p1free_colinear_x[i]=0; p1free_colinear_y[i]=10+3*i; } }
    else if (p1->inhanger_y == p1->y) { for(i=0; i<30;i++) { p1free_colinear_x[i]=10+3*i; p1free_colinear_y[i]=0; } }
    else if (p1->inhanger_x < 0 && p1->inhanger_y < 0 && (auxprev=hv3_previous_node(p1)) ) {
        g_print("p1 no hangers, prev node \n");
        auxangle=hv_angle_between(p1,auxprev);
        g_print("angle %d\n", auxangle);
        for(i=0; i<30;i++) {
            p1free_colinear_y[i] = (fastsin(auxangle-180) * (10+3*i));
            p1free_colinear_x[i] = (fastcos(auxangle-180) * (10+3*i));
        }
    }
    /*else if(p1->inhanger_x>=0 && p1->inhanger_y>=0) {  //hanger diagonal
        g_print("p1 diagonal prev hanger\n");
        auxpoint.x=p1->inhanger_x; auxpoint.y=p1->inhanger_y;
        auxangle=hv3_angle_between(p1,&auxpoint);
        for(i=0; i<30;i++) {
            p1free_colinear_y[i]=fastabs(fastsin(auxangle) * (10+3*i));
            p1free_colinear_x[i]=fastabs(fastcos(auxangle) * (10+3*i));
        }
    }*/
    else if(0) { //no hanger, try free angle
        //auxprev=hv2_previous_point(p2)
    }
    else {

    }

    if (p2->outhanger_x == p2->x) { for(i=0; i<30;i++) { p2free_colinear_x[i]=0; p2free_colinear_y[i]=10+3*i; } }
    else if (p2->outhanger_y == p2->y) { for(i=0; i<30;i++) { p2free_colinear_x[i]=10+3*i; p2free_colinear_y[i]=0; } }
    else if (p2->outhanger_x < 0 && p2->outhanger_y < 0 && (auxnext=hv3_next_node(p2)) ) {
        g_print("p2 no hangers, next node \n");
        auxangle=hv_angle_between(auxnext, p2);
        g_print("angle %d\n", auxangle);
        for(i=0; i<30;i++) {
            p2free_colinear_y[i] = (fastsin(auxangle-180) * (10+3*i));
            p2free_colinear_x[i] = (fastcos(auxangle-180) * (10+3*i));
        }
    }
   /* else if(p2->outhanger_x>=0 && p2->outhanger_y>=0) {  //hanger diagonal
        g_print("p2 diagonal next hanger\n");
        auxpoint.x=p2->outhanger_x; auxpoint.y=p2->outhanger_y;
        auxangle=hv3_angle_between(p2,&auxpoint);
        for(i=0; i<30;i++) {
            p2free_colinear_y[i]=fastabs(fastsin(auxangle) * (10+3*i));
            p2free_colinear_x[i]=fastabs(fastcos(auxangle) * (10+3*i));
        }
    }*/
    else if(0) { //no hanger, try free angle
        //auxprev=hv2_previous_point(p2)
    }
    else {

    }

    int fast_prev1x=-1,fast_prev1y=-1,fast_prev2x=-1,fast_prev2y=-1; //omit repetitive colinear table

    //now, try angles and distances. find the best combination.
    for(angle=0; angle<360; angle+=10) {
        for(dist=10; dist<=70; dist+=10) {
            for(colinear1_test=0; colinear1_test < 30; colinear1_test++) {

                //if(fast_prev1x==p1free_colinear_x[colinear1_test] && fast_prev1y==p1free_colinear_y[colinear1_test] ) break;
                //else { fast_prev1x=p1free_colinear_x[colinear1_test];fast_prev1y=p1free_colinear_y[colinear1_test] ; }

                for(colinear2_test=0; colinear2_test < 30; colinear2_test++) {

                    if(fast_prev2x==p2free_colinear_x[colinear2_test] && fast_prev2y==p2free_colinear_y[colinear2_test] ) break;
                    else { fast_prev2x=p2free_colinear_x[colinear2_test];fast_prev2y=p2free_colinear_y[colinear2_test] ; }

                    testmidout_x = mid->x + (int)(fastcos(angle)*dist);
                    testmidout_y = mid->y + (int)(fastsin(angle)*dist);
                    testmidin_x = mid->x + (int)(fastcos(angle+180)*dist);
                    testmidin_y = mid->y + (int)(fastsin(angle+180)*dist);

                    tempp1_x=p1->x + p1free_colinear_x[colinear1_test];
                    tempp1_y=p1->y + p1free_colinear_y[colinear1_test];
                    tempp2_x=p2->x + p2free_colinear_x[colinear2_test];
                    tempp2_y=p2->y + p2free_colinear_y[colinear2_test];

                    erroracum=hv_bezier_error(p1, mid, tempp1_x,tempp1_y,testmidin_x,testmidin_y);
                    erroracum+=hv_bezier_error(mid, p2, testmidout_x,testmidout_y,tempp2_x,tempp2_y);

                    if(erroracum <= tolerance && erroracum < besterror) {
                        besterror=erroracum;
                        best_p1_outhanger_x=tempp1_x; best_p1_outhanger_y=tempp1_y;
                        best_mid_inhanger_x=testmidin_x; best_mid_inhanger_y=testmidin_y;
                        best_mid_outhanger_x=testmidout_x; best_mid_outhanger_y=testmidout_y;
                        best_p2_inhanger_x=tempp2_x; best_p2_inhanger_y=tempp2_y;
                    }
                }
            }
        }
    }

    //voila
    if(besterror < tolerance) {
        p1->outhanger_x=best_p1_outhanger_x; p1->outhanger_y=best_p1_outhanger_y;
        mid->inhanger_x=best_mid_inhanger_x; mid->inhanger_y=best_mid_inhanger_y;
        mid->outhanger_x=best_mid_outhanger_x; mid->outhanger_y=best_mid_outhanger_y;
        p2->inhanger_x=best_p2_inhanger_x; p2->inhanger_y=best_p2_inhanger_y;
    }
}



//TODO: check this for closed paths
hvpoint* hv3_previous_node(hvpoint* p) {
    if(p->stroke==NULL) return NULL;

    hvstroke * s=p->stroke;
    hvpoint * aux, *last=NULL;

    if(s->points==p) { //first in stroke
        if(s->isclosed) { //is closed, return last (omit dummy last)
            aux = s->points->next_s;
            while(aux->next_s->next_s != NULL) {
                if(aux->isnode) last=aux;
                aux=aux->next_s;
            }
            if(aux->isnode) return aux;
            return last;
        }
        else return NULL;
    }

    last = s->points;
    aux = s->points;
    while(aux->next_s != p) {
        if(aux->isnode) last=aux;
        aux=aux->next_s;
    }
    if(aux->isnode) return aux;
    return last;

}


hvpoint* hv3_next_node(hvpoint* p) {
    if(p->stroke==NULL) return NULL;

    hvstroke * s=p->stroke;
    hvpoint * aux;
    char past=0;

    aux = s->points;
    while(aux) {

        if(past && aux->isnode) return aux;
        if(aux==p) past=1;

        aux=aux->next_s;
    }

    if(!past) return NULL;

    if(s->isclosed) {
        return s->points;
    }

    return NULL;
}


void hv3_discover_beziers_last_chance() {


}


void hv3_draw_nodes(cairo_t * cr) {

    int cont=0;
    hvstroke* auxs = strokelist;
    hvpoint* auxp=pointlist;

    cairo_set_line_width (cr, 1);
    cairo_set_source_rgb (cr, 0, 0, 0);

    hvpoint * prev;
    while(auxs) {

        auxp=auxs->points;
        prev = NULL;

        while(auxp) {

            if (auxp->isnode) {
                if(prev) { //omit first node on stroke.

                    if(prev->outhanger_x==-1 && prev->outhanger_y==-1 && auxp->inhanger_x==-1 && auxp->inhanger_y==-1 ){ //straight line
                        cairo_set_source_rgb (cr, 0, 0, 0);
                        cairo_move_to(cr, prev->x, prev->y);
                        cairo_line_to(cr, auxp->x, auxp->y);
                        cairo_stroke(cr);
                    }
                    else if(prev->outhanger_x < -1 && prev->outhanger_y < -1 && auxp->inhanger_x < -1 && auxp->inhanger_y < -1) { //error bezier!
                        cairo_set_source_rgb (cr, 20, 0, 0);
                        cairo_move_to(cr, prev->x, prev->y);
                        cairo_line_to(cr, auxp->x, auxp->y);
                        cairo_stroke(cr);

                    }
                    else { //bezier
                        cairo_set_source_rgb (cr, 0, 0, 0);
                        cairo_move_to(cr, prev->x, prev->y);
                        cairo_curve_to(cr, prev->outhanger_x, prev->outhanger_y, auxp->inhanger_x, auxp->inhanger_y, auxp->x, auxp->y);
                        //cairo_fill(cr);
                        cairo_stroke(cr);

                        if(1==1) {  //display bezier hangers
                            static const double dashedline[] = {2.0, 1.0};
                            static int lendashed  = sizeof(dashedline) / sizeof(dashedline[0]);


                            cairo_set_source_rgb (cr, 0, 0, 200); //blue outhanger
                            cairo_arc(cr, prev->outhanger_x, prev->outhanger_y, 2, 0, 2 * PI);
                            cairo_fill(cr);
                            cairo_set_dash(cr, dashedline, lendashed, 1);
                            cairo_move_to(cr,prev->x, prev->y);
                            cairo_line_to(cr, prev->outhanger_x, prev->outhanger_y);
                            cairo_stroke(cr);


                            cairo_set_source_rgb (cr, 200, 0, 0); //red inhanger
                            cairo_arc(cr, auxp->inhanger_x, auxp->inhanger_y, 2, 0, 2 * PI);
                            cairo_fill(cr);
                            cairo_set_dash(cr, dashedline, lendashed, 1);
                            cairo_move_to(cr,auxp->x, auxp->y);
                            cairo_line_to(cr, auxp->inhanger_x, auxp->inhanger_y);
                            cairo_stroke(cr);
                            cairo_set_dash(cr, NULL, NULL, 0);

                        }
                    }
                }

                //node point

                cairo_set_source_rgb (cr, 0, 0, 0);
                cairo_arc(cr, auxp->x, auxp->y, 2, 0, 2 * PI);
                cairo_fill(cr);

                prev = auxp;
            }

            auxp = auxp->next_s;

        }
        auxs=auxs->next;
        cont++;
        if(cont>4) cont=0;
    }

}
