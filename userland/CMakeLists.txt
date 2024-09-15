set(USER_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/user.ldS")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T ${USER_LINKER_SCRIPT}")

add_executable(init init.c crt0.S)
target_include_directories(init
	PUBLIC ${PROJECT_SOURCE_DIR}/include)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/init" DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/sbin/")

