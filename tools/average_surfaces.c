#include  <bicpl.h>

#undef SIMPLE_TRANSFORMATION
#define SIMPLE_TRANSFORMATION

#define USE_IDENTITY_TRANSFORMS


static  void  compute_transforms(
    int        n_surfaces,
    int        n_points,
    VIO_Point      **points,
    VIO_Transform  transforms[] );

static  void  create_average_polygons(
    int                   n_surfaces,
    int                   n_groups,
    int                   n_points,
    VIO_Point                 **points,
    VIO_Transform             transforms[],
    FILE                  *rms_file,
    FILE                  *variance_file,
    polygons_struct       *polygons );

#ifdef PRINT_TRANSFORMS
static  void  print_transform(
    VIO_Transform   *trans );
#endif

int  main(
    int    argc,
    char   *argv[] )
{
    VIO_Status           status;
    FILE             *rms_file, *variance_file;
    VIO_STR           filename, output_filename;
    VIO_STR           rms_filename, variance_filename;
    int              i, n_objects, n_surfaces, n_groups;
    VIO_File_formats     format;
    object_struct    *out_object;
    object_struct    **object_list;
    polygons_struct  *polygons, *average_polygons;
    VIO_Point            **points_list;
    VIO_Transform        *transforms;

    status = VIO_OK;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( NULL, &output_filename ) ||
        !get_string_argument( NULL, &rms_filename ) ||
        !get_string_argument( NULL, &variance_filename ) ||
        !get_int_argument( 1, &n_groups ) )
    {
        fprintf(stderr,
          "Usage: %s output.obj  none|rms_file  none|variance_file n_groups\n",
                  argv[0] );
        fprintf(stderr, "         [input1.obj] [input2.obj] ...\n" );
        return( 1 );
    }

    if( equal_strings( rms_filename, "none" ) )
        rms_filename = NULL;
    if( equal_strings( variance_filename, "none" ) )
        variance_filename = NULL;

    n_surfaces = 0;
    points_list = NULL;

    out_object = create_object( POLYGONS );
    average_polygons = get_polygons_ptr(out_object);

    while( get_string_argument( NULL, &filename ) )
    {
        if( input_graphics_file( filename, &format, &n_objects,
                                 &object_list ) != VIO_OK )
        {
            print( "Couldn't read %s.\n", filename );
            return( 1 );
        }

        if( n_objects != 1 || get_object_type(object_list[0]) != POLYGONS )
        {
            print( "Invalid object in file.\n" );
            return( 1 );
        }

        polygons = get_polygons_ptr( object_list[0] );

        if( n_surfaces == 0 )
        {
            copy_polygons( polygons, average_polygons );
        }
        else if( !polygons_are_same_topology( average_polygons, polygons ) )
        {
            print( "Invalid polygons topology in file.\n" );
            return( 1 );
        }

        SET_ARRAY_SIZE( points_list, n_surfaces, n_surfaces+1, 1 );
        ALLOC( points_list[n_surfaces], polygons->n_points );

        for_less( i, 0, polygons->n_points )
            points_list[n_surfaces][i] = polygons->points[i];

        ++n_surfaces;

        print( "%d:  %s\n", n_surfaces, filename );

        delete_object_list( n_objects, object_list );
    }

    ALLOC( transforms, n_surfaces );

    compute_transforms( n_surfaces, average_polygons->n_points,
                        points_list, transforms );

#ifdef  PRINT_TRANSFORMS
    for_less( i, 0, n_surfaces )
        print_transform( &transforms[i] );
#endif

    if( rms_filename != NULL )
    {
        if( open_file( rms_filename, WRITE_FILE, ASCII_FORMAT, &rms_file )!= VIO_OK)
            return( 1 );
    }
    else
        rms_file = NULL;

    if( variance_filename != NULL )
    {
        if( open_file( variance_filename, WRITE_FILE, ASCII_FORMAT,
                       &variance_file )!= VIO_OK)
            return( 1 );
    }
    else
        variance_file = NULL;

    create_average_polygons( n_surfaces, n_groups, average_polygons->n_points,
                             points_list, transforms,
                             rms_file, variance_file,
                             average_polygons );

    if( rms_filename != NULL )
        (void) close_file( rms_file );

    if( variance_filename != NULL )
        (void) close_file( variance_file );

    compute_polygon_normals( average_polygons );

    if( status == VIO_OK )
        status = output_graphics_file( output_filename, format,
                                       1, &out_object );

    if( status == VIO_OK )
        print( "Objects output.\n" );

    return( status != VIO_OK );
}

