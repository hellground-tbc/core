# Locate termcap (terminal input library) includes and library
# Once done this will define
#
#  TERMCAP_INCLUDE_DIR - Where to find termcap headers.
#  TERMCAP_LIBRARY     - List of libraries when using termcap.
#  TERMCAP_FOUND       - TRUE if termcap found.

#=============================================================================
# Copyright (C) 2012 HellGround <http://www.hellground.pl/>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#=============================================================================


FIND_PATH(TERMCAP_INCLUDE_DIR termcap.h)
FIND_LIBRARY(TERMCAP_LIBRARY NAMES termcap)

# handle the QUIETLY and REQUIRED arguments and set TERMCAP_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Termcap DEFAULT_MSG
                                          TERMCAP_LIBRARY
                                          TERMCAP_INCLUDE_DIR)
