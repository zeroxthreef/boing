# setup module config
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(SUB_SOURCES)
set(SUB_LIBRARIES)

# =====================
option(BUILD_OS "build the os module" ON)
option(BUILD_FFI "build the ffi module" ON)
option(BUILD_SCREEN "build the screen module" ON)
option(BUILD_EXAMPLE "build the example module" ON)

if(BUILD_OS)
	add_subdirectory(os)
endif()
if(BUILD_FFI)
	add_subdirectory(ffi)
endif()
if(BUILD_SCREEN)
	add_subdirectory(screen)
endif()
if(BUILD_EXAMPLE)
	add_subdirectory(example)
endif()
# add more here
# =====================

set(SOURCES ${SOURCES} ${SUB_SOURCES} PARENT_SCOPE)
set(LIBRARIES ${LIBRARIES} ${SUB_LIBRARIES} PARENT_SCOPE)