#ifdef SIMPLE_TRANSFORMATION

#ifndef  USE_IDENTITY_TRANSFORMS
static  void  compute_point_to_point_transform(
    int        n_points,
    VIO_Point      src_points[],
    VIO_Point      dest_points[],
    VIO_Transform  *transform )
{
    int    p, dim;
    VIO_Real   **coords, *target, coefs[4];

    VIO_ALLOC2D( coords, n_points, 3 );
    ALLOC( target, n_points );

    for_less( p, 0, n_points )
    {
        coords[p][VIO_X] = (VIO_Real) Point_x(src_points[p]);
        coords[p][VIO_Y] = (VIO_Real) Point_y(src_points[p]);
        coords[p][VIO_Z] = (VIO_Real) Point_z(src_points[p]);
    }

    make_identity_transform( transform );

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        for_less( p, 0, n_points )
            target[p] = (VIO_Real) Point_coord(dest_points[p],dim);

        least_squares( n_points, VIO_N_DIMENSIONS, coords, target, coefs );

        Transform_elem( *transform, dim, 0 ) = coefs[1];
        Transform_elem( *transform, dim, 1 ) = coefs[2];
        Transform_elem( *transform, dim, 2 ) = coefs[3];
        Transform_elem( *transform, dim, 3 ) = coefs[0];
    }

    FREE( target );
    VIO_FREE2D( coords );
}
#endif

static  void  compute_transforms(
    int        n_surfaces,
    int        n_points,
    VIO_Point      **points,
    VIO_Transform  transforms[] )
{
    int   s;

    make_identity_transform( &transforms[0] );

#ifdef  USE_IDENTITY_TRANSFORMS
    for_less( s, 1, n_surfaces )
        make_identity_transform( &transforms[s] );
#else
    for_less( s, 1, n_surfaces )
    {
        compute_point_to_point_transform( n_points, points[s], points[0],
                                          &transforms[s] );
    }
#endif
}
#else

static  void  compute_transforms(
    int        n_surfaces,
    int        n_points,
    VIO_Point      **points,
    VIO_Transform  transforms[] )
{
    int                    dim, s, i, j;
    linear_least_squares   lsq;
    VIO_Real                   *coefs, n, constant;

    ALLOC( coefs, (n_surfaces-1) * 4 );

    for_less( s, 0, n_surfaces )
        make_identity_transform( &transforms[s] );

    n = (VIO_Real) n_surfaces;

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        initialize_linear_least_squares( &lsq, (n_surfaces-1) * 4 );

        for_less( i, 0, n_points )
        {
            for_less( s, 0, n_surfaces )
            {
                for_less( j, 0, (n_surfaces-1)*4 )
                    coefs[j] = 0.0;
                constant = 0.0;

                if( s == 0 )
                {
                    constant = -Point_coord(points[s][i],dim);
                }
                else
                {
                    coefs[VIO_IJ(s-1,0,4)] = Point_x(points[s][i]);
                    coefs[VIO_IJ(s-1,1,4)] = Point_y(points[s][i]);
                    coefs[VIO_IJ(s-1,2,4)] = Point_z(points[s][i]);
                    coefs[VIO_IJ(s-1,3,4)] = 1.0;
                }

                for_less( j, 0, n_surfaces )
                {
                    if( j == 0 )
                    {
                        constant -= -Point_coord(points[j][i],dim) / n;
                    }
                    else
                    {
                        coefs[VIO_IJ(j-1,0,4)] -= Point_x(points[j][i]) / n;
                        coefs[VIO_IJ(j-1,1,4)] -= Point_y(points[j][i]) / n;
                        coefs[VIO_IJ(j-1,2,4)] -= Point_z(points[j][i]) / n;
                        coefs[VIO_IJ(j-1,3,4)] -= 1.0 / n;
                    }
                }

                add_to_linear_least_squares( &lsq, coefs, constant );
            }
        }

        (void) get_linear_least_squares_solution( &lsq, coefs );

        for_less( s, 1, n_surfaces )
        {
            for_less( j, 0, VIO_N_DIMENSIONS+1 )
                Transform_elem(transforms[s],dim,j) = coefs[VIO_IJ(s-1,j,4)];
        }

        delete_linear_least_squares( &lsq );
    }

    FREE( coefs );
}

