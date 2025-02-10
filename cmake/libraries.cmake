include_guard()

set(LIBS_PATH ${CMAKE_SOURCE_DIR}/ext)

function(setup_libraries target)
	# Find and link libraries
	find_package(SDL3 REQUIRED)

	option(CGLM_STATIC "" ON)
	add_subdirectory(${LIBS_PATH}/cglm)

	target_sources(
		${target} PRIVATE
			${LIBS_PATH}/sokol/sokol_gfx.h
			${LIBS_PATH}/glad/include/KHR/khrplatform.h
			${LIBS_PATH}/glad/include/glad/gl.h
			${LIBS_PATH}/glad/src/gl.c
	)

	target_include_directories(
		${target} PRIVATE
			${LIBS_PATH}/sokol
			${LIBS_PATH}/glad/include
	)

	target_link_libraries(${target} PRIVATE SDL3::SDL3 cglm)
	if(UNIX)
		target_link_libraries(${target} PRIVATE m ${CMAKE_DL_LIBS})
	endif()
endfunction()
