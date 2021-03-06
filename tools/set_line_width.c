#include  <bicpl.h>

static  void  usage(
    VIO_STR   executable )
{
    VIO_STR  usage_str = "\n\
Usage: %s  input_lines.obj  output_lines.obj [thickness]\n\
\n\
     Copies the input lines to the output lines, with the line thickness set\n\
     accordingly, defaulting to 1.\n\n";

    fprintf(stderr, usage_str, executable );
}

int  main(
    int   argc,
    char  *argv[] )
{
    VIO_STR               src_filename, dest_filename;
    VIO_File_formats         format;
    VIO_Real                 line_thickness;
    int                  i;
    int                  n_objects;
    object_struct        **objects;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &src_filename ) ||
        !get_string_argument( "", &dest_filename ) )
    {
        usage( argv[0] );
        return( 1 );
    }

    (void) get_real_argument( 1.0, &line_thickness );

    if( input_graphics_file( src_filename,
                             &format, &n_objects, &objects ) != VIO_OK )
        return( 1 );

    for_less( i, 0, n_objects )
    {
        if( get_object_type( objects[i] ) == LINES )
            get_lines_ptr(objects[i])->line_thickness = (float) line_thickness;
    }

    if( output_graphics_file( dest_filename, format, n_objects, objects ) != VIO_OK)
        return( 1 );

    delete_object_list( n_objects, objects );

    return( 0 );
}
