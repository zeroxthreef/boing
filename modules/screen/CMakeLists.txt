project(boing_screen)


if(LINK_MODULE_SCREEN OR BUILD_SCREEN)
	if(NOT EMSCRIPTEN)
		find_library(LIBSDL2 SDL2)
		if(NOT LIBSDL2)
			message(FATAL_ERROR "libSDL2 not installed")
		endif()
	endif()

	if(NOT EMSCRIPTEN)
		find_library(LIBSDL2_TTF SDL2_ttf)
		if(NOT LIBSDL2_TTF)
			message(FATAL_ERROR "libSDL2_ttf not installed")
		endif()
	endif()

	set(MODULE_SCREEN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/screen.c)


	if(EMSCRIPTEN)
		set(MODULE_SCREEN_LIBRARIES "-s USE_SDL=2" "-s USE_SDL_TTF=2")
	else()
		set(MODULE_SCREEN_LIBRARIES SDL2 SDL2_ttf)
	endif()
endif()


if(LINK_MODULE_SCREEN)
	message("module screen added to the interpreter")
	set(SUB_SOURCES ${SUB_SOURCES} ${MODULE_SCREEN_SOURCES} PARENT_SCOPE)
	set(SUB_LIBRARIES ${SUB_LIBRARIES} ${MODULE_SCREEN_LIBRARIES} PARENT_SCOPE)
endif()


if(BUILD_SCREEN)
	add_definitions(-DMODULE_STANDALONE)

	get_filename_component(BOING_ROOT ../../ ABSOLUTE)
	include_directories(${BOING_ROOT})
	
	add_library(boing_screen SHARED ${MODULE_SCREEN_SOURCES})
	target_link_libraries(boing_screen ${MODULE_SCREEN_LIBRARIES})
	install(TARGETS boing_screen DESTINATION lib/boing_modules)
endif()