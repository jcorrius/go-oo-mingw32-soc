/*
 *   OpenOffice.org extension for syntax highlighting
 *   Copyright (C) 2008  Cédric Bosdonnat cedricbosdo@openoffice.org
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; 
 *   version 2 of the License.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
package org.openoffice.coooder.comp.test;

import org.openoffice.coooder.comp.test.base.UnoTestSuite;

import junit.framework.Test;


public class AllTests  {

    public static Test suite() {
        
        // The tests to run by the suite
        Class[] testClasses = new Class[] {
                DummyTest.class
        };
        
        // Create the test suite
        UnoTestSuite suite = new UnoTestSuite(testClasses);
        
        return suite;
    }
}
