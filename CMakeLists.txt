cmake_minimum_required(VERSION 2.8)
project(boing)

option(LINK_MODULE_OS "link the os module with the interpreter binary" ON)

set(SOURCES ${SOURCES} host.c)
set(LIBRARIES m dl) # the base interpreter needs libm at least


if(WIN32)
	set(LIBRARIES ${LIBRARIES} ws2_32 shlwapi)
endif()

if(LINK_MODULE_OS)
	add_definitions(-DMODULE_OS_ADDED)
endif()


add_subdirectory(modules)

add_definitions("-DHOST_MODULE_LOCATION=\"${CMAKE_INSTALL_PREFIX}/lib/boing_modules\"")


add_executable(boing ${SOURCES})
target_link_libraries(boing ${LIBRARIES})

if(EMSCRIPTEN)
	# target_link_libraries(boing "-s EXPORTED_FUNCTIONS='[\"init_host\", \"destroy_host\", \"script_run\"]'")
	target_link_libraries(boing "-s EXPORT_ALL=1")
else()
	install(TARGETS boing DESTINATION bin)
endif()
