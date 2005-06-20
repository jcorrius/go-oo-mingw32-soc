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
cairoHelperGetGlitzDrawable( const SystemEnvData* pSysData, int width, int height )
{
    Display* display = (Display*) pSysData->pDisplay;
    Window window = pSysData->aWindow;
    VisualID vid = XVisualIDFromVisual( (Visual*) pSysData->pVisual );
    int screen = DefaultScreen( display );

    glitz_drawable_t *pDrawable = NULL;
    glitz_format_t* pFormat = NULL;
    glitz_drawable_format_t* pDrawableFormat;
    XWindowAttributes attrs;

    XGetWindowAttributes (display, window, &attrs);
    VisualID wVID = XVisualIDFromVisual( attrs.visual );
    printf ("find format for visual id %d (%d)\n", wVID, vid);
    pDrawableFormat = glitz_glx_find_drawable_format_for_visual ( display, screen, wVID );

    if( pDrawableFormat )
	printf ("format found color %d %d %d %d doublebuffer %d\n",
		pDrawableFormat->color.red_size,
		pDrawableFormat->color.green_size,
		pDrawableFormat->color.blue_size,
		pDrawableFormat->color.alpha_size,
		pDrawableFormat->doublebuffer);

    if( pDrawableFormat )
	pDrawable = glitz_glx_create_drawable_for_window( display, screen, pDrawableFormat, window, attrs.width, attrs.height );

    return pDrawable;
}

void*
cairoHelperGetGlitzSurface( const SystemEnvData* pSysData, void *drawable, int x, int y, int width, int height )
{
    Display* display = (Display*) pSysData->pDisplay;
    Window window = pSysData->aWindow;

    glitz_drawable_t *pDrawable = (glitz_drawable_t *) drawable;
    glitz_format_t* pFormat = NULL;
    glitz_surface_t* pGlitzSurface = NULL;

    if( pDrawable )
	pFormat = glitz_find_standard_format (pDrawable, GLITZ_STANDARD_ARGB32 );

    if( pFormat )
	pGlitzSurface = glitz_surface_create( pDrawable, pFormat, width, height, 0, NULL );

    if (pGlitzSurface)
	glitz_surface_attach (pGlitzSurface, pDrawable, GLITZ_DRAWABLE_BUFFER_FRONT_COLOR, x, y );

    if( pGlitzSurface )
	printf ("glitz surface created successfully\n");

    return pGlitzSurface;
}
