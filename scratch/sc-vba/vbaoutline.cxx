#include "vbaoutline.hxx"
using namespace ::com::sun::star;
using namespace ::org::openoffice;

void
ScVbaOutline::ShowLevels( const uno::Any& RowLevels, const uno::Any& ColumnLevels ) throw (uno::RuntimeException) 
{
	sal_Int16 nLevel = 0;
	if (mxOutline.is()) 
	{
		if (RowLevels >>= nLevel)
		{
			mxOutline->showLevel(nLevel, table::TableOrientation_ROWS);
		} 
		if (ColumnLevels >>= nLevel)
		{
			mxOutline->showLevel(nLevel,table::TableOrientation_COLUMNS);
		}
	}
}
