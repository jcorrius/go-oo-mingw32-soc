#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.0 $
#
#   last change: $Author: KAMI $ $Date: 2006/02/18 14:50:39 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

PRJ=..$/..

PRJPCH=

PRJNAME=scp2
TARGET=accessories
TARGETTYPE=CUI

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

SCP_PRODUCT_TYPE=osl
PARFILES= \
        module_accessories.par             \
        module_gallery_accessories.par     \
        module_templates_accessories.par   \
        module_samples_accessories.par     \
        module_font_accessories.par        \
        file_accessories.par               \
        file_gallery_accessories.par       \
        file_templates_accessories.par     \
        file_samples_accessories.par       \
        file_font_accessories.par

ULFFILES= \
        module_accessories.ulf             \
        module_gallery_accessories.ulf     \
        module_templates_accessories.ulf   \
        module_samples_accessories.ulf     \
        module_font_accessories.ulf


# --- File ---------------------------------------------------------
.INCLUDE :  target.mk
