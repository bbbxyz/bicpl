#ifndef  DEF_HISTOGRAM_H
#define  DEF_HISTOGRAM_H

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


#include  <volume_io.h>

typedef struct
{
    VIO_Real   delta;
    VIO_Real   offset;
    int    min_index;
    int    max_index;
    int    *counts;
} histogram_struct;

#endif
