#include <assert.h>

#include "simple_logger.h"

#include "gfc_config.h"

#include "gfc_shape.h"

Uint8 gfc_edge_circle_intersection_poc_old(Edge e,Circle c,Vector2D *poc,Vector2D *normal);
Uint8 gfc_edge_to_circle_intersection_poc(Edge e,Circle c,Vector2D *poc,Vector2D *normal);
Uint8 gfc_circle_to_edge_intersection_poc(Edge e,Circle c,Vector2D *poc,Vector2D *normal);

Vector2D gfc_rect_get_center_point(Rect r)
{
    return vector2d(r.x + r.w*0.5,r.y + r.h*0.5);
}

Vector2D gfc_edge_get_normal_for_rect(Edge e, Rect r)
{
    Vector2D out = {0};
    Vector2D c = {0};
    Vector2D n1,n2,p1,p2;
    Vector2D dir = {0};
    Vector2D parallel = {0};
    
    p1 = vector2d(e.x1,e.y1);
    p2 = vector2d(e.x2,e.y2);

    vector2d_sub(n1,p2,p1);
    vector2d_copy(parallel,n1);
    n1.x = p2.y - p1.y;
    n1.y = p1.x - p2.x;
    vector2d_scale(parallel,parallel,r.w+r.h);
    
    vector2d_normalize(&n1);
    vector2d_negate(n2,n1);
    c = gfc_rect_get_center_point(r);
    
    if (gfc_point_in_rect(vector2d(e.x1,e.y1),r))
    {
        out.x = e.x1 - e.x2;
        out.y = e.y1 - e.y2;
        vector2d_normalize(&out);
        return out;
    }
    if (gfc_point_in_rect(vector2d(e.x2,e.y2),r))
    {
        out.x = e.x2 - e.x1;
        out.y = e.y2 - e.y1;
        vector2d_normalize(&out);
        return out;
    }
    vector2d_scale(dir,n1,(r.w + r.h));
    if (gfc_edge_intersect(gfc_edge(p1.x - parallel.x,p1.y - parallel.y,p2.x + parallel.x,p2.y + parallel.y),
                            gfc_edge(c.x, c.y, c.x + dir.x, c.y + dir.y)))
    {
        return n2;
    }
    vector2d_scale(dir,n2,(r.w + r.h));
    if (gfc_edge_intersect(gfc_edge(p1.x - parallel.x,p1.y - parallel.y,p2.x + parallel.x,p2.y + parallel.y),
                            gfc_edge(c.x, c.y, c.x + dir.x, c.y + dir.y)))
    {
        return n1;
    }
    return out;
}

Vector2D gfc_rect_get_normal_for_edge(Rect r,Edge e)
{
    Vector2D out = {0};
    Vector2D c = {0};
    Vector2D n1,n2,p1,p2;
    Vector2D dir = {0};
    Vector2D parallel = {0};
    
    p1 = vector2d(e.x1,e.y1);
    p2 = vector2d(e.x2,e.y2);

    vector2d_sub(n1,p2,p1);
    vector2d_copy(parallel,n1);
    n1.x = p2.y - p1.y;
    n1.y = p1.x - p2.x;
    vector2d_scale(parallel,parallel,r.w+r.h);
    
    vector2d_normalize(&n1);
    vector2d_negate(n2,n1);
    c = gfc_rect_get_center_point(r);
    
    if (gfc_point_in_rect(vector2d(e.x1,e.y1),r))
    {
        out.x = e.x1 - e.x2;
        out.y = e.y1 - e.y2;
        vector2d_normalize(&out);
        return out;
    }
    if (gfc_point_in_rect(vector2d(e.x2,e.y2),r))
    {
        out.x = e.x2 - e.x1;
        out.y = e.y2 - e.y1;
        vector2d_normalize(&out);
        return out;
    }
    vector2d_scale(dir,n1,(r.w + r.h));
    if (gfc_edge_intersect(gfc_edge(p1.x - parallel.x,p1.y - parallel.y,p2.x + parallel.x,p2.y + parallel.y),
                            gfc_edge(c.x, c.y, c.x + dir.x, c.y + dir.y)))
    {
        return n2;
    }
    vector2d_scale(dir,n2,(r.w + r.h));
    if (gfc_edge_intersect(gfc_edge(p1.x - parallel.x,p1.y - parallel.y,p2.x + parallel.x,p2.y + parallel.y),
                            gfc_edge(c.x, c.y, c.x + dir.x, c.y + dir.y)))
    {
        return n1;
    }
    return out;
}

Vector2D gfc_circle_get_normal_for_rect(Circle c, Rect r)
{
    Vector2D out = {0};
    Vector2D poc = {0};
    if (!gfc_circle_rect_overlap_poc(c, r,&poc,NULL))
    {
        slog("can't calculate normal, no collision!");
        return out;
    }
    vector2d_sub(out,poc,c);
    vector2d_normalize(&out);
    return out;
}

Vector2D gfc_rect_get_normal_for_rect(Rect r, Rect ref)
{
    Vector2D out = {0};
    if (ref.x + 1 >= (r.x + r.w))
    {
        out.x = 1;
        return out;
    }
    if ((ref.x + ref.w - 1) <= r.x)
    {
        out.x = -1;
        return out;
    }
    if (ref.y + 1 >= (r.y + r.h))
    {
        out.y = 1;
        return out;
    }
    if ((ref.y + ref.h - 1) <= r.y)
    {
        out.y = -1;
        return out;
    }
    return out;
}

