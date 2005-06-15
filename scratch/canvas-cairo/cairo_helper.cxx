#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <glitz-glx.h>

#ifndef _SV_SYSDATA_HXX
#include <vcl/sysdata.hxx>
#endif

int cairoHelperGetDefaultScreen( void* display )
{
    return DefaultScreen( ( (Display *) display ) );
}

void*
cairoHelperGetGlitzSurface( const SystemEnvData* pSysData, int x, int y, int width, int height )
{
    Display* display = (Display*) pSysData->pDisplay;
    Window window = pSysData->aWindow;
    int screen = DefaultScreen( display );

    glitz_drawable_t *pDrawable = NULL;
    glitz_format_t* pFormat = NULL;
    glitz_drawable_format_t aTemplate;
    glitz_drawable_format_t* pDrawableFormat;
    glitz_surface_t* pGlitzSurface = NULL;
    XVisualInfo *vinfo = NULL;
    XSetWindowAttributes xswa;
    Window win;
    unsigned long mask = 0;

    aTemplate.samples = 1;
    mask |= GLITZ_FORMAT_SAMPLES_MASK;
    aTemplate.types.window = 1;
    mask |= GLITZ_FORMAT_WINDOW_MASK;
    aTemplate.doublebuffer = 0;
    mask |= GLITZ_FORMAT_DOUBLEBUFFER_MASK;

    pDrawableFormat = glitz_glx_find_drawable_format ( display, screen, mask, &aTemplate, 0);

    if( pDrawableFormat ) {
	vinfo = glitz_glx_get_visual_info_from_format( display, screen, pDrawableFormat );
    }

    if( vinfo ) {
	xswa.colormap = XCreateColormap( display, window, vinfo->visual, AllocNone );
	win = XCreateWindow( display, window,
			     x, y, width, height,
			     0, vinfo->depth, InputOutput,
			     vinfo->visual, CWColormap, &xswa );
	XFree (vinfo);

	XMapWindow( display, win );
	pDrawable = glitz_glx_create_drawable_for_window( display, screen, pDrawableFormat, win, width, height );
    }

    if( pDrawable )
	pFormat = glitz_find_standard_format (pDrawable, GLITZ_STANDARD_ARGB32 );

    if( pFormat )
	pGlitzSurface = glitz_surface_create( pDrawable, pFormat, width, height, 0, NULL );

    return pGlitzSurface;
}
