#ifndef _SV_SYSDATA_HXX
#include <vcl/sysdata.hxx>
#endif

#ifndef _SV_BITMAP_HXX
#include <vcl/bitmap.hxx>
#endif

namespace cairo {
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cairo.h>
#include <cairo-xlib.h>

#ifdef CAIRO_HAS_GLITZ_SURFACE
#include <GL/glx.h>

#include <cairo-glitz.h>

#include <glitz.h>
#include <glitz-glx.h>
#endif
}

using namespace ::cairo;

//#define OOO_CANVAS_GLITZ 1

int cairoHelperGetDefaultScreen( void* display )
{
    return DefaultScreen( ( (Display *) display ) );
}

void*
cairoHelperGetGlitzDrawable( const SystemEnvData* pSysData, int width, int height )
{
#ifdef OOO_CANVAS_GLITZ
    Display* display = (Display*) pSysData->pDisplay;
    ::cairo::Window window = pSysData->aWindow;
    VisualID vid = XVisualIDFromVisual( (Visual*) pSysData->pVisual );
    int screen = DefaultScreen( display );

    glitz_drawable_t *pDrawable = NULL;
    glitz_format_t* pFormat = NULL;
    glitz_drawable_format_t* pDrawableFormat;
    XWindowAttributes attrs;

    XGetWindowAttributes (display, window, &attrs);
    VisualID wVID = XVisualIDFromVisual( attrs.visual );
    OSL_TRACE ("find format for visual id %d (%d)\n", wVID, vid);
    pDrawableFormat = glitz_glx_find_drawable_format_for_visual ( display, screen, wVID );

    if( pDrawableFormat )
	OSL_TRACE ("format found color %d %d %d %d doublebuffer %d\n",
		pDrawableFormat->color.red_size,
		pDrawableFormat->color.green_size,
		pDrawableFormat->color.blue_size,
		pDrawableFormat->color.alpha_size,
		pDrawableFormat->doublebuffer);

    if( pDrawableFormat && pDrawableFormat->doublebuffer == 0 )
	pDrawable = glitz_glx_create_drawable_for_window( display, screen, pDrawableFormat, window, attrs.width, attrs.height );

    return pDrawable;
#endif
    return NULL;
}

void*
cairoHelperGetGlitzSurface( const SystemEnvData* pSysData, void *drawable, int x, int y, int width, int height )
{
#ifdef OOO_CANVAS_GLITZ
    Display* display = (Display*) pSysData->pDisplay;
    ::cairo::Window window = pSysData->aWindow;

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
	OSL_TRACE ("glitz surface created successfully\n");

    return pGlitzSurface;
#endif
    return NULL;
}

void*
cairoHelperGetSurface( const SystemEnvData* pSysData, int x, int y, int width, int height )
{
    cairo_surface_t* pSurface = NULL;

#ifdef OOO_CANVAS_GLITZ
#ifdef CAIRO_HAS_GLITZ_SURFACE
    glitz_drawable_t* pGlitzDrawable;
    glitz_surface_t* pGlitzSurface;

    OSL_TRACE ("try to create glitz surface %d x %d\n", width, height );
    pGlitzDrawable = (glitz_drawable_t*) cairoHelperGetGlitzDrawable( pSysData, width, height );

    if( pGlitzDrawable ) {
	pGlitzSurface = (glitz_surface_t*) cairoHelperGetGlitzSurface( pSysData, pGlitzDrawable, x, y, width, height );
	glitz_drawable_destroy( pGlitzDrawable );
    }

    if( pGlitzSurface ) {
	pSurface = cairo_glitz_surface_create( pGlitzSurface );
	glitz_surface_destroy( pGlitzSurface );
    }

#endif
#endif
    if( !pSurface ) {
	pSurface=cairo_xlib_surface_create( (Display*) pSysData->pDisplay,
					    pSysData->aWindow,
					    (Visual*) pSysData->pVisual,
					    width + x, height + y );
	cairo_surface_set_device_offset( pSurface, x, y );
    }

    return pSurface;
}

void
cairoHelperFlush( const SystemEnvData* pSysData )
{
#ifdef OOO_CANVAS_GLITZ
 #ifdef CAIRO_HAS_GLITZ_SURFACE
     glXWaitGL();
     glXWaitX();
     //glFinish();
 #else
    XSync( (Display*) pSysData->pDisplay, false );
 #endif
#else
    XSync( (Display*) pSysData->pDisplay, false );
#endif
}

void*
cairoHelperGetSurface( const SystemEnvData* pSysData, const BitmapSystemData& rBmpData, int width, int height )
{
	return cairo_xlib_surface_create( (Display*) pSysData->pDisplay,
					  (Drawable) rBmpData.aPixmap,
					  (Visual*) pSysData->pVisual,
					  width, height );
}