Vector2D gfc_rect_get_normal_for_cirlce(Rect r, Circle c)
{
    Vector2D out = {0};
    if (c.x < r.x)out.x = -1;
    if (c.y < r.y)out.y = -1;
    if (c.x > r.x + r.w)out.x = 1;
    if (c.y > r.y + r.h)out.y = 1;
    if ((out.x != 0)&&(out.y != 0))
    {
        if ((out.x < 0)&&(out.y < 0))
        {
            out.x = c.x - r.x;
            out.y = c.y - r.y;
        }
        else if ((out.x > 0)&&(out.y < 0))
        {
            out.x = c.x - (r.x + r.w);
            out.y = c.y - r.y;
        }
        else if ((out.x < 0)&&(out.y > 0))
        {
            out.x = c.x - r.x;
            out.y = c.y - (r.y + r.h);// this breaks without the parenthesis, I HAVE NO IDEA WHY

        }
        else if ((out.x > 0)&&(out.y > 0))
        {
            out.x = c.x - (r.x + r.w);
            out.y = c.y - (r.y + r.h);
        }
        vector2d_normalize(&out);
        // edge case where it has to be perfect 
        // check angle between the corner and the c, if its not damn near perfect 45, pick the dominant side
    }
    if ((out.x) ||(out.y))return out;
    if (gfc_point_in_rect(vector2d(c.x,c.y),r))
    {
        out = gfc_rect_get_center_point(r);
        vector2d_sub(out,c,out);
        return out;
    }
    return out;
}

Vector2D gfc_circle_get_normal_for_cirlce(Circle c, Circle c2)
{
    Vector2D out = {0};
    vector2d_sub(out,c2,c);
    vector2d_normalize(&out);
    return out;
}

Vector2D gfc_edge_get_normal_for_cirlce(Edge e, Circle c)
{
    Vector2D out = {0};
    Vector2D dir = {0};
    Vector2D p1,p2;
    Vector2D n1,n2;
    Vector2D parallel = {0};
    p1 = vector2d(e.x1,e.y1);
    p2 = vector2d(e.x2,e.y2);
        
    vector2d_sub(n1,p2,p1);
    vector2d_copy(parallel,n1);
    n1.x = p2.y - p1.y;
    n1.y = p1.x - p2.x;
    
    vector2d_normalize(&n1);
    vector2d_negate(n2,n1);
    
    vector2d_scale(parallel,parallel,c.r-1);
    vector2d_scale(dir,n1,(c.r));
    if (gfc_edge_intersect(gfc_edge(p1.x - parallel.x,p1.y - parallel.y,p2.x + parallel.x,p2.y + parallel.y),gfc_edge(c.x, c.y, c.x + dir.x, c.y + dir.y)))
    {
        return n2;
    }
    vector2d_scale(dir,n2,(c.r));
    if (gfc_edge_intersect(gfc_edge(p1.x - parallel.x,p1.y - parallel.y,p2.x + parallel.x,p2.y + parallel.y),gfc_edge(c.x, c.y, c.x + dir.x, c.y + dir.y)))
    {
        return n1;
    }
    c.r += 1;
    if (gfc_point_in_cicle(p1,c))
    {
        out.x = c.x - p1.x;
        out.y = c.y - p1.y;
        vector2d_normalize(&out);
        return out;
    }
    if (gfc_point_in_cicle(p2,c))
    {
        out.x = c.x - p2.x;
        out.y = c.y - p2.y;
        vector2d_normalize(&out);
        return out;
    }
    return out;
}

Vector2D gfc_edge_get_normal_for_edge(Edge e1,Edge e2)
{
    Vector2D out = {0};
    if (!gfc_edge_intersect_poc(e1,e2,NULL,&out))
    {
        return out;
    }
    return out;
}

Uint8 gfc_shape_compare(Shape a, Shape b)
{
    if (a.type != b.type)return 0;
    switch(a.type)
    {
        case ST_RECT:
            if ((a.s.r.x != b.s.r.x)||(a.s.r.y != b.s.r.y)||(a.s.r.w != b.s.r.w)||(a.s.r.h != b.s.r.h))
                return 0;
            break;
        case ST_CIRCLE:
            if ((a.s.c.x != b.s.c.x)||(a.s.c.y != b.s.c.y)||(a.s.c.r != b.s.c.r))
                return 0;
            break;
        case ST_EDGE:
            if ((a.s.e.x1 != b.s.e.x1)||(a.s.e.y1 != b.s.e.y1)||(a.s.e.x2 != b.s.e.x2)||(a.s.e.y2 != b.s.e.y2))
                return 0;
            break;
    }
    return 1;
}

Vector2D gfc_shape_get_normal_for_edge(Shape s, Edge e)
{
    Vector2D out = {0};
    switch(s.type)
    {
        case ST_RECT:
            out = gfc_edge_get_normal_for_rect(e, s.s.r);
            break;
        case ST_CIRCLE:
            out = gfc_edge_get_normal_for_cirlce(e, s.s.c);
            break;
        case ST_EDGE:
            out = gfc_edge_get_normal_for_edge(s.s.e, e);
            break;
    }
    return out;
}

Vector2D gfc_shape_get_normal_for_cirlce(Shape s, Circle c)
{
    Vector2D out = {0};
    switch(s.type)
    {
        case ST_RECT:
            out = gfc_rect_get_normal_for_cirlce(s.s.r, c);
            break;
        case ST_CIRCLE:
            out = gfc_circle_get_normal_for_cirlce(s.s.c, c);
            break;
        case ST_EDGE:
            out = gfc_edge_get_normal_for_cirlce(s.s.e, c);
            break;
    }
    return out;
}

Vector2D gfc_shape_get_normal_for_rect(Shape s, Rect r)
{
    Vector2D out = {0};
    switch(s.type)
    {
        case ST_RECT:
            out = gfc_rect_get_normal_for_rect(s.s.r, r);
            break;
        case ST_CIRCLE:
            out = gfc_circle_get_normal_for_rect(s.s.c, r);
            break;
        case ST_EDGE:
            out = gfc_edge_get_normal_for_rect(s.s.e, r);
            break;
    }
    return out;
}

Vector2D gfc_shape_get_normal_for_shape(Shape s, Shape s2)
{
    Vector2D out = {0};
    switch(s2.type)
    {
        case ST_RECT:
            out = gfc_shape_get_normal_for_rect(s, s2.s.r);
            break;
        case ST_CIRCLE:
            out = gfc_shape_get_normal_for_cirlce(s, s2.s.c);
            break;
        case ST_EDGE:
            out = gfc_shape_get_normal_for_edge(s, s2.s.e);
            break;
    }
    return out;
}

Vector4D gfc_rect_to_vector4d(Rect r)
{
    return vector4d(r.x,r.y,r.w,r.h);
}

