project(boing_example)



set(MODULE_EXAMPLE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/example.c)

set(MODULE_EXAMPLE_LIBRARIES)


if(LINK_MODULE_EXAMPLE)
	message("module example added to the interpreter")
	set(SUB_SOURCES ${SUB_SOURCES} ${MODULE_EXAMPLE_SOURCES} PARENT_SCOPE)
	set(SUB_LIBRARIES ${SUB_LIBRARIES} ${MODULE_EXAMPLE_LIBRARIES} PARENT_SCOPE)
endif()


if(BUILD_EXAMPLE)
	add_definitions(-DMODULE_STANDALONE)

	get_filename_component(BOING_ROOT ../../ ABSOLUTE)
	include_directories(${BOING_ROOT})

	add_library(boing_example SHARED ${MODULE_EXAMPLE_SOURCES})
	target_link_libraries(boing_example ${MODULE_EXAMPLE_LIBRARIES})
	install(TARGETS boing_example DESTINATION lib/boing_modules)
endif()