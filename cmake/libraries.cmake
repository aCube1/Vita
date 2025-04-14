include_guard()

set(_LIBS_DIR ${CMAKE_SOURCE_DIR}/ext)

function(setup_libraries target)
	# Find and link libraries
	add_subdirectory(${_LIBS_DIR}/glfw)

	target_include_directories(
		${target} PRIVATE
			${_LIBS_DIR}
			${_LIBS_DIR}/cglm/include
	)

	target_link_libraries(${target} PRIVATE glfw)
	if(UNIX)
		target_link_libraries(${target} PRIVATE m ${CMAKE_DL_LIBS})
	endif()
endfunction()
