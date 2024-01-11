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

# CMakeList.txt : Extension level CMake template project, define extension
# specific logic here.
#
add_library(extension 
    SHARED
    "extension.cpp" "${SM}/public/smsdk_ext.cpp"
)

string(REPLACE " " "" ext_name ${CMAKE_PROJECT_NAME})
set_target_properties(extension PROPERTIES OUTPUT_NAME "${ext_name}.ext" ) 
get_target_property(ext_name extension OUTPUT_NAME)

target_compile_options(extension 
    PRIVATE $<$<COMPILE_LANGUAGE:CXX>: 
                -std=c++14 -fpic -m32 -fexceptions -fno-threadsafe-statics 
                -Wno-overloaded-virtual -fvisibility-inlines-hidden
                -Wno-non-virtual-dtor -Wno-delete-non-virtual-dtor
                $<$<CONFIG:Debug>: 
                    -Wall -Wextra -pedantic -g -Og -ggdb >
                $<$<CONFIG:Release>: 
                    -s -O3 > >
)

target_compile_definitions(extension
    PRIVATE $<$<COMPILE_LANGUAGE:CXX>: 
                GNUC -D_LINUX -DPOSIX -Dstricmp=strcasecmp 
                -D_stricmp=strcasecmp -D_snprintf=snprintf
                -D_vsnprintf=vsnprintf -DHAVE_STDINT_H
                $<$<CONFIG:Debug>: 
                    -DDEBUG >
                $<$<CONFIG:Release>: 
                    -DNDEBUG > >
)

target_link_libraries(extension 
    PRIVATE $<$<CONFIG:Debug>: -static-libasan -fsanitize=address >
            -lm -m32 "-Wl,--exclude-libs,ALL" -shared 
)

target_include_directories(extension 
    PUBLIC  "${SM}/public" "${SM}/public/extensions" "${SM}/sourcepawn/include" 
            "${SM}/public/amtl/amtl" "${SM}/public/amtl" "." ".."
)

### To install the extension, click "Build > Install {projectname}"
if(IS_DIRECTORY ${install_path}/extensions)
    install(TARGETS 
                extension
            LIBRARY
                DESTINATION ${install_path}/extensions
                PERMISSIONS ${install_permissions}
    )
    MESSAGE(STATUS "Using install dir ${install_path}/extensions")
else()
    MESSAGE(SEND_ERROR "Can't install extension in ${install_path}/extensions")
endif()