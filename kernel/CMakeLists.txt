set(KERNEL_SOURCE
	main.c
	panic.c
	printk.c
	sched.c
	timer.c
	trap.c
	uart.c
	vm.c
	init.S
	start.S
	syscall.c
	alloc.c
	sleeplock.c
	plic.c
	task.c
	ipc.c
	trampoline.S
	timer.S
	swtch.S)

add_subdirectory(servers)
add_subdirectory(sys)
add_subdirectory(lib)

set(KERNEL_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/kernel.ldS)

add_executable(kernel ${KERNEL_SOURCE})

target_include_directories(kernel
	PUBLIC ${PROJECT_SOURCE_DIR}/include
	PUBLIC ${NEWLIB_INCLUDES})
target_link_libraries(kernel sys lib)

target_link_options(kernel PRIVATE LINKER:-T ${KERNEL_LINKER_SCRIPT})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel DESTINATION ${CMAKE_INSTALL_PREFIX})

