set(USER_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/user.ldS")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T ${USER_LINKER_SCRIPT}")

add_executable(procmgr procmgr.c crt0.S)
target_include_directories(procmgr
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/procmgr" DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/sbin/")