Rect gfc_rect(float x, float y, float w, float h)
{
    Rect r;
    gfc_rect_set(r,x,y,w,h);
    return r;
}

Circle gfc_circle(float x, float y, float r)
{
    Circle c;
    gfc_circle_set(c,x,y,r);
    return c;
}

SDL_Rect gfc_rect_to_sdl_rect(Rect r)
{
    SDL_Rect r2;
    r2.x = r.x;
    r2.y = r.y;
    r2.w = r.w;
    r2.h = r.h;
    return r2;
}

Rect gfc_rect_from_sdl_rect(SDL_Rect r)
{
    Rect r2;
    r2.x = r.x;
    r2.y = r.y;
    r2.w = r.w;
    r2.h = r.h;
    return r2;
}

Uint8 gfc_point_in_rect(Vector2D p,Rect r)
{
    if ((p.x >= r.x) && (p.x <= r.x + r.w)&&
        (p.y >= r.y) && (p.y <= r.y + r.h))
        return 1;
    return 0;
}


Uint8 gfc_rect_overlap_poc(Rect a,Rect b,Vector2D *poc, Vector2D *normal)
{
    if ((a.x > b.x+b.w)||
        (b.x > a.x+a.w)||
        (a.y > b.y+b.h)||
        (b.y > a.y+a.h))
    {
        return 0;
    }
    if (poc)
    {
        poc->y = poc->x = 0;
        if (normal)normal->x = normal->y = 0;
        if (a.x + 1 >= b.x + b.w)
        {
            poc->x = a.x;
            if (normal)normal->x = -1;
        }
        else if (b.x + 1 >= a.x + a.w)
        {
            poc->x = b.x;
            if (normal)normal->x = 1;
        }
        if (a.y + 1 >= b.y + b.h)
        {
            poc->y = a.y;
            if (normal)normal->y = -1;
        }
        if (b.y + 1 >= a.y + a.h)
        {
            if (normal)normal->y = 1;
            poc->y = b.y;
        }
    }
    return 1;
}

Uint8 gfc_rect_overlap(Rect a,Rect b)
{
    return gfc_rect_overlap_poc(a,b,NULL,NULL);
}

Uint8 gfc_point_in_shape(Vector2D p,Shape s)
{
    switch(s.type)
    {
        case ST_RECT:
            return gfc_point_in_rect(p,s.s.r);
        case ST_CIRCLE:
            return gfc_point_in_cicle(p,s.s.c);
        default:
            return 0;
    }
    return 0;
}

Uint8 gfc_point_in_cicle(Vector2D p,Circle c)
{
    if (vector2d_magnitude_compare(vector2d(c.x-p.x,c.y-p.y),c.r) <= 0)return 1;
    return 0;
}

// return number of points of intersection (0, 1, 2, or -1 if the circles are the same)

int gfc_circle_intersect_circle(Circle A, Circle B, Vector2D *pocA, Vector2D *pocB)
{
    Vector2D v,p,outA,outB;
    float dS,d,a,h;
    vector2d_sub(v,A,B);//difference in positions
    if ((v.x == 0)&&(v.y == 0))
    {
        //circles have the same position
        if (A.r == B.r)return -1;//same circle
        return 0;// concentric
    }
    dS = vector2d_magnitude_compare(v,A.r+B.r);
    if (dS > 0)return 0;// circles too far away
    d = sqrt(dS);//distance between circles
    if (d < fabs(A.r - B.r))return 0; // circle wholy inside the other
    v.x /= d;
    v.y /= d;
    a = ((A.r*A.r)-(B.r *B.r) + dS)/(d * 2);
    p.x = A.x + a * v.x;
    p.y = A.y + a * v.y;
    h = sqrt(A.r * A.r - a * a);
    
    outA.x = p.x + (h * v.y);
    outA.y = p.y - (h * v.x);
    outB.x = p.x - (h * v.y);
    outB.y = p.y + (h * v.x);
    
    if (pocA)vector2d_copy((*pocA),outA);
    if (pocB)vector2d_copy((*pocB),outB);
    if ((outA.x == outB.x)&&(outA.y == outB.y))return 1;
    return 2;
}

Uint8 gfc_circle_overlap_poc(Circle a, Circle b,Vector2D *poc,Vector2D *normal)
{
    Vector2D v;
    vector2d_set(v,a.x - b.x,a.y - b.y);
    if (vector2d_magnitude_compare(v,a.r+b.r) <= 0)
    {
        if (poc)
        {
            vector2d_normalize(&v);
            if (normal)
            {
                normal->x = v.x;
                normal->y = v.y;
            }
            vector2d_scale(v,v,a.r);
            poc->x = a.x + v.x;
            poc->y = a.y + v.y;
            
        }
        return 1;
    }
    return 0;
}


Uint8 gfc_circle_overlap(Circle a, Circle b)
{
    return gfc_circle_overlap_poc(a, b,NULL,NULL);
}