#endif

static  VIO_Real  get_rms_points(
    int                   n_surfaces,
    int                   n_groups,
    VIO_Point                 samples[],
    VIO_Point                 *centroid,
    VIO_Real                  inv_variance[3][3] )
{
    int    i, j, s;
    VIO_Real   rms, dx, dy, dz;
    VIO_Real   variance[3][3], inverse[3][3];

    rms = 0.0;

    for_less( i, 0, 3 )
    for_less( j, 0, 3 )
        variance[i][j] = 0.0;

    for_less( s, 0, n_surfaces )
    {
        dx = (VIO_Real) Point_x(samples[s]) - (VIO_Real) Point_x(*centroid);
        dy = (VIO_Real) Point_y(samples[s]) - (VIO_Real) Point_y(*centroid);
        dz = (VIO_Real) Point_z(samples[s]) - (VIO_Real) Point_z(*centroid);

        variance[0][0] += dx * dx;
        variance[0][1] += dx * dy;
        variance[0][2] += dx * dz;
        variance[1][1] += dy * dy;
        variance[1][2] += dy * dz;
        variance[2][2] += dz * dz;

        rms += dx * dx + dy * dy + dz * dz;
    }

    variance[1][0] = variance[0][1];
    variance[2][0] = variance[0][2];
    variance[2][1] = variance[1][2];

    for_less( i, 0, 3 )
    for_less( j, 0, 3 )
        variance[i][j] /= (VIO_Real) (n_surfaces-n_groups);

    if( !invert_square_matrix( 3, (VIO_Real**)variance, (VIO_Real**)inverse ) ) {
        fprintf(stderr, "Error getting inverse of variance\n" );
    }

    for_less( i, 0, 3 )
    for_less( j, 0, 3 )
        inv_variance[i][j] = inverse[i][j];

    rms /= (VIO_Real) n_surfaces;

    rms = sqrt( rms );

    return( rms );
}

static  void  create_average_polygons(
    int                   n_surfaces,
    int                   n_groups,
    int                   n_points,
    VIO_Point                 **points,
    VIO_Transform             transforms[],
    FILE                  *rms_file,
    FILE                  *variance_file,
    polygons_struct       *polygons )
{
    VIO_Real   x, y, z;
    int    i, j, p, s;
    VIO_Real   rms, inv_variance[3][3];
    VIO_Point  *samples;

    ALLOC( samples, n_surfaces );

    for_less( p, 0, n_points )
    {
        for_less( s, 0, n_surfaces )
        {
            transform_point( &transforms[s],
                             (VIO_Real) Point_x(points[s][p]),
                             (VIO_Real) Point_y(points[s][p]),
                             (VIO_Real) Point_z(points[s][p]), &x, &y, &z );

            fill_Point( samples[s], x, y, z );
        }

        get_points_centroid( n_surfaces, samples, &polygons->points[p] );

        if( rms_file != NULL || variance_file != NULL ) {
          rms = get_rms_points( n_surfaces, n_groups,
                                samples, &polygons->points[p], inv_variance );

          if( rms_file != NULL ) {
            (void) output_real( rms_file, rms );
            (void) output_newline( rms_file );
          } 
          if( variance_file != NULL ) {
            for_less( i, 0, 3 )
            for_less( j, 0, 3 )
                (void) output_real( variance_file, inv_variance[i][j] );
            (void) output_newline( variance_file );
          }
        }
    }

    FREE( samples );
}

#ifdef PRINT_TRANSFORMS
static  void  print_transform(
    VIO_Transform   *trans )
{
    int   i, j;

    for_less( i, 0, VIO_N_DIMENSIONS )
    {
        for_less( j, 0, VIO_N_DIMENSIONS+1 )
            print( " %12g", Transform_elem(*trans,i,j) );
        print( "\n" );
    }

    print( "\n" );
}
#endif
