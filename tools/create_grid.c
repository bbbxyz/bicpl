#include  <bicpl.h>


static  VIO_BOOL  get_axis_from_name(
    VIO_STR   axis_name,
    int      *slice_axis )
{
    VIO_BOOL  found;

    found = FALSE;

    if( axis_name != NULL )
    {
        if( equal_strings( axis_name, "x" ) ||
            equal_strings( axis_name, "X" ) )
        {
            found = TRUE;
            *slice_axis = VIO_X;
        }
        else if( equal_strings( axis_name, "y" ) ||
                 equal_strings( axis_name, "Y" ) )
        {
            found = TRUE;
            *slice_axis = VIO_Y;
        }
        else if( equal_strings( axis_name, "z" ) ||
                 equal_strings( axis_name, "Z" ) )
        {
            found = TRUE;
            *slice_axis = VIO_Z;
        }
    }

    return( found );
}


int  main(
    int   argc,
    char  *argv[] )
{
    VIO_STR               dest_lines_filename, axis_name;
    VIO_Real                 limits[2][2], slice_pos, line_pos, *positions;
    int                  i, n_grids, a1, a2, a, slice_axis, line_axis, *axes;
    VIO_BOOL              found[2];
    object_struct        *object;
    lines_struct         *lines;
    VIO_Point                point1, point2;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &dest_lines_filename ) ||
        !get_string_argument( "", &axis_name ) ||
        !get_axis_from_name( axis_name, &slice_axis ) ||
        !get_real_argument( 0.0, &slice_pos ) )
    {
        print( "Usage: %s  output.obj  x|y|z slice_position\n", argv[0] );
        print( "       [x|y|z line_position] [x|y|z line_position] ...\n" );
        print(" e.g.: %s  output.obj  z 40   x -10 x 0 x 10 y -20 y 0 y -20\n",
               argv[0]);
        print("           draws a grid at z = 40\n" );
        return( 1 );
    }

    n_grids = 0;

    found[0] = FALSE;
    found[1] = FALSE;

    positions = NULL;
    axes = NULL;

    while( get_string_argument( "", &axis_name ) &&
           get_axis_from_name( axis_name, &line_axis ) &&
           get_real_argument( 0.0, &line_pos ) )
    {
        if( line_axis == slice_axis )
        {
            print( "Cannot specify the same axis for a grid line as for the\n");
            print( "grid plane.\n" );
            return( 1 );
        }

        a = (line_axis-slice_axis+VIO_N_DIMENSIONS) % VIO_N_DIMENSIONS - 1;

        if( !found[a] || line_pos < limits[a][0] )
            limits[a][0] = line_pos;
        if( !found[a] || line_pos > limits[a][1] )
            limits[a][1] = line_pos;

        found[a] = TRUE;

        SET_ARRAY_SIZE( axes, n_grids, n_grids+1, DEFAULT_CHUNK_SIZE );
        SET_ARRAY_SIZE( positions, n_grids, n_grids+1, DEFAULT_CHUNK_SIZE );

        axes[n_grids] = line_axis;
        positions[n_grids] = line_pos;
        ++n_grids;
    }

    /* There's going to be a problem if the user did not specify
       lines in both non-plane directions: limits[] will not be initialized.
    */

    object = create_object( LINES );
    lines = get_lines_ptr(object);
    initialize_lines( lines, WHITE );

    a1 = (slice_axis + 1) % VIO_N_DIMENSIONS;
    a2 = (slice_axis + 2) % VIO_N_DIMENSIONS;

    for_less( i, 0, n_grids )
    {
        Point_coord( point1, slice_axis ) = (VIO_Point_coord_type) slice_pos;
        Point_coord( point1, a1 ) = (VIO_Point_coord_type) limits[0][0];
        Point_coord( point1, a2 ) = (VIO_Point_coord_type) limits[1][0];
        Point_coord( point2, slice_axis ) = (VIO_Point_coord_type) slice_pos;
        Point_coord( point2, a1 ) = (VIO_Point_coord_type) limits[0][1];
        Point_coord( point2, a2 ) = (VIO_Point_coord_type) limits[1][1];

        Point_coord( point1, axes[i] ) = (VIO_Point_coord_type) positions[i];
        Point_coord( point2, axes[i] ) = (VIO_Point_coord_type) positions[i];

        start_new_line( lines );
        add_point_to_line( lines, &point1 );
        add_point_to_line( lines, &point2 );
    }

    if( output_graphics_file( dest_lines_filename, ASCII_FORMAT, 1, &object )
                                                      != VIO_OK )
        return( 1 );

    return( 0 );
}
