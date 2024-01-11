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

# CMakeList.txt : CMake template project, include source and define
# project specific logic here. Requirements: Visual Studio 2022 and WSL2.
#
cmake_minimum_required (VERSION 3.19)

project ("CMake Template Project"  
            VERSION 1.0.0
            DESCRIPTION "CMake template for SourceMod projects"
            LANGUAGES CXX)
        
### Linux user/owner of the server sourcemod directory
set(install_owner "username") 

### /path/to/ server sourcemod directory
cmake_path(SET install_path NORMALIZE "/path/to/addons/sourcemod")

MESSAGE(CHECK_START "Searching for SP compiler")
if(EXISTS ${install_path}/scripting/spcomp)
    set(SP_COMPILER "${install_path}/scripting/spcomp")
    MESSAGE(CHECK_PASS "found at: ${SP_COMPILER}")
elseif(EXISTS ${install_path}/scripting/spcomp64)
    set(SP_COMPILER "${install_path}/scripting/spcomp64")
    MESSAGE(CHECK_PASS "found at: ${SP_COMPILER}")
else()
    set(SP_COMPILER "")
    MESSAGE(CHECK_FAIL "couldn't find SourcePawn compiler in ${install_path}/scripting/. Not compiling plugins")
endif()

### Finding the SourceMod SDK and any game SDKs on the WSL
get_filename_component(build_root ".." ABSOLUTE)

if(IS_DIRECTORY ${build_root}/sourcemod) # Can be a symlink to your Window's drive system but IntelliSense won't figure it out
    set(SM "${build_root}/sourcemod")
    MESSAGE(STATUS "Found SourceMod directory at ${SM}")
else()
    MESSAGE(FATAL_ERROR "Couldn't find SourceMod SDK at ${build_root}/sourcemod/")
endif()

if(IS_DIRECTORY ${build_root}/hl2sdk)
    set (HL2SDK "${build_root}/hl2sdk")
    MESSAGE(STATUS "Found HL2SDK directory at ${HL2SDK}")
else()
    MESSAGE(STATUS "No other SDKs found")
endif()

##################################################################

### Include sub-projects.
add_subdirectory("extensions")
if(EXISTS ${SP_COMPILER})
    add_subdirectory("scripting")
endif()