Uint8 gfc_circle_rect_overlap_poc(Circle a, Rect b,Vector2D *poc,Vector2D * normal)
{
    Rect newrect1,newrect2;
    gfc_rect_set(newrect1,b.x - a.r,b.y,b.w + a.r+ a.r,b.h);
    gfc_rect_set(newrect2,b.x,b.y - a.r,b.w,b.h + a.r + a.r);
    if (gfc_point_in_cicle(vector2d(b.x,b.y),a))
    {
        if (poc)
        {
            poc->x = b.x;
            poc->y = b.y;
        }
        if (normal)
        {
            normal->x = a.x-b.x;
            normal->y = a.y-b.y;
            vector2d_normalize(normal);
        }
        return 1;
    }
    if (gfc_point_in_cicle(vector2d(b.x+b.w,b.y),a))
    {
        if (poc)
        {
            poc->x = b.x+b.w;
            poc->y = b.y;
        }
        if (normal)
        {
            normal->x = a.x-(b.x+b.w);
            normal->y = a.y-b.y;
            vector2d_normalize(normal);
        }
        return 1;
    }
    if (gfc_point_in_cicle(vector2d(b.x,b.y+b.h),a))
    {
        if (poc)
        {
            poc->x = b.x;
            poc->y = b.y+b.h;
        }
        if (normal)
        {
            normal->x = a.x-b.x;
            normal->y = a.y-(b.y + b.h);
            vector2d_normalize(normal);
        }
        return 1;
    }
    if (gfc_point_in_cicle(vector2d(b.x+b.w,b.y+b.h),a))
    {
        if (poc)
        {
            poc->x = b.x+b.w;
            poc->y = b.y+b.h;
        }
        if (normal)
        {
            normal->x = a.x-(b.x+b.w);
            normal->y = a.y-(b.y + b.h);
            vector2d_normalize(normal);
        }
        return 1;
    }

    if ((gfc_point_in_rect(vector2d(a.x,a.y),newrect1))||
        (gfc_point_in_rect(vector2d(a.x,a.y),newrect2)))
    {
        if (poc)
        {
            if (a.x < b.x)
            {
                poc->y = a.y;
                poc->x = b.x;
                if (normal)
                {
                    normal->x = -1;
                    normal->y = 0;
                }
            }
            else if (a.x > b.x + b.w)
            {
                poc->y = a.y;
                poc->x = b.x + b.w;
                if (normal)
                {
                    normal->x = 1;
                    normal->y = 0;
                }
            }
            if (a.y < b.y)
            {
                poc->y = b.y;
                poc->x = a.x;
                if (normal)
                {
                    normal->x = 0;
                    normal->y = -1;
                }
            }
            else if (a.y > b.y + b.y)
            {
                poc->y = b.y + b.h;
                poc->x = a.x;
                if (normal)
                {
                    normal->x = 0;
                    normal->y = 1;
                }
            }
        }
        return 1;
    }
    return 0;
}

Uint8 gfc_circle_rect_overlap(Circle a, Rect b)
{
    Rect newrect;
    gfc_rect_set(newrect,b.x - a.r,b.y,b.w + a.r+ a.r,b.h);
    if (gfc_point_in_rect(vector2d(a.x,a.y),newrect))return 1;
    gfc_rect_set(newrect,b.x,b.y - a.r,b.w,b.h + a.r + a.r);
    if (gfc_point_in_rect(vector2d(a.x,a.y),newrect))return 1;
    
    if (gfc_point_in_cicle(vector2d(b.x,b.y),a))return 1;
    if (gfc_point_in_cicle(vector2d(b.x+b.w,b.y),a))return 1;
    if (gfc_point_in_cicle(vector2d(b.x,b.y+b.h),a))return 1;
    if (gfc_point_in_cicle(vector2d(b.x+b.w,b.y+b.h),a))return 1;
    return 0;
}

Uint8 gfc_shape_overlap_poc(Shape a, Shape b, Vector2D *poc, Vector2D *normal)
{
    switch(a.type)
    {
        case ST_CIRCLE:
            switch(b.type)
            {
                case ST_CIRCLE:
                    return gfc_circle_overlap_poc(a.s.c,b.s.c,poc,normal);
                case ST_RECT:
                    return gfc_circle_rect_overlap_poc(a.s.c,b.s.r,poc,normal);
                case ST_EDGE:
                    return gfc_circle_to_edge_intersection_poc(b.s.e,a.s.c,poc,normal);
            }
        case ST_RECT:
            switch (b.type)
            {
                case ST_RECT:
                    return gfc_rect_overlap_poc(a.s.r,b.s.r,poc,normal);
                case ST_CIRCLE:
                    return gfc_circle_rect_overlap_poc(b.s.c,a.s.r,poc,normal);
                case ST_EDGE:
                    return gfc_edge_rect_intersection_poc(b.s.e, a.s.r,poc,normal);
            }
        case ST_EDGE:
            switch (b.type)
            {
                case ST_EDGE:
                    return gfc_edge_intersect_poc(a.s.e,b.s.e,poc,normal);
                case ST_CIRCLE:
                    return gfc_edge_to_circle_intersection_poc(a.s.e,b.s.c,poc,normal);
                case ST_RECT:
                    return gfc_edge_rect_intersection_poc(a.s.e, b.s.r,poc,normal);
            }
    }
    return 0;
}

Uint8 gfc_shape_overlap(Shape a, Shape b)
{
    return gfc_shape_overlap_poc(a,b,NULL,NULL);
}

Shape gfc_shape_rect(float x, float y, float w, float h)
{
    Shape shape;
    shape.type = ST_RECT;
    shape.s.r.x = x;
    shape.s.r.y = y;
    shape.s.r.w = w;
    shape.s.r.h = h;
    return shape;
}

Shape gfc_shape_from_rect(Rect r)
{
    Shape shape;
    shape.type = ST_RECT;
    shape.s.r.x = r.x;
    shape.s.r.y = r.y;
    shape.s.r.w = r.w;
    shape.s.r.h = r.h;
    return shape;
}

Shape gfc_shape_sdl_rect(SDL_Rect r)
{
    Shape shape;
    shape.type = ST_RECT;
    shape.s.r.x = r.x;
    shape.s.r.y = r.y;
    shape.s.r.w = r.w;
    shape.s.r.h = r.h;
    return shape;
}

Shape gfc_shape_circle(float x, float y, float r)
{
    Shape shape;
    shape.type = ST_CIRCLE;
    shape.s.c.x = x; 
    shape.s.c.y = y;
    shape.s.c.r = r;
    return shape;
}

Shape gfc_shape_from_circle(Circle c)
{
    Shape shape;
    shape.type = ST_CIRCLE;
    shape.s.c.x = c.x;
    shape.s.c.y = c.y;
    shape.s.c.r = c.r;
    return shape;
}

Circle gfc_shape_to_circle(Shape s)
{
    Circle c = {0};
    switch(s.type)
    {
        default:
            return c;
        case ST_EDGE:
            return gfc_edge_get_bounding_circle(s.s.e);
        case ST_RECT:
            return gfc_rect_get_bounding_circle(s.s.r);
        case ST_CIRCLE:
            return s.s.c;
        
    }
}

