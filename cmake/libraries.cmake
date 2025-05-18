include_guard()

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(_LIBS_DIR ${CMAKE_SOURCE_DIR}/ext)

function(setup_libraries target)
	# Find and link libraries
	FetchContent_Declare(
		SDL3
		GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
		GIT_TAG "release-3.2.14"
		GIT_PROGRESS TRUE

		FIND_PACKAGE_ARGS "3.2.14"
	)
	FetchContent_MakeAvailable(SDL3)

	target_include_directories(
		${target} PRIVATE
			${_LIBS_DIR}
			${_LIBS_DIR}/cglm/include
	)

	target_link_libraries(${target} PRIVATE SDL3::SDL3)
	if(UNIX)
		target_link_libraries(${target} PRIVATE m ${CMAKE_DL_LIBS})
	endif()
endfunction()
