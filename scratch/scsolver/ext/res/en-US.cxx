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

#include "resource.hxx"
#include "scsolver.hrc"
#include "unoglobal.hxx"

#include "rtl/ustring.hxx"

using ::rtl::OUString;

namespace scsolver {

OUString getLocaleStr(int resid)
{
    static const OUString strList[] = {
        // SCSOLVER_STR_MAINDLG_TITLE
        ascii("Optimization Solver"),
        // SCSOLVER_STR_DEFINE_MODEL
        ascii("Define model"),
        // SCSOLVER_STR_SET_TARGET_CELL
        ascii("Target cell"),
        // SCSOLVER_STR_GOAL
        ascii("Goal"),
        // SCSOLVER_STR_MAXIMIZE
        ascii("Maximize"),
        // SCSOLVER_STR_MINIMIZE
        ascii("Minimize"),
        // SCSOLVER_STR_DECISIONVAR_CELLS
        ascii("By changing cells"),
        // SCSOLVER_STR_CONSTRAINT_SEP
        ascii("Subject to the constraints"),
        // SCSOLVER_STR_CONSTRAINTDLG_TITLE
        ascii("Constraint"),
        // SCSOLVER_STR_CELL_REFERENCE
        ascii("Cell Reference"),
        // SCSOLVER_STR_CONSTRAINT
        ascii("Constraint"),
        // SCSOLVER_STR_BTN_OK
        ascii("OK"),
        // SCSOLVER_STR_BTN_CANCEL
        ascii("Cancel"),
        // SCSOLVER_STR_MSG_REF_CON_RANGE_MISMATCH
        ascii("The reference and constraint ranges do not match."),
        // SCSOLVER_STR_BTN_ADD
        ascii("Add"),
        // SCSOLVER_STR_BTN_CHANGE
        ascii("Change"),
        // SCSOLVER_STR_BTN_DELETE
        ascii("Delete"),
        // SCSOLVER_STR_BTN_SOLVE
        ascii("Solve"),
        // SCSOLVER_STR_BTN_RESET
        ascii("Reset"),
        // SCSOLVER_STR_BTN_OPTIONS
        ascii("Options"),
        // SCSOLVER_STR_BTN_SAVE_MODEL
        ascii("Save"),
        // SCSOLVER_STR_BTN_LOAD_MODEL
        ascii("Load"),
        // SCSOLVER_STR_BTN_CLOSE
        ascii("Close"),
        // SCSOLVER_STR_MSG_SOLUTION_NOT_FOUND
        ascii("Solution could not be found"),
        // SCSOLVER_STR_MSG_SOLUTION_FOUND
        ascii("Solution found"),
        // SCSOLVER_STR_MSG_CELL_GEOMETRIES_DIFFER
        ascii("Cell geometries differ"),
        // SCSOLVER_STR_MSG_MAX_ITERATION_REACHED
        ascii("Maximum iteration reached"),
        // SCSOLVER_STR_MSG_STD_EXCEPTION_CAUGHT
        ascii("Standard exception caught"),
        // SCSOLVER_STR_MSG_ITERATION_TIMED_OUT
        ascii("Iteration timed out"),
        // SCSOLVER_STR_MSG_GOAL_NOT_SET
        ascii("Goal is not set"),
        // SCSOLVER_STR_OPTIONDLG_TITLE
        ascii("Options"),
        // SCSOLVER_STR_OPTION_ASSUME_LINEAR
        ascii("Assume linear model"),
        // SCSOLVER_STR_OPTION_VAR_POSITIVE
        ascii("Allow only positive values"),
        // SCSOLVER_STR_OPTION_VAR_INTEGER
        ascii("Allow only integer values")
    };

    if (resid - SCSOLVER_RES_START >= sizeof(strList)/sizeof(strList[0]))
        return ascii("-");

    return strList[resid - SCSOLVER_RES_START];
}

}
