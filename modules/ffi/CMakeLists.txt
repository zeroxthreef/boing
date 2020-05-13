project(boing_ffi)



if(LINK_MODULE_FFI OR BUILD_FFI)
	find_library(LIBFFI ffi)
	if(NOT LIBFFI)
		message(FATAL_ERROR "libffi not installed")
	endif()

	set(MODULE_FFI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ffi.c)

	set(MODULE_FFI_LIBRARIES ffi)
endif()

if(LINK_MODULE_FFI)
	message("module ffi added to the interpreter")
	set(SUB_SOURCES ${SUB_SOURCES} ${MODULE_FFI_SOURCES} PARENT_SCOPE)
	set(SUB_LIBRARIES ${SUB_LIBRARIES} ${MODULE_FFI_LIBRARIES} PARENT_SCOPE)
endif()


if(BUILD_FFI)
	add_definitions(-DMODULE_STANDALONE)

	get_filename_component(BOING_ROOT ../../ ABSOLUTE)
	include_directories(${BOING_ROOT})

	add_library(boing_ffi SHARED ${MODULE_FFI_SOURCES})
	target_link_libraries(boing_ffi ${MODULE_FFI_LIBRARIES})
	install(TARGETS boing_ffi DESTINATION lib/boing_modules)
endif()