Shape gfc_shape_edge(float x1,float y1,float x2,float y2)
{
    return gfc_shape_from_edge(gfc_edge(x1,y1,x2,y2));
}

Shape gfc_shape_from_edge(Edge e)
{
    Shape shape;
    shape.type = ST_EDGE;
    gfc_edge_copy(shape.s.e,e);
    return shape;
}

void gfc_shape_copy(Shape *dst,Shape src)
{
    if (!dst)return;
    memcpy(dst,&src,sizeof(Shape));
}

void gfc_shape_move(Shape *shape,Vector2D move)
{
    if (!shape)return;
    shape->s.r.x += move.x;
    shape->s.r.y += move.y;
    if (shape->type == ST_EDGE)
    {
        shape->s.e.x2 += move.x;
        shape->s.e.y2 += move.y;
    }
}

Uint8 gfc_edge_intersect_poc(
    Edge a,
    Edge b,
    Vector2D *contact,
    Vector2D *normal
)
{
    float testx, testy;
    float Ua,Ub,Uden;

    Uden = ((b.y2 - b.y1)*(a.x2 - a.x1)) - ((b.x2 - b.x1)*(a.y2 - a.y1));
    if(Uden == 0)
    {
        return 0;/*parallel, can't hit*/
    }

    Ua = (((b.x2 - b.x1)*(a.y1 - b.y1))-((b.y2 - b.y1)*(a.x1 - b.x1))) / Uden;
    Ub = (((a.x2 - a.x1)*(a.y1 - b.y1))-((a.y2 - a.y1)*(a.x1 - b.x1))) / Uden;

    testx = a.x1 + (Ua * (a.x2 - a.x1));
    testy = a.y1 + (Ua * (a.y2 - a.y1));

  
    if((Ua >= 0) && (Ua <= 1) && (Ub >= 0) && ( Ub <= 1))
    {
        if(contact != NULL)
        {
            contact->x = testx;
            contact->y = testy;
        }
        if (normal != NULL)
        {
            normal->x = b.y2 - b.y1;
            normal->y = b.x2 - b.x1;
            vector2d_normalize(normal);
        }
        return 1;
    }
    return 0;  
}

Uint8 gfc_edge_intersect(Edge a,Edge b)
{
    return gfc_edge_intersect_poc(a,b,NULL,NULL);
}

Edge gfc_edge(float x1, float y1, float x2, float y2)
{
    Edge e;
    gfc_edge_set(e,x1,y1,x2,y2);
    return e;
}

Edge gfc_edge_from_vectors(Vector2D a,Vector2D b)
{
    Edge e;
    gfc_edge_set(e,a.x,a.y,b.x,b.y);
    return e;
}

Uint8 gfc_rect_to_intersection_poc(Edge e, Rect r,Vector2D *poc,Vector2D *normal)
{
    Uint8 ret;
    Uint8 i;
    Vector2D contacts[4] = {0};
    Vector2D normals[4] = {0};
    Vector2D *best = NULL;
    Vector2D *bestNormal = NULL;
    float bestDistance;
    if (gfc_edge_intersect_poc(e,gfc_edge(r.x,r.y,r.x+r.w,r.y),&contacts[0],NULL))//top
    {
        ret = 1;
        normals[0].y = -1;
    }
    if (gfc_edge_intersect_poc(e,gfc_edge(r.x,r.y,r.x,r.y+r.h),&contacts[1],NULL))//left
    {
        ret |= 2;
        normals[1].x = -1;
    }
    if (gfc_edge_intersect_poc(e,gfc_edge(r.x,(r.y+r.h),r.x+r.w,(r.y+r.h)),&contacts[2],NULL))//bottom
    {
        ret |= 4;
        normals[2].y = 1;
    }
    if (gfc_edge_intersect_poc(e,gfc_edge(r.x+r.w,r.y,r.x+r.w,r.y+r.h),&contacts[3],NULL))//right
    {
        ret |= 8;
        normals[3].x = 1;
    }
    if (!ret)return 0;
    if ((!poc)&&(!normal))return 1;// no need to do additional work here
    for (i = 0;i < 4;i++)
    {
        if ((1<<i) & ret)
        {
            if (!best)
            {
                best = &contacts[i];
                bestNormal = &normals[i];
                bestDistance = vector2d_magnitude_squared(vector2d(e.x1-contacts[i].x,e.y1-contacts[i].y));
                continue;
            }
            if (vector2d_magnitude_squared(vector2d(e.x1-contacts[i].x,e.y1-contacts[i].y)) < bestDistance)
            {
                best = &contacts[i];
                bestNormal = &normals[i];
                bestDistance = vector2d_magnitude_squared(vector2d(e.x1-contacts[i].x,e.y1-contacts[i].y));
            }
        }
    }
    if (normal)
    {
        normal->x = bestNormal->x;
        normal->y = bestNormal->y;
    }
    if (poc)
    {
        poc->x = best->x;
        poc->y = best->y;
    }
    return 1;
}

