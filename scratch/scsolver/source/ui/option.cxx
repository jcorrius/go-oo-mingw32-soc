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

#include "option.hxx"

namespace scsolver {

/**
 *  This is where the values of options settings are stored. The
 *  default values are specified in the constructor.
 */
struct OptionDataImpl
{
	OptionDataImpl() :
		VarPositive(true)
	{
	}

	~OptionDataImpl() throw() {}

	bool VarPositive:1;
};

//-----------------------------------------------------------------

OptionData::OptionData() : m_pImpl( new OptionDataImpl )
{
}

OptionData::~OptionData() throw()
{
}

void OptionData::setVarPositive( bool b )
{
	m_pImpl->VarPositive = b;
}

bool OptionData::getVarPositive() const
{
	return m_pImpl->VarPositive;
}

}
