cmake_minimum_required(VERSION 3.26)
project(
	sol2
	VERSION 3.5.0
	LANGUAGES C CXX
)

add_library(${PROJECT_NAME} INTERFACE)
add_library(sol2::sol2 ALIAS ${PROJECT_NAME})

target_include_directories(
	${PROJECT_NAME}
	INTERFACE
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(
	${PROJECT_NAME}
	INTERFACE
		"-DSOL_ALL_SAFETIES_ON=1"
)
