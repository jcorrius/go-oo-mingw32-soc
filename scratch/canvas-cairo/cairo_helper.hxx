#ifndef _SV_SYSDATA_HXX
#include <vcl/sysdata.hxx>
#endif

int cairoHelperGetDefaultScreen( void* display );
void* cairoHelperGetGlitzDrawable( const SystemEnvData* pSysData, int width, int height );
void* cairoHelperGetGlitzSurface( const SystemEnvData* pSysData, void *drawable, int x, int y, int width, int height );
