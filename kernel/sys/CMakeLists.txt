
add_library(sys STATIC
	sys_write.c
	)

target_include_directories(sys PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${NEWLIB_INCLUDES})
