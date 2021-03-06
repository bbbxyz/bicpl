#ifndef  DEF_STATISTICS
#define  DEF_STATISTICS

/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : statistics.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Structures for computing basic statistics.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */


#include  <volume_io.h>

typedef  struct
{
    int    n_samples;
    VIO_Real   *samples;
    VIO_Real   min_value;
    VIO_Real   max_value;
    VIO_Real   min_median_range;
    VIO_Real   max_median_range;
    int    n_below_median_range;
    int    n_above_median_range;
    int    n_median_boxes;
    int    *median_box_counts;
    VIO_Real   *median_box_values;
    VIO_Real   sum_x;
    VIO_Real   sum_xx;
} statistics_struct;

#endif
