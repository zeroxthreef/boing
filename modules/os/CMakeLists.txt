project(boing_os)



set(MODULE_OS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/os.c)

set(MODULE_OS_LIBRARIES)


if(LINK_MODULE_OS)
	message("module os added to the interpreter")
	set(SUB_SOURCES ${MODULE_OS_SOURCES} PARENT_SCOPE)
	set(SUB_LIBRARIES ${MODULE_OS_LIBRARIES} PARENT_SCOPE)
endif()

add_definitions(-DMODULE_STANDALONE)

get_filename_component(BOING_ROOT ../../ ABSOLUTE)
include_directories(${BOING_ROOT})

add_library(boing_os SHARED ${MODULE_OS_SOURCES})
target_link_libraries(boing_os ${MODULE_OS_LIBRARIES})
install(TARGETS boing_os DESTINATION lib/boing_modules)