Uint8 gfc_edge_rect_intersection_poc(Edge e, Rect r,Vector2D *poc,Vector2D *normal)
{
    Uint8 ret = 0;
    Uint8 i;
    Vector2D contacts[4] = {0};
    Vector2D normals[4] = {0};
    Vector2D center;
    Vector2D *best = NULL;
    Vector2D *bestNormal = NULL;
    float bestDistance;
    if (gfc_edge_intersect_poc(e,gfc_edge(r.x,r.y,r.x+r.w,r.y),&contacts[0],NULL))//top
    {
        ret = 1;
        normals[0].y = -1;
    }
    if (gfc_edge_intersect_poc(e,gfc_edge(r.x,r.y,r.x,r.y+r.h),&contacts[1],NULL))//left
    {
        ret |= 2;
        normals[1].x = -1;
    }
    if (gfc_edge_intersect_poc(e,gfc_edge(r.x,(r.y+r.h),r.x+r.w,(r.y+r.h)),&contacts[2],NULL))//bottom
    {
        ret |= 4;
        normals[2].y = 1;
    }
    if (gfc_edge_intersect_poc(e,gfc_edge(r.x+r.w,r.y,r.x+r.w,r.y+r.h),&contacts[3],NULL))//right
    {
        ret |= 8;
        normals[3].x = 1;
    }
    if (!ret)return 0;
    if ((!poc)&&(!normal))return 1;// no need to do additional work here
    for (i = 0;i < 4;i++)
    {
        if ((1<<i) & ret)
        {
            if (!best)
            {
                best = &contacts[i];
                bestNormal = &normals[i];
                bestDistance = vector2d_magnitude_squared(vector2d(e.x1-contacts[i].x,e.y1-contacts[i].y));
                continue;
            }
            if (vector2d_magnitude_squared(vector2d(e.x1-contacts[i].x,e.y1-contacts[i].y)) < bestDistance)
            {
                best = &contacts[i];
                bestNormal = &normals[i];
                bestDistance = vector2d_magnitude_squared(vector2d(e.x1-contacts[i].x,e.y1-contacts[i].y));
            }
        }
    }
    if (normal)
    {
        if ((vector2d_distance_between_less_than(*best,vector2d(r.x,r.y),0.1))||// top left corner
            (vector2d_distance_between_less_than(*best,vector2d(r.x+r.w,r.y),0.1))||
            (vector2d_distance_between_less_than(*best,vector2d(r.x,r.y+r.h),0.1))||
            (vector2d_distance_between_less_than(*best,vector2d(r.x+r.w,r.y+r.h),0.1)))
        {
            center = gfc_rect_get_center_point(r);
            *normal = gfc_edge_get_normal_for_cirlce(e, gfc_circle(center.x,center.y,r.w+r.h));
        }
        else
        {
            normal->x = bestNormal->x;
            normal->y = bestNormal->y;
        }
    }
    if (poc)
    {
        poc->x = best->x;
        poc->y = best->y;
    }
    return 1;
}

Uint8 gfc_edge_rect_intersection(Edge e, Rect r)
{
    return gfc_edge_rect_intersection_poc(e,r,NULL,NULL);
}

Uint8 gfc_edge_intersect_shape(Edge e,Shape s)
{
    return gfc_shape_overlap(gfc_shape_from_edge(e), s);
}

Uint8 gfc_edge_intersect_shape_poc(Edge e,Shape s,Vector2D *poc,Vector2D *normal)
{
    return gfc_shape_overlap_poc(gfc_shape_from_edge(e), s,poc,normal);
}

Uint8 gfc_circle_to_edge_intersection_poc(Edge e,Circle c,Vector2D *poc,Vector2D *normal)
{
    float dx, dy, A, B, C, det, t,t1,t2;
    Vector2D intersection1, intersection2;
    dx = e.x2 - e.x1;
    dy = e.y2 - e.y1;

    A = dx * dx + dy * dy;
    B = 2 * (dx * (e.x1 - c.x) + dy * (e.y1 - c.y));
    C = (e.x1 - c.x) * (e.x1 - c.x) +
        (e.y1 - c.y) * (e.y1 - c.y) -
        c.r * c.r;

    det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0))
    {
        // No real solutions.
        return 0;
    }
    if (det == 0)
    {
        // One solution.
        t = -B / (2 * A);
        intersection1 = vector2d(e.x1 + t * dx, e.y1 + t * dy);
        if ((intersection1.x < MIN(e.x1,e.x2))||(intersection1.x > MAX(e.x1,e.x2))||
            (intersection1.y < MIN(e.y1,e.y2))||(intersection1.y > MAX(e.y1,e.y2)))
        {
            //point lies outside of line segment
            return 0;
        }
        if (poc)
        {
            *poc = intersection1;
        }
        if (normal)
        {
            *normal = gfc_edge_get_normal_for_cirlce(e, c);
        }
        return 1;
    }
    else
    {
        // Two solutions. picking the one closer to the first point of the edge
        t1 = (float)((-B + sqrt(det)) / (2 * A));
        t2 = (float)((-B - sqrt(det)) / (2 * A));
        intersection1 = vector2d(e.x1 + t1 * dx, e.y1 + t1 * dy);
        intersection2 = vector2d(e.x1 + t2 * dx, e.y1 + t2 * dy);
        if ((intersection1.x < MIN(e.x1,e.x2))||(intersection1.x > MAX(e.x1,e.x2))||
            (intersection1.y < MIN(e.y1,e.y2))||(intersection1.y > MAX(e.y1,e.y2)))
        {
            if ((intersection2.x < MIN(e.x1,e.x2))||(intersection2.x > MAX(e.x1,e.x2))||
                (intersection2.y < MIN(e.y1,e.y2))||(intersection2.y > MAX(e.y1,e.y2)))
            {
                return 0;
            }
            t = t2;
        }
        else
        {
            if ((intersection2.x < MIN(e.x1,e.x2))||(intersection2.x > MAX(e.x1,e.x2))||
                (intersection2.y < MIN(e.y1,e.y2))||(intersection2.y > MAX(e.y1,e.y2)))
            {
                t = t1;
            }
            else
            {
                t = MIN(t1,t2);
            }
        }
        intersection1 = vector2d(e.x1 + t * dx, e.y1 + t * dy);        
        if (poc)
        {
            *poc = intersection1;
        }
        if (normal)
        {
            *normal = gfc_edge_get_normal_for_cirlce(e, c);
        }
        return 2;
    }
}

