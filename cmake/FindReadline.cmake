# - Find Readline
# Locate Readline (line input library with editing features) includes and library
# Once done this will define
#
#  READLINE_INCLUDE_DIRS - Where to find Readline.
#  READLINE_LIBRARIES    - List of libraries when using Readline.
#  READLINE_FOUND        - TRUE if Readline was found.

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


find_path(READLINE_INCLUDE_DIRS readline/readline.h)
find_library(READLINE_LIBRARIES NAMES readline)

if(READLINE_INCLUDE_DIRS AND READLINE_LIBRARIES)
    find_package(Termcap QUIET) # Termcap is an optional dependency
    if(TERMCAP_FOUND)
        list(APPEND READLINE_LIBRARIES "${TERMCAP_LIBRARIES}")
    endif()
endif()

# Handle the QUIETLY and REQUIRED arguments and set READLINE_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(Readline DEFAULT_MSG READLINE_LIBRARIES READLINE_INCLUDE_DIRS)
