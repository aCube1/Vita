# Sets the following variables:
#
# LUAJIT_FOUND
# LUAJIT_INCLUDE_DIR
# LUAJIT_LIBRARY

set(LUAJIT_SEARCH_PATHS
	/usr/local
	/usr
	)

find_path(LUAJIT_INCLUDE_DIR
	NAMES luajit.h
	PATH_SUFFIXES include include/luajit-2.0 include/luajit-2.1
	PATHS ${LUAJIT_SEARCH_PATHS})

find_library(LUAJIT_LIBRARY
	NAMES luajit-5.1
	PATH_SUFFIXES lib
	PATHS ${LUAJIT_SEARCH_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LuaJIT DEFAULT_MSG LUAJIT_LIBRARY LUAJIT_INCLUDE_DIR)

if(LUAJIT_FOUND AND NOT TARGET luajit::luajit)
	add_library(luajit::luajit INTERFACE IMPORTED)
	set_target_properties(
		luajit::luajit
		PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${LUAJIT_INCLUDE_DIR}"
			INTERFACE_LINK_LIBRARIES "${LUAJIT_LIBRARY}"
	)
endif()

mark_as_advanced(LUAJIT_INCLUDE_DIR LUAJIT_LIBRARY)
