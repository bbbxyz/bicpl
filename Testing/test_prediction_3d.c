#include  <internal_volume_io.h>
#include  <bicpl.h>

int  main(
    int     argc,
    char    *argv[] )
{
    int      i, n_points, dim;
    Real     x, y, z, value;
    Real     xs[10], ys[10], zs[10], weights[10];

    xs[0] = 1.0;
    ys[0] = 2.0;
    zs[0] = 3.0;

    xs[1] = 3.0;
    ys[1] = 2.0;
    zs[1] = -5.0;

    xs[2] = 5.0;
    ys[2] = 8.0;
    zs[2] = 10.0;

    xs[3] = 10.0;
    ys[3] = -8.0;
    zs[3] = 14.0;

    xs[4] = 3.0;
    ys[4] = 8.3;
    zs[4] = 1.4;

    x = 1.0;
    y = 0.1;
    z = 2.2;

    n_points = 5;

    if( !get_prediction_weights_3d( x, y, z, n_points, xs, ys, zs, weights ) )
        print_error( "Dang.\n" );

    for_less( dim, 0, 3 )
    {
        value = 0.0;
        for_less( i, 0, n_points )
        {
            value += (dim == 0 ? weights[i] : 0.0) * xs[i] +
                     (dim == 1 ? weights[i] : 0.0) * ys[i] +
                     (dim == 2 ? weights[i] : 0.0) * zs[i];
        }

        print( "%g %g %g: %g\n", x, y, z, value );
    }

    return( 0 );
}
