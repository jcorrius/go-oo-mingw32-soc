/*************************************************************************
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Kohei Yoshida.
 *    1039 Kingsway Dr., Apex, NC 27502, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/


#ifndef _MATRIX_HXX_
#define _MATRIX_HXX_

#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include <memory>

namespace scsolver { namespace numeric {

class MatrixSizeMismatch : public std::exception 
{
public:
	virtual const char* what() const throw();
};

class MatrixNotDiagonal : public std::exception
{
public:
	virtual const char* what() const throw();
};

class OperationOnEmptyMatrix : public std::exception
{
public:
	virtual const char* what() const throw();
};

class SingularMatrix : public std::exception
{
public:
	virtual const char* what() const throw();
};

class NonSquareMatrix : public std::exception
{
public:
	virtual const char* what() const throw();
};

class MatrixImpl;

class Matrix
{
public:
	Matrix();
	Matrix( size_t, size_t, bool = false );
	Matrix( const Matrix& );
	Matrix( const Matrix* );
	Matrix( const MatrixImpl& );
	Matrix( ::boost::numeric::ublas::matrix<double> );
	~Matrix() throw();
	
	void setResizable( bool );
	void swap( Matrix& ) throw();
	void clear();
	void copy( const Matrix& );
	Matrix clone() const;

	::boost::numeric::ublas::matrix< std::string > getDisplayElements( size_t, size_t, bool ) const;	
	void print( size_t = 2, size_t = 1 ) const;
	
	Matrix getColumn( size_t );
	void deleteColumn( size_t );
	void deleteColumns( const std::vector<size_t>& );
	void deleteRow( size_t );
	void deleteRows( const std::vector<size_t>& );
	
	const Matrix adj() const;
	double cofactor( size_t, size_t ) const;
	double det() const;
	const Matrix inverse() const;
	const Matrix trans() const;
	double minors( size_t, size_t ) const;
	void resize( size_t, size_t );

	size_t rows() const;
	size_t cols() const;
	
	bool empty() const;
	bool isRowEmpty( size_t ) const;
	bool isColumnEmpty( size_t ) const;
	bool isSameSize( const Matrix& ) const;
	bool isSquare() const;
	
	// Overloaded Operators
	
	Matrix& operator=( const Matrix& );
	const Matrix operator+( const Matrix& ) const;
	const Matrix operator-( const Matrix& ) const;
	const Matrix operator*( double ) const;
	const Matrix operator*( const Matrix& ) const;
	const Matrix operator/( double ) const;
	Matrix& operator+=( const Matrix& );
	Matrix& operator+=( double );
	Matrix& operator-=( const Matrix& );
	Matrix& operator*=( double );
	Matrix& operator/=( double );
	const double operator()( size_t, size_t ) const;
	double& operator()( size_t, size_t );
	
	bool operator==( const Matrix& ) const;
	bool operator!=( const Matrix& ) const;
	

private:
	std::auto_ptr<MatrixImpl> m_pImpl;

}; // class Matrix

const Matrix operator+( const Matrix&, double );
const Matrix operator+( double, const Matrix& );

}}

#endif //_MATRIX_HXX_
