#ifndef _WORK_BOOK_HXX_
#define _WORK_BOOK_HXX_

#include <com/sun/star/uno/XInterface.hpp>

using namespace ::com::sun::star;

class WorkBook : public uno::XInterface
{
	String maName;
	String maPath;
	String maFullName;
	
public:
	WorkBook(const ::rtl::OUString& aFullName);
	virtual ~WorkBook();

	//are these required?
	virtual void acquire() throw (); 
	virtual void release() throw ();
	virtual uno::Any queryInterface( const uno::Type & rType ) throw (uno::RuntimeException);


	// attribute get methods
	virtual ::rtl::OUString SAL_CALL getName() throw (uno::RuntimeException);

	virtual ::rtl::OUString SAL_CALL getPath() throw (uno::RuntimeException);

	virtual ::rtl::OUString SAL_CALL getFullName() throw (uno::RuntimeException);
	
};

#endif /* _WORK_BOOK_HXX_ */

