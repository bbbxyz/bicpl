#include "bicpl_internal.h"

#define  N_BOX_RATIO   0.05
#define  MIN_N_BOXES   10

typedef  struct
{
    int   n_points;
    int   *point_indices;
} box_struct;

static  void  get_box_index(
    VIO_Point   *point,
    VIO_Point   *min_point,
    VIO_Point   *max_point,
    int     n_boxes[],
    int     *i,
    int     *j,
    int     *k )
{
    VIO_Real  diff, p, min_p, max_p;
    int   dim, *coords[VIO_N_DIMENSIONS];

    coords[VIO_X] = i;
    coords[VIO_Y] = j;
    coords[VIO_Z] = k;

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        p = (VIO_Real) Point_coord(*point,dim);
        min_p = (VIO_Real) Point_coord(*min_point,dim);
        max_p = (VIO_Real) Point_coord(*max_point,dim);
        if( p <= min_p )
            *(coords[dim]) = 0;
        else if( p >= max_p )
            *(coords[dim]) = n_boxes[dim]-1;
        else
        {
            diff = max_p - min_p;
            if( diff <= 0.0 )
                *(coords[dim]) = 0;
            else
                *(coords[dim]) = (int) ((VIO_Real) n_boxes[dim] *
                                        (p - min_p) / diff);
        }
    } 
}

BICAPI  void  coalesce_object_points(
    int      *n_points,
    VIO_Point    *points[],
    int      n_indices,
    int      indices[] )
{
    int         n_boxes[VIO_N_DIMENSIONS];
    int         i, j, k, p, point, ind, *points_list, *translation;
    int         cum_index, dim, new_n_points;
    box_struct  ***boxes;
    VIO_Point       min_point, max_point, *new_points;

    new_points = NULL;

    get_range_points( *n_points, *points, &min_point, &max_point );

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        n_boxes[dim] = (int) pow( (VIO_Real) *n_points * N_BOX_RATIO, 0.3333 );
        if( n_boxes[dim] < MIN_N_BOXES )
            n_boxes[dim] = MIN_N_BOXES;
        if( Point_coord(min_point,dim) == Point_coord(max_point,dim) )
            n_boxes[dim] = 1;
    }

    VIO_ALLOC3D( boxes, n_boxes[VIO_X], n_boxes[VIO_Y], n_boxes[VIO_Z] );

    for_less( i, 0, n_boxes[VIO_X] )
    for_less( j, 0, n_boxes[VIO_Y] )
    for_less( k, 0, n_boxes[VIO_Z] )
    {
        boxes[i][j][k].n_points = 0;
    }

    for_less( point, 0, *n_points )
    {
        get_box_index( &(*points)[point], &min_point, &max_point, n_boxes,
                       &i, &j, &k );
        ++boxes[i][j][k].n_points;
    }

    ALLOC( points_list, *n_points );

    cum_index = 0;
    for_less( i, 0, n_boxes[VIO_X] )
    for_less( j, 0, n_boxes[VIO_Y] )
    for_less( k, 0, n_boxes[VIO_Z] )
    {
        boxes[i][j][k].point_indices = &points_list[cum_index];
        cum_index += boxes[i][j][k].n_points;
        boxes[i][j][k].n_points = 0;
    }

    for_less( point, 0, *n_points )
    {
        get_box_index( &(*points)[point], &min_point, &max_point, n_boxes,
                       &i, &j, &k );
        boxes[i][j][k].point_indices[boxes[i][j][k].n_points] = point;
        ++boxes[i][j][k].n_points;
    }

    ALLOC( translation, *n_points );
    for_less( point, 0, *n_points )
        translation[point] = -1;

    new_n_points = 0;

    for_less( point, 0, *n_points )
    {
        get_box_index( &(*points)[point], &min_point, &max_point, n_boxes,
                       &i, &j, &k );

        for_less( p, 0, boxes[i][j][k].n_points )
        {
            ind = boxes[i][j][k].point_indices[p];
            if( ind < point && EQUAL_POINTS( (*points)[point], (*points)[ind] ))
            {
                break;
            }
        }

        if( p < boxes[i][j][k].n_points )
            translation[point] = translation[ind];
        else
        {
            translation[point] = new_n_points;
            ADD_ELEMENT_TO_ARRAY( new_points, new_n_points, (*points)[point],
                                  DEFAULT_CHUNK_SIZE );
        }
    }

    for_less( i, 0, n_indices )
        indices[i] = translation[indices[i]];

    VIO_FREE3D( boxes );
    FREE( points_list );
    FREE( translation );

    FREE( *points );
    *n_points = new_n_points;
    *points = new_points;
}

BICAPI  void  separate_object_points(
    int           *new_n_points,
    VIO_Point         *points[],
    int           n_indices,
    int           indices[],
    Colour_flags  colour_flag,
    VIO_Colour        *colours[] )
{
    int    point_index, ind;
    VIO_Point  *new_points;
    VIO_Colour *new_colours;

    new_colours = NULL;
    new_points = NULL;
    *new_n_points = 0;
    for_less( ind, 0, n_indices )
    {
        point_index = indices[ind];
        ADD_ELEMENT_TO_ARRAY( new_points, *new_n_points,
                              (*points)[point_index], DEFAULT_CHUNK_SIZE );

        if( colour_flag == PER_VERTEX_COLOURS )
        {
            --(*new_n_points);
            ADD_ELEMENT_TO_ARRAY( new_colours, *new_n_points,
                                  (*colours)[point_index], DEFAULT_CHUNK_SIZE );
        }

        indices[ind] = *new_n_points - 1;
    }

    FREE( *points );
    *points = new_points;

    if( colour_flag == PER_VERTEX_COLOURS )
    {
        FREE( *colours );
        *colours = new_colours;
    }
}
