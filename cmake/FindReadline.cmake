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


find_path(READLINE_INCLUDE_DIR readline/readline.h)
find_library(READLINE_LIBRARY NAMES readline)

if(READLINE_INCLUDE_DIR AND READLINE_LIBRARY)
    # Readline can be confugured with '--with-curses' option in that case it is
    # required to link also with termcap or curses library.

    # TODO: Find a better way of determining whether the addional lib (termcap
    #       or curses) is required
    find_library(READLINE_TERMCAP_LIBRARY NAMES termcap)
    mark_as_advanced(READLINE_TERMCAP_LIBRARY)
endif()

# Handle the QUIETLY and REQUIRED arguments and set READLINE_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(Readline
    "This project requires Readline installed. Please install Readline package"
    READLINE_LIBRARY
    READLINE_INCLUDE_DIR
)

mark_as_advanced(
    READLINE_INCLUDE_DIR
    READLINE_LIBRARY
)

set(READLINE_INCLUDE_DIRS ${READLINE_INCLUDE_DIR})
set(READLINE_LIBRARIES ${READLINE_LIBRARY})

if(READLINE_TERMCAP_LIBRARY)
    list(APPEND READLINE_LIBRARIES ${READLINE_TERMCAP_LIBRARY})
endif()