Uint8 gfc_edge_to_circle_intersection_poc(Edge e,Circle c,Vector2D *poc,Vector2D *normal)
{
    float dx, dy, A, B, C, det, t,t1,t2;
    Vector2D intersection1, intersection2;
    Vector2D cp;
    dx = e.x2 - e.x1;
    dy = e.y2 - e.y1;

    cp.x = c.x;
    cp.y = c.y;
    A = dx * dx + dy * dy;
    B = 2 * (dx * (e.x1 - c.x) + dy * (e.y1 - c.y));
    C = (e.x1 - c.x) * (e.x1 - c.x) +
        (e.y1 - c.y) * (e.y1 - c.y) -
        c.r * c.r;

    det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0))
    {
        // No real solutions.
        return 0;
    }
    else if (det == 0)
    {
        // One solution.
        t = -B / (2 * A);
        intersection1 = vector2d(e.x1 + t * dx, e.y1 + t * dy);
        if ((intersection1.x < MIN(e.x1,e.x2))||(intersection1.x > MAX(e.x1,e.x2))||
            (intersection1.y < MIN(e.y1,e.y2))||(intersection1.y > MAX(e.y1,e.y2)))
        {
            //point lies outside of line segment
            return 0;
        }
        if (poc)
        {
            *poc = intersection1;
        }
        if (normal)
        {
            vector2d_sub(intersection2,cp,intersection1);
            vector2d_normalize(&intersection2);
            *normal = intersection2;
        }
        return 1;
    }
    else
    {
        // Two solutions. picking the one closer to the first point of the edge
        t1 = (float)((-B + sqrt(det)) / (2 * A));
        t2 = (float)((-B - sqrt(det)) / (2 * A));
        intersection1 = vector2d(e.x1 + t1 * dx, e.y1 + t1 * dy);
        intersection2 = vector2d(e.x1 + t2 * dx, e.y1 + t2 * dy);
        if ((intersection1.x < MIN(e.x1,e.x2))||(intersection1.x > MAX(e.x1,e.x2))||
            (intersection1.y < MIN(e.y1,e.y2))||(intersection1.y > MAX(e.y1,e.y2)))
        {
            if ((intersection2.x < MIN(e.x1,e.x2))||(intersection2.x > MAX(e.x1,e.x2))||
                (intersection2.y < MIN(e.y1,e.y2))||(intersection2.y > MAX(e.y1,e.y2)))
            {
                return 0;
            }
            t = t2;
        }
        else
        {
            if ((intersection2.x < MIN(e.x1,e.x2))||(intersection2.x > MAX(e.x1,e.x2))||
                (intersection2.y < MIN(e.y1,e.y2))||(intersection2.y > MAX(e.y1,e.y2)))
            {
                t = t1;
            }
            else
            {
                t = MIN(t1,t2);
            }
        }
        intersection1 = vector2d(e.x1 + t * dx, e.y1 + t * dy);        
        if (poc)
        {
            *poc = intersection1;
        }
        if (normal)
        {
            vector2d_sub(intersection2,cp,intersection1);
            vector2d_normalize(&intersection2);
            *normal = intersection2;
        }
        return 2;
    }
}

Uint8 gfc_edge_circle_intersection(Edge e,Circle c)
{
    Vector2D dir = {0};
    Vector2D p1,p2;
    Vector2D n1,n2;
    p1 = vector2d(e.x1,e.y1);
    p2 = vector2d(e.x2,e.y2);
        
    vector2d_sub(n1,p2,p1);
    n1.x = p2.y - p1.y;
    n1.y = p1.x - p2.x;
    
    vector2d_normalize(&n1);
    vector2d_negate(n2,n1);
    
    vector2d_scale(dir,n1,c.r);
    if (gfc_edge_intersect(e,gfc_edge(c.x, c.y, c.x + dir.x, c.y + dir.y)))
    {
        return 1;
    }
    vector2d_scale(dir,n2,c.r);
    if (gfc_edge_intersect(e,gfc_edge(c.x, c.y, c.x + dir.x, c.y + dir.y)))
    {
        return 1;
    }
    if (gfc_point_in_cicle(p1,c))
    {
        return 1;
    }
    if (gfc_point_in_cicle(p2,c))
    {
        return 1;
    }
    return 0;
}

void gfc_edge_slog(Edge e)
{
    slog("Edge: (%f,%f),(%f,%f)",e.x1,e.y1,e.x2,e.y2);
}

void gfc_rect_slog(Rect r)
{
    slog("Rect: (%f,%f,%f,%f)",r.x,r.y,r.w,r.h);
}

void gfc_circle_slog(Circle c)
{
    slog("Circle: (%f,%f) radius (%f)",c.x,c.y,c.r);
}

void gfc_shape_slog(Shape shape)
{
    switch(shape.type)
    {
        case ST_EDGE:
            gfc_edge_slog(shape.s.e);
            break;
        case ST_RECT:
            gfc_rect_slog(shape.s.r);
            break;
        case ST_CIRCLE:
            gfc_circle_slog(shape.s.c);
            break;
    }
}

float gfc_edge_length(Edge e)
{
    float a,b;
    a = e.x1 - e.x2;
    b = e.y1 - e.y2;
    return vector2d_magnitude(vector2d(a,b));
}

Circle gfc_edge_get_bounding_circle(Edge e)
{
    Circle c;
    c.r = gfc_edge_length(e) * 0.5;
    c.x = e.x1 + (e.x2 - e.x1) * 5;
    c.y = e.y1 + (e.y2 - e.y1) * 5;
    return c;
}

Circle gfc_rect_get_bounding_circle(Rect r)
{
    Circle c;
    Edge e;
    float diameter;
    e = gfc_edge(r.x,r.y,r.x + r.w,r.y + r.h);
    diameter = gfc_edge_length(e);
    c.r = diameter * 0.5;
    c.x = r.x + (r.w * 05);
    c.y = r.y + (r.h * 05);
    return c;
}

Rect gfc_edge_get_bounds(Edge e)
{
    Rect r;
    r.x = MIN(e.x1,e.x2);
    r.y = MIN(e.y1,e.y2);
    r.w = fabs(e.x1 - e.x2);
    r.h = fabs(e.y1 - e.y2);
    return r;
}

Rect gfc_circle_get_bounds(Circle c)
{
    Rect r;
    r.x = c.x-c.r;
    r.y = c.y-c.r;
    r.w = c.r*2;
    r.h = c.r*2;
    return r;
}

Rect gfc_rect_from_vector4(Vector4D v)
{
    Rect r;
    r.x = v.x;
    r.y = v.y;
    r.w = v.z;
    r.h = v.w;
    return r;
}

Rect gfc_shape_get_bounds(Shape shape)
{
    Rect r = {0,0,0,0};
    switch(shape.type)
    {
        case ST_EDGE:
            r = gfc_edge_get_bounds(shape.s.e);
            break;
        case ST_RECT:
            return shape.s.r;
            break;
        case ST_CIRCLE:
            r = gfc_circle_get_bounds(shape.s.c);
            break;
    }
    return r;
}

