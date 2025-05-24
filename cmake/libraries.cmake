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

	option(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS "" ON)
	option(SPIRV_CROSS_CLI "" OFF)
	option(SPIRV_CROSS_ENABLE_TESTS "" OFF)
	option(SPIRV_CROSS_ENABLE_GLSL "" ON)
	option(SPIRV_CROSS_ENABLE_HLSL "" OFF)
	option(SPIRV_CROSS_ENABLE_MSL "" OFF)
	option(SPIRV_CROSS_ENABLE_C_API "" OFF)
	option(SPIRV_CROSS_ENABLE_UTIL "" OFF)
	option(SPIRV_CROSS_SKIP_INSTALL "" ON)
	option(SPIRV_SKIP_EXECUTABLES "" ON)

	add_subdirectory(${_LIBS_DIR}/SPIRV-Cross)
	add_subdirectory(${_LIBS_DIR}/SPIRV-Headers)
	add_subdirectory(${_LIBS_DIR}/SPIRV-Tools)
	add_subdirectory(${_LIBS_DIR}/tint)

	target_include_directories(
		${target} PRIVATE
			${_LIBS_DIR}
			${_LIBS_DIR}/cglm/include
	)

	target_link_libraries(
		${target}
		PRIVATE
			spirv-cross-cpp
			SPIRV-Tools
			SDL3::SDL3
	)

	if(UNIX)
		target_link_libraries(${target} PRIVATE m ${CMAKE_DL_LIBS})
	endif()
endfunction()
