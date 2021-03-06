#include  <bicpl.h>

  VIO_Status  process_object(
    object_struct  *object );

static  void  copy_colours(
    object_struct  *src_object,
    object_struct  *dest_object )
{
    int          i, n_points, n_src_points, n_dest_points;
    VIO_Point        *points;
    Colour_flags *src_colour_flag, *dest_colour_flag;
    VIO_Colour       *src_colours, *dest_colours;

    src_colour_flag = get_object_colours( src_object, &src_colours );
    n_src_points = get_object_points( src_object, &points );
    dest_colour_flag = get_object_colours( dest_object, &dest_colours );
    n_dest_points = get_object_points( dest_object, &points );

    if( n_src_points == n_dest_points &&
        (*src_colour_flag == PER_VERTEX_COLOURS ||
         *src_colour_flag == PER_ITEM_COLOURS) )
    {
        if( *src_colour_flag == PER_VERTEX_COLOURS )
            n_points = n_src_points;
        else if( src_object->object_type == POLYGONS )
            n_points = get_polygons_ptr(src_object)->n_items;
        else
            return;

        REALLOC( dest_colours, n_points );

        for_less( i, 0, n_points )
            dest_colours[i] = src_colours[i];

        *dest_colour_flag = *src_colour_flag;
        set_object_colours( dest_object, dest_colours );
    }
    else
        print( "Colours not copied.\n" );
}

int  main(
    int    argc,
    char   *argv[] )
{
    VIO_Status         status;
    VIO_STR         src_filename, dest_filename;
    int            i, n_src_objects, n_dest_objects;
    VIO_File_formats   format;
    object_struct  **src_object_list, **dest_object_list;

    status = VIO_OK;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( NULL, &src_filename ) ||
        !get_string_argument( NULL, &dest_filename ) )
    {
        (void) fprintf( stderr, "Must have two filename arguments.\n" );
        return( 1 );
    }

    status = input_graphics_file( src_filename, &format, &n_src_objects,
                                  &src_object_list );

    if( status == VIO_OK )
        status = input_graphics_file( dest_filename, &format, &n_dest_objects,
                                      &dest_object_list );

    print( "%d Objects input.\n", n_src_objects );

    if( status == VIO_OK && n_src_objects != n_dest_objects )
    {
        print( "Different number of objects in the two files.\n" );
        status = VIO_ERROR;
    }

    if( status == VIO_OK )
    {
        for_less( i, 0, n_src_objects )
        {
            if( status == VIO_OK )
                copy_colours( src_object_list[i], dest_object_list[i] );
        }

        if( status == VIO_OK )
            print( "Objects processed.\n" );
    }

    if( status == VIO_OK )
        status = output_graphics_file( dest_filename, format,
                                       n_dest_objects, dest_object_list );

    if( status == VIO_OK )
    {
        delete_object_list( n_src_objects, src_object_list );

        delete_object_list( n_dest_objects, dest_object_list );
    }

    if( status == VIO_OK )
        print( "Objects output.\n" );

    return( status != VIO_OK );
}