int gfc_rect_from_json(SJson *json,Rect *rect)
{
    Vector4D dimensions = {0};
    if ((!rect)||(!json))return 0;
    
    if (sj_value_as_vector4d(json,&dimensions))
    {
        *rect = gfc_rect_from_vector4(dimensions);
        return 1;
    }

    return 0;
}

int gfc_shape_from_json(SJson *json,Shape *shape)
{
    const char *type;
    Vector4D dimensions;
    Vector2D point,point2;
    float radius;
    if ((!json)||(!shape))return 0;
    type = sj_get_string_value(sj_object_get_value(json,"type"));
    if (!type)
    {
        slog("gfc_shape_from_json: json missing type specifier, expect [edge,rect,circle]");
        return 0;
    }
    if (strcmp(type,"circle")== 0)
    {
        sj_value_as_vector2d(sj_object_get_value(json,"center"),&point);
        sj_get_float_value(sj_object_get_value(json,"radius"),&radius);
        *shape = gfc_shape_circle(point.x, point.y, radius);
        return 1;
    }
    if (strcmp(type,"rect")== 0)
    {
        sj_value_as_vector4d(sj_object_get_value(json,"dimensions"),&dimensions);
        *shape = gfc_shape_rect(dimensions.x, dimensions.y, dimensions.z, dimensions.w);
        return 1;
    }
    if (strcmp(type,"edge")== 0)
    {
        sj_value_as_vector2d(sj_object_get_value(json,"point1"),&point);
        sj_value_as_vector2d(sj_object_get_value(json,"point2"),&point2);
        *shape = gfc_shape_edge(point.x,point.y,point2.x,point2.y);
        return 1;
    }
    return 0;
}

Vector2D gfc_shape_get_bezier_point_2d(Vector2D p0, Vector2D p1, Vector2D p2,float t)
{
    Vector2D point;
    Vector2D qp,qp2,qpv; /*approximation line starting point and vector*/
    Vector2D p0v,p1v,temp; /*vectors from point to next point*/
    vector2d_sub(p0v,p1,p0);
    vector2d_sub(p1v,p2,p1);
    /*calculate Q*/
    vector2d_scale(temp,p0v,t);
    vector2d_add(qp,p0,temp);
    
    vector2d_scale(temp,p1v,t);
    vector2d_add(qp2,p1,temp);
    
    vector2d_sub(qpv,qp2,qp);
    
    vector2d_scale(temp,qpv,t);
    vector2d_add(point,qp,temp);
    return point;
}

Vector3D gfc_shape_get_bezier_point_3d(Vector3D p0, Vector3D p1, Vector3D p2,float t)
{
    Vector3D point;
    Vector3D qp,qp2,qpv; /*approximation line starting point and vector*/
    Vector3D p0v,p1v,temp; /*vectors from point to next point*/
    vector3d_sub(p0v,p1,p0);
    vector3d_sub(p1v,p2,p1);
    /*calculate Q*/
    vector3d_scale(temp,p0v,t);
    vector3d_add(qp,p0,temp);
    
    vector3d_scale(temp,p1v,t);
    vector3d_add(qp2,p1,temp);
    
    vector3d_sub(qpv,qp2,qp);
    
    vector3d_scale(temp,qpv,t);
    vector3d_add(point,qp,temp);
    return point;
}

List *gfc_shape_get_bezier_point_list_2d(Vector2D p0, Vector2D p1, Vector2D p2,Uint32 count)
{
    List *points;
    Vector2D *point;
    Vector2D qp,qp2,qpv; /*approximation line starting point and vector*/
    Vector2D p0v,p1v,temp; /*vectors from point to next point*/
    Vector2D dp; /*draw point*/
    float t = 0;  /*time segment*/
    float tstep;
    vector2d_sub(p0v,p1,p0);
    vector2d_sub(p1v,p2,p1);
    tstep = 1/(float)count;
    points = gfc_list_new();
    if (!points)return NULL;
    for (t = 0; t <= 1;t += tstep)
    {
        /*calculate Q*/
        vector2d_scale(temp,p0v,t);
        vector2d_add(qp,p0,temp);
        
        vector2d_scale(temp,p1v,t);
        vector2d_add(qp2,p1,temp);
        
        vector2d_sub(qpv,qp2,qp);
        
        vector2d_scale(temp,qpv,t);
        vector2d_add(dp,qp,temp);
        point = gfc_allocate_array(sizeof(Vector2D),1);
        if (!point)continue;
        vector2d_copy((*point),dp);
        points = gfc_list_append(points,point);
    }
    return points;
}

List *gfc_shape_get_bezier_point_list_3d(Vector3D p0, Vector3D p1, Vector3D p2,Uint32 count)
{
    List *points;
    Vector3D *point;
    Vector3D qp,qp2,qpv; /*approximation line starting point and vector*/
    Vector3D p0v,p1v,temp; /*vectors from point to next point*/
    Vector3D dp; /*draw point*/
    float t = 0;  /*time segment*/
    float tstep;
    vector3d_sub(p0v,p1,p0);
    vector3d_sub(p1v,p2,p1);
    tstep = 1/(float)count;
    points = gfc_list_new();
    if (!points)return NULL;
    for (t = 0; t <= 1;t += tstep)
    {
        /*calculate Q*/
        vector3d_scale(temp,p0v,t);
        vector3d_add(qp,p0,temp);
        
        vector3d_scale(temp,p1v,t);
        vector3d_add(qp2,p1,temp);
        
        vector3d_sub(qpv,qp2,qp);
        
        vector3d_scale(temp,qpv,t);
        vector3d_add(dp,qp,temp);
        point = gfc_allocate_array(sizeof(Vector3D),1);
        if (!point)continue;
        vector3d_copy((*point),dp);
        points = gfc_list_append(points,point);
    }
    return points;
}

void gfc_shape_point_list_free(List *list)
{
    if (!list)return;
    gfc_list_foreach(list,(gfc_work_func*)free);
    gfc_list_delete(list);
}

/*eol@eof*/
