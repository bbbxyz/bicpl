#include  <def_mni.h>
#include  <def_module.h>

public  void  create_line_circle(
    Point            *centre,
    int              plane_axis,
    Real             x_size,
    Real             y_size,
    int              n_points,
    lines_struct     *lines )
{
    int      i, a1, a2;
    double   angle;

    initialize_lines( lines, WHITE );

    ALLOC( lines->points, n_points );
    ALLOC( lines->end_indices, 1 );
    ALLOC( lines->indices, n_points+1 );

    a1 = (plane_axis+1) % 3;
    a2 = (plane_axis+2) % 3;

    for_less( i, 0, n_points )
    {
        angle = 2.0 * PI * (Real) i / (Real) n_points;
        Point_coord(lines->points[i],a1) = Point_coord(*centre,a1) +
                                           x_size * cos( angle );
        Point_coord(lines->points[i],a2) = Point_coord(*centre,a2) +
                                           y_size * sin( angle );
        Point_coord(lines->points[i],plane_axis) =
                                           Point_coord(*centre,plane_axis);
    }

    for_less( i, 0, n_points )
        lines->indices[i] = i;

    lines->indices[n_points] = 0;

    lines->end_indices[0] = n_points+1;
}