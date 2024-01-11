#
# SourceMod CMake Template
# Copyright (C) 2024 bezdmn
# =============================================================================
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License, version 3.0, as published by the
# Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#

# CMakeList.txt : Scripting level CMake template project, define plugin
# specific logic here.
#
get_filename_component(SP_INCLUDE "." ABSOLUTE)
set(SP_INCLUDE "${SP_INCLUDE}/include")

if(IS_DIRECTORY ${SP_INCLUDE})
    MESSAGE(STATUS "SourcePawn include directory found at ${SP_INCLUDE}")
else()
    MESSAGE(SEND_ERROR "Couldn't find SP include directory")
endif()

file(GLOB SP_FILES LIST_DIRECTORIES false *.sp) # Compile all SP files
foreach(SP_FILE ${SP_FILES})
    get_filename_component(SP_BASE ${SP_FILE} NAME_WE)
    add_custom_command(
        OUTPUT 
            ${SP_BASE}.smx
        COMMAND
            ${SP_COMPILER} --verbose=2 --include=${SP_INCLUDE} ${SP_FILE}
        WORKING_DIRECTORY 
            ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS 
            ${SP_BASE}.sp
    )
    list(APPEND SMX_FILES ${SP_BASE}.smx)
endforeach()

add_custom_target(plugins 
    ALL
    DEPENDS 
        ${SMX_FILES}
    COMMENT 
        "Compiling all plugins"
)
