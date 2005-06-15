#ifndef _SV_SYSDATA_HXX
#include <vcl/sysdata.hxx>
#endif

int cairoHelperGetDefaultScreen( void* display );
void* cairoHelperGetGlitzSurface( const SystemEnvData* pSysData, int x, int y, int width, int height );
