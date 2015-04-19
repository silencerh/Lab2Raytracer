# Top-Level CMakeList.txt

cmake_minimum_required (VERSION 2.6)
project ( Raytracer )
#set(LOCAL_LIBRARY &{PROJECT_SOURCE_DIR}/lib)
add_executable( Raytracer skeleton.cpp)

find_package (SDL)
#include_directories(&{PROJECT_SOURCE_DIR}/SDL/include)
#link_directories(&{PROJECT_SOURCE_DIR}/SDL/lib)
#target_link_libraries(SDL lib)
if ( NOT SDL_FOUND )
   message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )

if(SDL_FOUND)
	include_directories(
		${PROJECT_SOURCE_DIR}/SDL
		${PROJECT_SOURCE_DIR}/glm
	)
	#link_libraries(${SDL_LIBRARY})
	target_link_libraries(Raytracer ${SDL_LIBRARY})
endif(SDL_FOUND)

