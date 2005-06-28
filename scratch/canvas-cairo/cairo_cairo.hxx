#ifndef _CAIROCANVAS_CAIRO_HXX
#define _CAIROCANVAS_CAIRO_HXX

namespace cairo {
#ifndef CAIRO_H
#include <cairo.h>
#endif

typedef cairo_surface_t Surface;
typedef cairo_t Cairo;
typedef cairo_matrix_t Matrix;
typedef cairo_format_t Format;

}

#endif
