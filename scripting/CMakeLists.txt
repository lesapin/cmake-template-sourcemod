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
        OUTPUT ${SP_BASE}.smx
        COMMAND ${SP_COMPILER} --verbose=2 --include=${SP_INCLUDE} ${SP_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${SP_BASE}.sp
    )
    list(APPEND SMX_FILES ${SP_BASE}.smx)
endforeach()

add_custom_target(plugins 
    ALL
    DEPENDS ${SMX_FILES} ${GAMEDATA_FILES} ${TRANSLATION_FILES} ${CONFIG_FILES} ${INCLUDE_FILES}
    COMMENT "Compiling all plugins"
)

### Install plugins and all the accessory files in include, gamedata, translations and configs

if(IS_DIRECTORY ${install_path}/plugins)
    list(TRANSFORM SMX_FILES PREPEND "${CMAKE_CURRENT_BINARY_DIR}/")
    install(
        FILES ${SMX_FILES}
        DESTINATION ${install_path}/plugins
        PERMISSIONS ${install_permissions}
    )
    MESSAGE(STATUS "Using install dir ${install_path}/plugins")
else()
    MESSAGE(SEND_ERROR "Can't install plugins in ${install_path}/plugins")
endif()

if(IS_DIRECTORY ${install_path}/gamedata)
    file(GLOB_RECURSE GAMEDATA_FILES LIST_DIRECTORIES false ../gamedata/*) 
    install(
        FILES ${GAMEDATA_FILES}
        DESTINATION ${install_path}/gamedata
        PERMISSIONS ${install_permissions}
    )
    MESSAGE(STATUS "Using install dir ${install_path}/gamedata")
else()
    MESSAGE(SEND_ERROR "Can't install gamedata in ${install_path}/gamedata")
endif()

if(IS_DIRECTORY ${install_path}/translations)
    file(GLOB_RECURSE TRANSLATION_FILES LIST_DIRECTORIES false ../translations/*) 
    install(
        FILES ${TRANSLATION_FILES}
        DESTINATION ${install_path}/translations
        PERMISSIONS ${install_permissions}
    )
    MESSAGE(STATUS "Using install dir ${install_path}/translations")
else()
    MESSAGE(SEND_ERROR "Can't install translations in ${install_path}/translations")
endif()

if(IS_DIRECTORY ${install_path}/configs)
    file(GLOB_RECURSE CONFIG_FILES LIST_DIRECTORIES false ../configs/*) 
    install(
        FILES ${CONFIG_FILES}
        DESTINATION ${install_path}/configs
        PERMISSIONS ${install_permissions}
    )
    MESSAGE(STATUS "Using install dir ${install_path}/configs")
else()
    MESSAGE(SEND_ERROR "Can't install configs in ${install_path}/configs")
endif()

if(IS_DIRECTORY ${install_path}/scripting/include)
    file(GLOB_RECURSE INCLUDE_FILES LIST_DIRECTORIES false ./include/*) 
    install(
        FILES ${INCLUDE_FILES}
        DESTINATION ${install_path}/scripting/include
        PERMISSIONS ${install_permissions}
    )
    MESSAGE(STATUS "Using install dir ${install_path}/scripting/include")
else()
    MESSAGE(SEND_ERROR "Can't install scripting/include in ${install_path}/scripting/include")
endif()