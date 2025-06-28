include_guard()

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(_LIBS_DIR ${CMAKE_SOURCE_DIR}/ext)

function(setup_libraries target)
	# Find and link libraries
	find_package(LuaJIT REQUIRED)

	FetchContent_Declare(
		SDL3
		GIT_REPOSITORY "git@github.com:libsdl-org/SDL.git"
		GIT_TAG "release-3.2.14"
		GIT_PROGRESS TRUE

		FIND_PACKAGE_ARGS "3.2.14"
	)
	FetchContent_MakeAvailable(SDL3)

	add_subdirectory("${_LIBS_DIR}/sol2")

	target_include_directories(
		${target}
		PRIVATE
			"${_LIBS_DIR}"
			"${_LIBS_DIR}/cglm/include"
	)

	target_link_libraries(
		${target}
		PRIVATE
			SDL3::SDL3
			luajit::luajit
			sol2::sol2
	)

	if(UNIX)
		target_link_libraries(${target} PRIVATE m ${CMAKE_DL_LIBS})
	endif()
endfunction()
