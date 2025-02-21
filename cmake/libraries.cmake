include_guard()

set(_LIBS_DIR ${CMAKE_SOURCE_DIR}/ext)

function(setup_libraries target)
	# Find and link libraries
	find_package(glfw3 3.4 REQUIRED)

	option(CGLM_STATIC "" ON)
	add_subdirectory(${_LIBS_DIR}/cglm)

	target_sources(
		${target} PRIVATE
			${_LIBS_DIR}/sokol/sokol_gfx.h
			${_LIBS_DIR}/glad/include/KHR/khrplatform.h
			${_LIBS_DIR}/glad/include/glad/gl.h
			${_LIBS_DIR}/glad/src/gl.c
	)

	target_include_directories(
		${target} PRIVATE
			${_LIBS_DIR}/sokol
			${_LIBS_DIR}/glad/include
	)

	target_link_libraries(${target} PRIVATE glfw cglm)
	if(UNIX)
		target_link_libraries(${target} PRIVATE m ${CMAKE_DL_LIBS})
	endif()
endfunction()
