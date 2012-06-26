# Locate Readline (line input library with editing features) includes and library
# Once done this will define
#
#  READLINE_INCLUDE_DIR - Where to find Readline.
#  READLINE_LIBRARY     - List of libraries when using Readline.
#  READLINE_FOUND       - TRUE if Readline was found.

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


FIND_PATH(READLINE_INCLUDE_DIR readline/readline.h)
FIND_LIBRARY(READLINE_LIBRARY NAMES readline)

IF (READLINE_INCLUDE_DIR AND READLINE_LIBRARY)
    FIND_PACKAGE(Termcap) # Termcap is an optional dependency
    IF (TERMCAP_FOUND)
        LIST (APPEND READLINE_LIBRARY "${TERMCAP_LIBRARY}")
    ENDIF (TERMCAP_FOUND)
ENDIF (READLINE_INCLUDE_DIR AND READLINE_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set READLINE_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE (${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS (Readline DEFAULT_MSG
                                            READLINE_LIBRARY
                                            READLINE_INCLUDE_DIR)
