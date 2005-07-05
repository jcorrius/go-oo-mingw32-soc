#ifndef SC_VBA_HELPER_HXX
#define SC_VBA_HELPER_HXX

using namespace ::com::sun::star;
using namespace ::org::openoffice;

#ifdef DEBUG
#  define SC_VBA_FIXME(a) OSL_TRACE( a )
#  define SC_VBA_STUB() SC_VBA_FIXME(( "%s - stubbed\n", __FUNCTION__ ))
#else
#  define SC_VBA_FIXME(a)
#  define SC_VBA_STUB()
#endif

#endif
